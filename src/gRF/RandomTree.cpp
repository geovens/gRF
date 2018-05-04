
#include "RandomTree.h"
#include "Data.h"
#include "LinkerPointer.h"
#include "LinkerIndexer.h"
#include "LinkerIndexerFile.h"
#include "Node.h"
#include "Function.h"
#include "stdafx.h"

RandomTree::RandomTree()
{
	ID = 0;
	MaxThreadNumber = 4;
	CandidatesEachNode = 1000;
	ThreadCount = 0;
	//IsWritingNode = 0;
}

RandomTree::~RandomTree()
{
}

int RandomTree::RecursionNewChildNodes(Node* node)
{
	int r = Grow(node);

	if (r == 0)
	{
		RecursionNewChildNodes(node->Left);
		RecursionNewChildNodes(node->Right);
	}

	return 0;
}

int RandomTree::Plant()
{
	Root = new Node();
	Root->Index = 0;
	Root->Level = 0;
	Root->Tree = this;
	Root->Parent = NULL;
	Depth = 0;
	return 0;
}

int RandomTree::Grow(Node* node)
{
	if (node->Left != NULL && node->Right != NULL)
		return -1;
	if (node->Level == MaxDepth)
	{
		node->IsLeaf = true;
		return 1;
	}
	Node* lnode = new Node();
	Node* rnode = new Node();
	lnode->Tree = this;
	rnode->Tree = this;
	lnode->Index = node->Index * 2 + 0;
	rnode->Index = node->Index * 2 + 1;
	node->Left = lnode;
	node->Right = rnode;
	lnode->Parent = node;
	rnode->Parent = node;
	lnode->Level = node->Level + 1;
	rnode->Level = node->Level + 1;
	if (node->Level + 1 > Depth)
		Depth = node->Level + 1;
	return 0;
}

int RandomTree::PlantGrowFull(int depth)
{
	if (depth > 25)
		return -1;
	MaxDepth = depth;
	Plant();
	RecursionNewChildNodes(Root);
	return 0;
}

int RandomTree::RecursionSplitTrainingSet(Node* node)
{
	if (node->Trained)
	{
		if (!node->IsLeaf)
		{
			Grow(node);
			node->Left->ThisData = node->ThisData;
			node->Right->ThisData = node->ThisData;
			if (node->Level <= 4)
			{
				for (int i = 0; i < node->Level; i++)
					printf("   ");
				printf("trained previously\n");
			}
			RecursionSplitTrainingSet(node->Left);
			RecursionSplitTrainingSet(node->Right);
		}
	}
	else
	{
		// TODO: consider disable vote on non-leaf nodes to speed up!
		if (node->ThisDataPointers == NULL)
			LoadDataPointers(node);
		node->Vote();
		double maxhist = 0;
		for (int k = 0; k < ThisData->K; k++)
		if (node->HistLabels[k] > maxhist)
			maxhist = node->HistLabels[k];
		//if ((1 - maxhist) * node->ThisDataPointers->N < ThisData->N / ThisData->K / 20 || node->Level == MaxDepth || node->IsLeaf)
		if (node->Level == MaxDepth || node->IsLeaf)
		{
			node->IsLeaf = true;
			//node->Vote();
			node->Trained = 1;
			WriteNode(node);
			node->ThisDataPointers->Release();
			return 0;
		}
		int rec;
		if (node->Type != 1)
			rec = node->SplitManyTimes(CandidatesEachNode);
		else
			rec = node->SplitManyTimes(1);
		if (rec == 0)
		{
			Grow(node);

			//DataPointers** lrdata = node->ThisDataPointers->NewChildren();
			Linker** lrdata = node->ThisDataPointers->Split();
			node->Left->ThisDataPointers = *lrdata;
			node->Right->ThisDataPointers = *(lrdata + 1);
			node->Left->ThisData = node->ThisData;
			node->Right->ThisData = node->ThisData;
			node->Left->ThisDataPointers->ThisNode = node->Left;
			node->Right->ThisDataPointers->ThisNode = node->Right;
			if (node->Level <= 4)
			{
				for (int i = 0; i < node->Level; i++)
					printf("   ");
				printf("%d -> %d, %d", node->ThisDataPointers->N, node->Left->ThisDataPointers->N, node->Right->ThisDataPointers->N);
				//printf("      class 0: %d -> %d, %d", node->ThisDataPointers->LabelCount[0], node->Left->ThisDataPointers->LabelCount[0], node->Right->ThisDataPointers->LabelCount[0]);
				// Calculate the contribution of each node, but only valid if there are only two classes!!
				int thismin = node->ThisData->N;
				int leftmin = node->ThisData->N;
				int rightmin = node->ThisData->N;
				int rootmin = node->ThisData->N;
				for (int k = 0; k < node->ThisData->K; k++)
				{
					if (node->ThisDataPointers->LabelCount[k] < thismin) thismin = node->ThisDataPointers->LabelCount[k];
					if (node->Left->ThisDataPointers->LabelCount[k] < leftmin) leftmin = node->Left->ThisDataPointers->LabelCount[k];
					if (node->Right->ThisDataPointers->LabelCount[k] < rightmin) rightmin = node->Right->ThisDataPointers->LabelCount[k];
					if (Root->ThisDataPointers->LabelCount[k] < rootmin) rootmin = Root->ThisDataPointers->LabelCount[k];
				}
				//printf("      contribution: %.3lf", (double)(thismin - leftmin - rightmin) / rootmin);
				printf("\n");
			}

			node->Trained = 1;
			WriteNode(node);

			if (node->Left->ThisDataPointers->N == 0 || node->Right->ThisDataPointers->N == 0)
			{
				printf("WTFWTF???");
			}

			if (node != Root)
				node->ThisDataPointers->Release();

			if (node->Type != 1)
			{
				if (ThreadCount >= MaxThreadNumber)
				{
					RecursionSplitTrainingSet(node->Left);
				}
				else
				{
					int** hp = new int*[2];
					hp[0] = (int*)this;
					hp[1] = (int*)(node->Left);
					new std::thread(NewThread, hp);
				}
			}
			else
			{
				node->Left->IsLeaf = true;
				for (int k = 0; k < ThisData->K; k++)
					node->HistLabels[k] = 0;
				node->Left->Trained = 1;
				WriteNode(node->Left);
				node->Left->ThisDataPointers->Release();
			}
			RecursionSplitTrainingSet(node->Right);
		}
		else
		{
			node->IsLeaf = true;
			//node->Vote();
			node->Trained = 1;
			WriteNode(node);
			node->ThisDataPointers->Release();
		}
		return 0;
	}
}

