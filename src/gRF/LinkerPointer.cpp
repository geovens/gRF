
#include "LinkerPointer.h"
#include "Data.h"
#include "Node.h"
#include "RandomTree.h"
#include "stdafx.h"

LinkerPointer::LinkerPointer()
{
	N = 0;
	PointerMemoryAlloCount = 0;
	FeaturePointers = NULL;
	//LabelPointers = NULL;
}

int LinkerPointer::InitFromData(Data* data)
{
	ThisData = data;
	N = 0;
	PointerMemoryAlloCount = 0;
	FeaturePointers = NULL;
	//LabelPointers = NULL;
	LabelCount = new int[data->K];
	memset(LabelCount, 0, data->K * sizeof(int));

	FastInit();
	for (int i = 0; i < data->N; i++)
	{
		featuretype* fp;
		labeltype lp;
		fp = data->GetFeatureP(i, NULL);
		data->GetLabel(i, &lp);
		AddPointer(fp, lp);
		LabelCount[lp]++;
	}
	FastClose();

	return 0;
}

int LinkerPointer::AddPointer(featuretype* feature, labeltype label)
{
	if (N >= PointerMemoryAlloCount)
	{
		featuretype** newfpointers = new featuretype*[PointerMemoryAlloCount * 2 + 1];
		memcpy(newfpointers, FeaturePointers, N * sizeof(featuretype*));
		if (FeaturePointers != NULL)
			delete FeaturePointers;
		FeaturePointers = newfpointers;

		labeltype* newlpointers = new labeltype[PointerMemoryAlloCount * 2 + 1];
		memcpy(newlpointers, Labels, N * sizeof(labeltype));
		if (Labels != NULL)
			delete Labels;
		Labels = newlpointers;

		PointerMemoryAlloCount = PointerMemoryAlloCount * 2 + 1;
	}
	FeaturePointers[N] = feature;
	Labels[N] = label;
	N++;
	return 0;
}

int LinkerPointer::FastInit()
{
	GetFeaturep = FeaturePointers;
	GetLabelp = Labels;
	SetSplitFlagp = SplitFlags;
	return 0;
}

int LinkerPointer::FastClose()
{
	return 0;
}

//featuretype* DataPointers::GetFeatureNext(featuretype* abc)
//{
//	featuretype* r = *GetFeaturep;
//	GetFeaturep++;
//	return r;
//}
labeltype LinkerPointer::GetLabelNext()
{
	labeltype r = *GetLabelp;
	GetLabelp++;
	return r;
}
void LinkerPointer::GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out)
{
	//*feature_out = *GetFeaturep;
	memcpy(feature_out, *GetFeaturep, sizeof(featuretype)* ThisData->D);
	*label_out = *GetLabelp;
	GetFeaturep++;
	GetLabelp++;
}
void LinkerPointer::SetSplitFlagNext(char flag)
{
	*SetSplitFlagp = flag;
	SetSplitFlagp++;
}

//Linker** LinkerPointer::NewChildren()
//{
//
//}

int LinkerPointer::NewSplitFlags()
{
	SplitFlags = new char[N];
	memset(SplitFlags, 0, N * sizeof(char));
	return 0;
}

Linker** LinkerPointer::Split()
{
	LinkerPointer** child = new LinkerPointer*[2];
	child[0] = new LinkerPointer();
	child[1] = new LinkerPointer();

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
		child[i]->FeaturePointers = new featuretype*[child[i]->N];
		child[i]->Labels = new labeltype[child[i]->N];
		child[i]->LabelCount = new int[ThisData->K];
		memset(child[i]->LabelCount, 0, sizeof(int) * ThisData->K);
	}

	featuretype** childfp[2];
	childfp[0] = child[0]->FeaturePointers;
	childfp[1] = child[1]->FeaturePointers;
	featuretype** fp = FeaturePointers;
	labeltype* childlp[2];
	childlp[0] = child[0]->Labels;
	childlp[1] = child[1]->Labels;
	labeltype* lp = Labels;
	for (int i = 0; i < N; i++)
	{
		char flag = SplitFlags[i];
		*childfp[flag] = *fp;
		*childlp[flag] = *lp;
		child[flag]->LabelCount[*lp]++;
		childfp[flag]++;
		childlp[flag]++;
		fp++;
		lp++;
	}

	return (Linker**)child;
}

int LinkerPointer::Load(Node* node)
{
	ThisNode = node;
	ThisData = node->ThisData;
	LabelCount = new int[node->ThisData->K];
	memset(LabelCount, 0, node->ThisData->K * sizeof(int));

	FastInit();
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < ThisData->N; i++)
	{
		featuretype* feature = ThisData->GetFeatureP(i, NULL);
		Node* n = node->Tree->TestFeature(i, node->Level, feature_temp_store);
		if (n == node)
		{
			labeltype label;
			ThisData->GetLabel(i, &label);
			AddPointer(feature, label);
		}
	}
	delete feature_temp_store;
	FastClose();

	FastInit();
	for (int i = 0; i < N; i++)
		LabelCount[GetLabelNext()]++;
	FastClose();
	return 0;
}

int LinkerPointer::Release()
{
	if (FeaturePointers != NULL)
		delete FeaturePointers;
	if (Labels != NULL)
		delete Labels;
	if (SplitFlags != NULL)
		delete SplitFlags;
	if (LabelCount != NULL)
		delete LabelCount;
	delete this;
	return 0;
}