
#include "Node.h"
#include "RandomTree.h"
#include "Data.h"
#include "Linker.h"
#include "Function.h"
#include "stdafx.h"
//#include "flowgraph/flowgraph.h"

Node::Node()
{
	N = 0;
	ABC = NULL;
	FuncRandomABC = NULL;
	Left = NULL;
	Right = NULL;
	Trained = 0;
}

int Node::Split(featuretype* abc, featuretype* feature_temp_store, bool setsplitflags)
{
	ThisDataPointers->ChildrenN[0] = 0;
	ThisDataPointers->ChildrenN[1] = 0;
	
	//featuretype* feature = new featuretype[ThisData->D];
	valuetype label;
	if (true)
	{
		ThisDataPointers->NewSplitFlags();
		ThisDataPointers->FastInit();
		for (int i = 0; i < ThisDataPointers->N; i++)
		{
			ThisDataPointers->GetFeatureValueNext(abc, feature_temp_store, &label);
			int flag = Tree->Function->TestFeature(feature_temp_store, abc);
			ThisDataPointers->ChildrenN[flag]++;
			ThisDataPointers->SetSplitFlagNext(flag);
		}
		ThisDataPointers->FastClose();
	}
	else
	{
		ThisDataPointers->FastInit();
		for (int i = 0; i < ThisDataPointers->N; i++)
		{
			ThisDataPointers->GetFeatureValueNext(abc, feature_temp_store, &label);
			int flag = Tree->Function->TestFeature(feature_temp_store, abc);
			ThisDataPointers->ChildrenN[flag]++;
		}
		ThisDataPointers->FastClose();
	}

	if (ThisDataPointers->ChildrenN[0] == 0 || ThisDataPointers->ChildrenN[1] == 0)
		return -1;
	else
		return 0;
}

int Node::SplitManyTimes(int times)
{
	ABC = new featuretype[Tree->Function->ABCNum];
	for (int d = 0; d < Tree->Function->ABCNum; d++)
		ABC[d] = 0;
	double mindiff = 10000000.0;
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	featuretype* abc = new featuretype[Tree->Function->ABCNum];
	double* eout = new double[4];
	//srand(101 + Tree->ID);

	for (int t = 0; t < times; t++)
	{
		//Tree->Function->Data = ThisDataPointers;
		if (FuncRandomABC == NULL)
			Tree->Function->RandomABC(this, abc);
		else
			FuncRandomABC(this, ThisData, abc);
		int rec = Split(abc, feature_temp_store);

		if (rec == 0)
		{
			CalEntropy(eout);
			if (eout[0] < mindiff)
			{
				memcpy(ABC, abc, Tree->Function->ABCNum * sizeof(featuretype));
				mindiff = eout[0];
			}
		}
	}

	//ThisDataPointers->FastClose();
	delete feature_temp_store;
	delete abc;
	delete eout;

	if (mindiff == 0)
	{
		return -1;
	}
	else
	{
		//ThisDataPointers->NewSplitFlags();
		featuretype* feature_temp_store = new featuretype[ThisData->D];
		Split(ABC, feature_temp_store, true);
		delete feature_temp_store;
		return 0;
	}
}

int Node::CalEntropy(double* eout)
{
	valuetype sum[2] = { 0, 0 };
	int n[2] = { 0, 0 };

	ThisDataPointers->FastInit();
	for (int i = 0; i < ThisDataPointers->N; i++)
	{
		float v = ThisDataPointers->GetValueNext();
		int flag = ThisDataPointers->GetSplitFlagNext();
		sum[flag] += v;
		n[flag]++;
	}
	ThisDataPointers->FastClose();

	float average[2];
	if (n[0] > 0)
		average[0] = sum[0] / n[0];
	if (n[1] > 0)
		average[1] = sum[1] / n[1];
	float sum2[2] = { 0, 0 };

	ThisDataPointers->FastInit();
	for (int i = 0; i < ThisDataPointers->N; i++)
	{
		float v = ThisDataPointers->GetValueNext();
		int flag = ThisDataPointers->GetSplitFlagNext();
		sum2[flag] += pow(v - average[flag], 2);
	}
	ThisDataPointers->FastClose();

	eout[0] = (float)n[0] / (n[0] + n[1]) * sum2[0] + (float)n[1] / (n[0] + n[1]) * sum2[1];
	return 0;
}

int Node::Vote()
{
	valuetype sum = 0;
	int n = 0;

	ThisDataPointers->FastInit();
	for (int i = 0; i < ThisDataPointers->N; i++)
	{
		float v = ThisDataPointers->GetValueNext();
		sum += v;
		n++;
	}
	ThisDataPointers->FastClose();

	if (n > 0)
		AverageValue = sum / n;

	return 0;
}