int RandomTree::NewThread(void* lpParameter)
{
	int* hp[2];
	memcpy(hp, lpParameter, 2 * sizeof(int*));
	RandomTree* tree = (RandomTree*)hp[0];
	Node* node = (Node*)hp[1];

	srand(time(NULL));
	//srand(node->Index);

	tree->ThreadCount_mutex.lock();
	tree->ThreadCount++;
	tree->ThreadCount_mutex.unlock();
	tree->RecursionSplitTrainingSet(node);
	tree->ThreadCount_mutex.lock();
	tree->ThreadCount--;
	tree->ThreadCount_mutex.unlock();
	return 0;
}

Node* RandomTree::FindNode(int level, unsigned long long index)
{
	Node* n = Root;
	while (n->Level < level)
	{
		if (n->Left == NULL || n->Right == NULL)
			Grow(n);
		int i = (index >> (level - n->Level - 1)) & 0x1;
		if (i == 1)
			n = n->Right;
		else
			n = n->Left;
	}
	return n;
}

int RandomTree::LoadDataPointers(Node* node)
{
	if (PointersMode == 1)
	{
		LinkerPointer* loader = new LinkerPointer();
		loader->Load(node);
		node->ThisDataPointers = (Linker*)loader;
	}
	else if (PointersMode == 2)
	{
		LinkerIndexerFile* loader = new LinkerIndexerFile();
		loader->Load(node);
		node->ThisDataPointers = (Linker*)loader;
	}
	else if (PointersMode == 3)
	{
		LinkerIndexer* loader = new LinkerIndexer();
		loader->Load(node);
		node->ThisDataPointers = (Linker*)loader;
	}
	return 0;
}

int RandomTree::ReadTrainingProcess()
{
	FILE* ftree;
	ftree = fopen(".\\output\\tree.txt", "r+");
	if (ftree == NULL)
		return 0;

	int depth, abcnum, d, n, k;
	fscanf(ftree, "%d,%d,%d,%d,%d\n", &depth, &n, &k, &d, &abcnum);
	if (n != ThisData->N)
		return -1;
	if (k != ThisData->K)
		return -1;
	if (d != ThisData->D)
		return -1;
	if (abcnum != Function->ABCNum)
		return -1;
	fclose(ftree);

	int red = ReadNodeFile();
	if (red == -1)
		return 0;
	else
		return 1;
}

