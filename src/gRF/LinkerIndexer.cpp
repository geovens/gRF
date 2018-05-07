
#include "LinkerIndexer.h"
#include "Data.h"
#include "Node.h"
#include "RandomTree.h"
#include "stdafx.h"

LinkerIndexer::LinkerIndexer()
{
	N = 0;
	Indexes = NULL;
	SplitFlags = NULL;
	IndexesMemoryAlloCount = 0;
	LabelCount = NULL;
	ChildrenLabelCount = NULL;
}

int LinkerIndexer::InitFromData(Data* data)
{
	ThisData = data;
	N = 0;
	IndexesMemoryAlloCount = 0;
	Indexes = NULL;
	LabelCount = new int[data->K];
	memset(LabelCount, 0, data->K * sizeof(int));

	int ei = 0;
	for (int i = 0; i < data->N; i++)
	{
		labeltype lp;
		data->GetLabel(i, &lp);
		AddIndex(i);
		LabelCount[lp]++;
	}

	return 0;
}

int LinkerIndexer::AddIndex(int index)
{
	if (N >= IndexesMemoryAlloCount)
	{
		int* newindexes = new int[IndexesMemoryAlloCount * 2 + 1];
		memcpy(newindexes, Indexes, N * sizeof(int));
		if (Indexes != NULL)
			delete Indexes;
		Indexes = newindexes;
		IndexesMemoryAlloCount = IndexesMemoryAlloCount * 2 + 1;
	}
	Indexes[N] = index;
	N++;
	return 0;
}

int LinkerIndexer::FastInit()
{
	GetFeatureindex = 0;
	GetFeatureLabelindex = 0;
	GetLabelindex = 0;
	SetSplitFlagindex = 0;
	return 0;
}

int LinkerIndexer::FastClose()
{
	return 0;
}

//featuretype* DataIndexer::GetFeatureNext(featuretype* abc)
//{
//	featuretype* r = *GetFeaturep;
//	GetFeaturep++;
//	return r;
//}
labeltype LinkerIndexer::GetLabelNext()
{
	labeltype r;
	ThisData->GetLabel(Indexes[GetLabelindex], &r);
	GetLabelindex++;
	return r;
}
void LinkerIndexer::GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out)
{
	//featuretype* feature;
	//labeltype label;
	ThisData->GetFeatureLabel(Indexes[GetFeatureindex], abc, feature_out, label_out);
	GetFeatureindex++;
}
void LinkerIndexer::SetSplitFlagNext(char flag)
{
	SplitFlags[SetSplitFlagindex] = flag;
	SetSplitFlagindex++;
}

//char LinkerIndexer::GetSplitFlagNext()
//{
//	int flag = 0;
//	int r = fread(&flag, 1, 1, SplitFlagsFile);
//	if (r != 1)
//		printf("ERROR: fread failed in DataPointersFile::GetSplitFlagNext()\n");
//	/*int flag;
//	flag = fgetc(SplitFlagsFile);*/
//	return flag;
//}

//int DataPointersFile::NewWriteIndex()
//{
//	if (Indexes != NULL)
//		fclose(Indexes);
//	if (SplitFlagsFile != NULL)
//		fclose(SplitFlagsFile);
//	Indexes = fopen(strstr(IndexesFileName, "indexes.bin"), "r+");
//	SplitFlagsFile = fopen(strstr(IndexesFileName, "splitflags.bin"), "r+");
//}
//
//int DataPointersFile::NewReadIndex()
//{
//	if (Indexes != NULL)
//		fclose(Indexes);
//	if (SplitFlagsFile != NULL)
//		fclose(SplitFlagsFile);
//	Indexes = fopen(strstr(IndexesFileName, "indexes.bin"), "r");
//	SplitFlagsFile = fopen(strstr(IndexesFileName, "splitflags.bin"), "r");
//}

//int DataPointersFile::AddIndexFast(int index)
//{
//	fwrite(&index, 4, 1, Indexes);
//}

//int DataPointersFile::GetIndexFast()
//{
//	int index;
//	fread(&index, 4, 1, Indexes);
//	return index;
//}

//int DataPointers::NewSplitFlags()
//{
//	SplitFlags = new int[N];
//	memset(SplitFlags, 0, N * sizeof(int));
//	return 0;
//}

//int DataPointers::GetLabelCount(int k)
//{
//	return LabelCount[k];
//}

//DataPointers** DataPointers::NewChildren()
//{
//	DataPointers** child = new DataPointers*[2];
//	child[0] = new DataPointers();
//	child[1] = new DataPointers();
//	return child;
//}

int LinkerIndexer::NewSplitFlags()
{
	SplitFlags = new char[N];
	memset(SplitFlags, 0, N * sizeof(char));
	return 0;
}
Linker** LinkerIndexer::Split()
{
	LinkerIndexer** child = new LinkerIndexer*[2];
	child[0] = new LinkerIndexer();
	child[1] = new LinkerIndexer();

	ChildrenN[0] = 0;
	ChildrenN[1] = 0;

	for (int i = 0; i < N; i++)
		ChildrenN[SplitFlags[i]]++;

	for (int i = 0; i < 2; i++)
	{
		//child[i]->D = D;
		//child[i]->K = K;
		child[i]->ThisData = ThisData;
		child[i]->N = ChildrenN[i];
		child[i]->Info = Info;
		child[i]->Indexes = new int[child[i]->N];
		child[i]->LabelCount = new int[ThisData->K];
		memset(child[i]->LabelCount, 0, sizeof(int) * ThisData->K);
	}

	int* childfp[2];
	childfp[0] = child[0]->Indexes;
	childfp[1] = child[1]->Indexes;
	int* fp = Indexes;
	for (int i = 0; i < N; i++)
	{
		char flag = SplitFlags[i];
		*childfp[flag] = *fp;
		labeltype lb;
		ThisData->GetLabel(*fp, &lb);
		child[flag]->LabelCount[lb]++;
		childfp[flag]++;
		fp++;
	}

	return (Linker**)child;
}

int LinkerIndexer::Load(Node* node)
{
	//printf("Debug: Loading node %d - %d ...\n", node->Level, node->Index);
	ThisNode = node;
	ThisData = node->ThisData;
	LabelCount = new int[node->ThisData->K];
	memset(LabelCount, 0, node->ThisData->K * sizeof(int));

	FastInit();
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < ThisData->N; i++)
	{
		//featuretype* feature = ThisData->GetFeatureP(i, NULL, &GetFeaturePEI);
		//Node* n = node->Tree->TestFeature(i, node->Level);
		//if (n == node)
		if (node->Tree->TestFeatureReach(i, node, feature_temp_store))
		{
			AddIndex(i);
		}
	}
	delete feature_temp_store;
	FastClose();

	FastInit();
	for (int i = 0; i < N; i++)
		LabelCount[GetLabelNext()]++;
	FastClose();
	//printf("Debug: Loading node %d - %d finished\n", node->Level, node->Index);
	return 0;
}

int LinkerIndexer::Release()
{
	if (Indexes != NULL)
		delete Indexes;
	if (SplitFlags != NULL)
		delete SplitFlags;
	if (LabelCount != NULL)
		delete LabelCount;
	delete this;
	return 0;
}