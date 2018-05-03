
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
	CandidatesEachNode = 1000;
	MaxThreadNumber = 4;
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
		if (node->ThisDataPointers == NULL)
			LoadDataPointers(node);

		node->N = node->ThisDataPointers->N;
		if (node->Level == MaxDepth || node->IsLeaf || node->ThisDataPointers->N < 2)
		{
			node->IsLeaf = true;
			node->Vote();
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
				printf("%d -> %d, %d\n", node->ThisDataPointers->N, node->Left->ThisDataPointers->N, node->Right->ThisDataPointers->N);
			}

			node->Trained = 1;
			WriteNode(node);

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
					/*HANDLE hThread;
					hThread = CreateThread(NULL, 0, NewThread, hp, 0, NULL);
					CloseHandle(hThread);*/
					new std::thread(NewThread, hp);
				}
			}
			else
			{
				node->Left->IsLeaf = true;
				node->Left->Trained = 1;
				WriteNode(node->Left);
				node->Left->ThisDataPointers->Release();
			}
			RecursionSplitTrainingSet(node->Right);
		}
		else
		{
			node->IsLeaf = true;
			node->Vote();
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

	int depth, abcnum, d, n;
	fscanf(ftree, "%d,%d,%d,%d,%d\n", &depth, &n, &d, &abcnum);
	if (n != ThisData->N)
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

	int ABCNum;
	char line[2048];
	fgets(line, 2048, fnode);
	sscanf(line, "%d", &ABCNum);
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
		int level, leaf, n;
		unsigned long long index;
		double ave;
		int re = sscanf(line, "%d,%lld,%d,%le,%d,%n", &level, &index, &leaf, &ave, &n, &sumbyt);
		if (re != 5)
			continue;
		Node* node = FindNode(level, index);
		if (leaf == 1)
			node->IsLeaf = true;
		node->AverageValue = ave;
		node->N = n;

		node->ABC = new featuretype[Function->ABCNum];
	
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
	printf("Reading trained data of tree #%d\n", ID);

	fclose(fnode);
	return 0;
}

int RandomTree::WriteNode(Node* node)
{
//again:
//	while (IsWritingNode > 0)
//		Sleep(10);
//	IsWritingNode++;

	char* line = new char[2048];
	sprintf(line, "%d,%lld,%d,%le,%d,", node->Level, node->Index, node->IsLeaf ? 1 : 0, node->AverageValue, node->N);
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

int RandomTree::Train(Data* data, int linkermode)
{
	ThisData = data;
	Root->ThisData = data;
	PointersMode = linkermode;
	int rec = ReadTrainingProcess();
	if (rec == 0)
	{
		printf("starting a new training process.\n\n");
		FILE* ftree;
		ftree = fopen(".\\output\\tree.txt", "w");
		fprintf(ftree, "%d,%d,%d,%d\n", MaxDepth, ThisData->N, ThisData->D, Function->ABCNum);
		fprintf(ftree, "%d,%d\n", CandidatesEachNode, linkermode);
		fclose(ftree);
		char nodefiletext[1024];
		sprintf(nodefiletext, ".\\output\\nodes-%d.txt", ID);
		FNode = fopen(nodefiletext, "w");
		fprintf(FNode, "%d\n", Function->ABCNum);

		Linker* datapointers;
		if (linkermode == 1)
		{
			// This linker is only appliable for fixed features of finite dimension, not for dynamicly calculated features using abc.
			datapointers = new LinkerPointer();
		}
		else if (linkermode == 2)
		{
			// use file instead of memory
			datapointers = new LinkerIndexerFile();
		}
		else if (linkermode == 3)
		{
			// this should be the default linker
			datapointers = new LinkerIndexer();
		}
		else
		{
			// this should be the default linker
			datapointers = new LinkerIndexer();
		}

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
		printf("ERROR: Previous (either finished or unfinished) training process is detected,\n");
		printf("       however some settings of it are not consistent with the current settings,\n");
		printf("       thus resuming of the previous training process has failed.\n");
		printf("       If you want to start a new training process, \n");
		printf("       please delete all previous training files located at ./output/\n\n");
		return -1;
	}
	return -2;
}

int RandomTree::Test(Data* data)
{
	ThisData = data;


	//data->Predictions = new Node*[data->N];
	int EI = 0, EInode = 0, eit = 0;
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < data->N; i++)
	{
		Node* node = TestFeature(i, &eit, feature_temp_store);
		data->SetReachedNode(i, node, &EInode);
	}
	delete feature_temp_store;
	return 0;
}

int RandomTree::Test(Data* data, int level)
{
	ThisData = data;

	//data->Predictions = new Node*[data->N];
	int EI = 0, EInode = 0, eit = 0;
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < data->N; i++)
	{
		Node* node = TestFeature(i, level, &eit, feature_temp_store);
		data->SetReachedNode(i, node, &EInode);
	}
	delete feature_temp_store;
	return 0;
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


Node* RandomTree::TestFeature(int index, int* ei, featuretype* feature_temp_store)
{
	Node* node = Root;
	int ei2;
	while (!node->IsLeaf)
	{
		if (node->ABC == NULL)
			return NULL;
		ei2 = *ei;
		ThisData->GetFeature(index, node->ABC, feature_temp_store, &ei2);
		int flag = Function->TestFeature(feature_temp_store, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	*ei = ei2;
	return node;
}


Node* RandomTree::TestFeature(int index, int level, int* ei, featuretype* feature_temp_store)
{
	Node* node = Root;
	int ei2;
	while (!node->IsLeaf && node->Level < level)
	{
		if (node->ABC == NULL)
			return NULL;
		ei2 = *ei;
		ThisData->GetFeature(index, node->ABC, feature_temp_store, &ei2);
		//if (node->Trained == 0)
		//	return node;
		int flag = Function->TestFeature(feature_temp_store, node->ABC);
		if (flag == 0)
			node = node->Left;
		else
			node = node->Right;
	}
	*ei = ei2;
	return node;
}

bool RandomTree::TestFeatureReach(int index, Node* srcnode, int* ei, featuretype* feature_temp_store)
{
	Node* node = Root;
	while (!node->IsLeaf && node->Level < srcnode->Level)
	{
		if (node->ABC == NULL)
			return false;
		ThisData->GetFeature(index, node->ABC, feature_temp_store, ei);
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