int RandomTree::ReadNodeFile()
{
	char nodefiletext[1024];
	sprintf(nodefiletext, ".\\output\\nodes-%d.txt", ID);
	FILE* fnode;
	fnode = fopen(nodefiletext, "r+");
	if (fnode == NULL)
		return -1;

	int K;
	int ABCNum;
	char line[2048];
	fgets(line, 2048, fnode);
	sscanf(line, "%d,%d", &K, &ABCNum);
	int sumbyt;
	int byt;
	int linenum = 0;
	// test
	int abc5[10];
	memset(abc5, 0, 40);
	while (!feof(fnode))
	{
		linenum++;
		sumbyt = 0;
		fgets(line, 2048, fnode);
		if (strlen(line) < 10)
			printf("ERROR: nodes.txt crupted at line %d\n", linenum);
		int level, leaf, major;
		unsigned long long index;
		int re = sscanf(line, "%d,%lld,%d,%d,%n", &level, &index, &leaf, &major, &sumbyt);
		if (re != 4)
			continue;
		Node* node = FindNode(level, index);
		if (leaf == 1)
			node->IsLeaf = true;
		node->MajorLabel = major;
		node->HistLabels = new double[K];
		for (int k = 0; k < K; k++)
			node->HistLabels[k] = 0;
		node->SampleReachedCount = new int[K];
		for (int k = 0; k < K; k++)
			node->SampleReachedCount[k] = 0;
		node->ABC = new featuretype[Function->ABCNum];
		for (int k = 0; k < K; k++)
		{
			double r;
			re = sscanf(line + sumbyt, "%le,%n", &r, &byt);
			if (re != 1)
			{
				printf("ERRORw1, linenum=%d\n", linenum);
				continue;
			}
			sumbyt += byt;
			node->HistLabels[k] = r;
		}
		for (int k = 0; k < K; k++)
		{
			int r;
			re = sscanf(line + sumbyt, "%d,%n", &r, &byt);
			if (re != 1)
			{
				printf("ERRORxey, linenum=%d\n", linenum);
				continue;
			}
			sumbyt += byt;
			node->SampleReachedCount[k] = r;
		}
		for (int a = 0; a < ABCNum; a++)
		{
			double r;
			re = sscanf(line + sumbyt, "%le,%n", &r, &byt);
			if (re != 1)
			{
				printf("ERRORuqq, linenum=%d\n", linenum);
				continue;
			}
			sumbyt += byt;
			node->ABC[a] = r;

			// test
			//if (!node->IsLeaf && a == 4) abc5[(int)r]++;  // CAUSEING TROUBLE!!!
		}
		node->Trained = 1;
	}

	// test
	printf("%d, %d, %d, %d, %d, %d, %d\n", abc5[0], abc5[1], abc5[2], abc5[3], abc5[4], abc5[5]);

	fclose(fnode);
	return 0;
}

int RandomTree::WriteNode(Node* node)
{
//again:
//	while (IsWritingNode > 0)
//		Sleep(10);
//	IsWritingNode++;

	char* line = new char[10000];
	sprintf(line, "%d,%lld,%d,%d,", node->Level, node->Index, node->IsLeaf ? 1 : 0, node->MajorLabel);
	for (int k = 0; k < ThisData->K; k++)
		sprintf(line + strlen(line), "%le,", node->HistLabels != NULL ? node->HistLabels[k] : 0.0);
	for (int k = 0; k < ThisData->K; k++)
		sprintf(line + strlen(line), "%d,", node->SampleReachedCount != NULL ? node->SampleReachedCount[k] : 1);
	for (int a = 0; a < Function->ABCNum; a++)
		sprintf(line + strlen(line), "%le,", node->ABC != NULL ? node->ABC[a] : 0.0);
	sprintf(line + strlen(line), "\n");

	/*if (IsWritingNode > 1)
	{
		delete line;
		IsWritingNode--;
		goto again;
	}*/

	//FILE* fnode;
	//fnode = fopen(".\\output\\nodes.txt", "a+");
	//if (fnode == NULL)
	//	fnode = fopen(".\\output\\nodes.txt", "w+");
	fprintf(FNode, "%s", line);
	fflush(FNode);
	//fclose(fnode);
	delete line;

	//IsWritingNode--;
	return 0;
}

