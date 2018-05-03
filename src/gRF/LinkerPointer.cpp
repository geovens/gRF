
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

	FastInit();
	for (int i = 0; i < data->N; i++)
	{
		featuretype* fp;
		valuetype lp;
		fp = data->GetFeatureP(i, NULL, &GetFeaturePEI);
		data->GetValue(i, &lp, &GetLabelPEI);
		AddPointer(fp, lp);
	}
	FastClose();

	return 0;
}

int LinkerPointer::AddPointer(featuretype* feature, valuetype label)
{
	if (N >= PointerMemoryAlloCount)
	{
		featuretype** newfpointers = new featuretype*[PointerMemoryAlloCount * 2 + 1];
		memcpy(newfpointers, FeaturePointers, N * sizeof(featuretype*));
		if (FeaturePointers != NULL)
			delete FeaturePointers;
		FeaturePointers = newfpointers;

		valuetype* newlpointers = new valuetype[PointerMemoryAlloCount * 2 + 1];
		memcpy(newlpointers, Values, N * sizeof(valuetype));
		if (Values != NULL)
			delete Values;
		Values = newlpointers;

		PointerMemoryAlloCount = PointerMemoryAlloCount * 2 + 1;
	}
	FeaturePointers[N] = feature;
	Values[N] = label;
	N++;
	return 0;
}

int LinkerPointer::FastInit()
{
	GetFeaturep = FeaturePointers;
	GetLabelp = Values;
	SetSplitFlagp = SplitFlags;
	GetSplitFlagp = SplitFlags;
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
valuetype LinkerPointer::GetValueNext()
{
	valuetype r = *GetLabelp;
	GetLabelp++;
	return r;
}
void LinkerPointer::GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out)
{
	//*feature_out = *GetFeaturep;
	memcpy(feature_out, *GetFeaturep, sizeof(featuretype)* ThisData->D);
	*value_out = *GetLabelp;
	GetFeaturep++;
	GetLabelp++;
}
void LinkerPointer::SetSplitFlagNext(char flag)
{
	*SetSplitFlagp = flag;
	SetSplitFlagp++;
}
char LinkerPointer::GetSplitFlagNext()
{
	char flag = *GetSplitFlagp;
	GetSplitFlagp++;
	return flag;
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
		child[i]->Values = new valuetype[child[i]->N];
	}

	featuretype** childfp[2];
	childfp[0] = child[0]->FeaturePointers;
	childfp[1] = child[1]->FeaturePointers;
	featuretype** fp = FeaturePointers;
	valuetype* childlp[2];
	childlp[0] = child[0]->Values;
	childlp[1] = child[1]->Values;
	valuetype* lp = Values;
	for (int i = 0; i < N; i++)
	{
		char flag = SplitFlags[i];
		*childfp[flag] = *fp;
		*childlp[flag] = *lp;
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

	FastInit();
	int ei = 0;
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < ThisData->N; i++)
	{
		featuretype* feature = ThisData->GetFeatureP(i, NULL, &GetFeaturePEI);
		Node* n = node->Tree->TestFeature(i, node->Level, &ei, feature_temp_store);
		if (n == node)
		{
			valuetype value;
			ThisData->GetValue(i, &value, &GetLabelPEI);
			AddPointer(feature, value);
		}
	}
	delete feature_temp_store;
	FastClose();

	return 0;
}

int LinkerPointer::Release()
{
	if (FeaturePointers != NULL)
		delete FeaturePointers;
	if (Values != NULL)
		delete Values;
	if (SplitFlags != NULL)
		delete SplitFlags;
	delete this;
	return 0;
}