int RandomTree::Train(Data* data, int pointermode)
{
	ThisData = data;
	Root->ThisData = data;
	ThisData->CalLabelPercentage();
	PointersMode = pointermode;
	int rec = ReadTrainingProcess();
	if (rec == 0)
	{
		printf("starting a new training process.\n\n");
		FILE* ftree;
		ftree = fopen(".\\output\\tree.txt", "w");
		fprintf(ftree, "%d,%d,%d,%d,%d\n", MaxDepth, ThisData->N, ThisData->K, ThisData->D, Function->ABCNum);
		fprintf(ftree, "%d,%d\n", CandidatesEachNode, pointermode);
		fclose(ftree);
		char nodefiletext[1024];
		sprintf(nodefiletext, ".\\output\\nodes-%d.txt", ID);
		FNode = fopen(nodefiletext, "w");
		fprintf(FNode, "%d,%d\n", data->K, Function->ABCNum);

		Linker* datapointers;
		if (pointermode == 1)
			datapointers = new LinkerPointer();
		else if (pointermode == 2)
			datapointers = new LinkerIndexerFile();
		else
			datapointers = new LinkerIndexer();
		datapointers->InitFromData(data);
		Root->ThisDataPointers = datapointers;
		Root->ThisDataPointers->ThisNode = Root;
		//RecursionSplitTrainingSet(Root);
		int** hp = new int*[2];
		hp[0] = (int*)this;
		hp[1] = (int*)Root;
		//HANDLE hThread;
		//hThread = CreateThread(NULL, 0, NewThread, hp, 0, NULL);
		//CloseHandle(hThread);
		//Sleep(100);
		new std::thread(NewThread, hp);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (ThreadCount > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		fclose(FNode);
		printf("\ntraining finished.\n");
		return 0;
	}
	else if (rec == 1)
	{
		printf("resuming previous training process.\n\n");
		char nodefiletext[1024];
		sprintf(nodefiletext, ".\\output\\nodes-%d.txt", ID);
		FNode = fopen(nodefiletext, "a+");

		//RecursionSplitTrainingSet(Root);
		int** hp = new int*[2];
		hp[0] = (int*)this;
		hp[1] = (int*)Root;
		//HANDLE hThread;
		//hThread = CreateThread(NULL, 0, NewThread, hp, 0, NULL);
		//CloseHandle(hThread);
		//Sleep(100);
		new std::thread(NewThread, hp);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (ThreadCount > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		fclose(FNode);
		printf("\ntraining finished.\n");
		return 0;
	}
	else if (rec == -1)
	{
		printf("ERROR: previous unfinished training process is detected, \n");
		printf("       however the params of it are not consistent with the current settings\n");
		printf("       thus resuming of the previous training process is failed.\n");
		printf("       if you want to start a new training process, \n");
		printf("       please delete all previous training files located at .\\output\\\n\n");
		return -1;
	}
	return -2;
}

double RandomTree::Test(Data* data)
{
	ThisData = data;
	ThisData->CalLabelPercentage();

	int correct = 0;
	//data->Predictions = new Node*[data->N];
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < data->N; i++)
	{
		Node* node = TestFeature(i, feature_temp_store);
		data->SetReachedNode(i, node);
		labeltype lb;
		data->GetLabel(i, &lb);
		if (node->MajorLabel == lb)
			correct++;
	}
	delete feature_temp_store;
	return (double)correct / data->N;
}

double RandomTree::Test(Data* data, int level)
{
	ThisData = data;
	ThisData->CalLabelPercentage();

	int correct = 0;
	//data->Predictions = new Node*[data->N];
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < data->N; i++)
	{
		Node* node = TestFeature(i, level, feature_temp_store);
		data->SetReachedNode(i, node);
		labeltype lb;
		data->GetLabel(i, &lb);
		if (node->MajorLabel == lb)
			correct++;
	}
	delete feature_temp_store;
	return (double)correct / data->N;
}

Node* RandomTree::TestFeature(featuretype* feature)
{
	Node* node = Root;
	while (!node->IsLeaf)
	{
		//Function->Data = node->Data;
		int flag = Function->TestFeature(feature, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	return node;
}

Node* RandomTree::TestFeature(featuretype* feature, int level)
{
	Node* node = Root;
	while (!node->IsLeaf && node->Level < level)
	{
		//Function->Data = node->Data;
		if (node->ABC == NULL)
			return NULL;
		int flag = Function->TestFeature(feature, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	return node;
}


Node* RandomTree::TestFeature(int index, featuretype* feature_temp_store)
{
	Node* node = Root;
	while (!node->IsLeaf)
	{
		if (node->ABC == NULL)
			return NULL;
		ThisData->GetFeature(index, node->ABC, feature_temp_store);
		int flag = Function->TestFeature(feature_temp_store, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	return node;
}


Node* RandomTree::TestFeature(int index, int level, featuretype* feature_temp_store)
{
	Node* node = Root;
	while (!node->IsLeaf && node->Level < level)
	{
		if (node->ABC == NULL)
			return NULL;
		ThisData->GetFeature(index, node->ABC, feature_temp_store);
		//if (node->Trained == 0)
		//	return node;
		int flag = Function->TestFeature(feature_temp_store, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	return node;
}

bool RandomTree::TestFeatureReach(int index, Node* srcnode, featuretype* feature_temp_store)
{
	Node* node = Root;
	while (!node->IsLeaf && node->Level < srcnode->Level)
	{
		if (node->ABC == NULL)
			return false;
		ThisData->GetFeature(index, node->ABC, feature_temp_store);
		if (node->Trained == 0)
			return false;
		int flag = Function->TestFeature(feature_temp_store, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
		if (srcnode->Index >> (srcnode->Level - node->Level) != node->Index)
			return false;
	}
	if (node == srcnode)
		return true;
	else
		return false;
}