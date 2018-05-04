
#include "Node.h"
#include "RandomTree.h"
#include "Data.h"
#include "Linker.h"
#include "Function.h"
#include "stdafx.h"
//#include "flowgraph/flowgraph.h"

Node::Node()
{
	ABC = NULL;
	FuncRandomABC = NULL;
	HistLabels = NULL;
	SampleReachedCount = NULL;
	Left = NULL;
	Right = NULL;
	Trained = 0;
}

int Node::Split(featuretype* abc, featuretype* feature_temp_store, bool setsplitflags)
{
	memset(ThisDataPointers->ChildrenLabelCount[0], 0, ThisData->K * sizeof(int));
	memset(ThisDataPointers->ChildrenLabelCount[1], 0, ThisData->K * sizeof(int));
	ThisDataPointers->ChildrenN[0] = 0;
	ThisDataPointers->ChildrenN[1] = 0;
	
	//featuretype* feature = new featuretype[ThisData->D];
	labeltype label;
	if (setsplitflags)
	{
		ThisDataPointers->NewSplitFlags();
		ThisDataPointers->FastInit();
		for (int i = 0; i < ThisDataPointers->N; i++)
		{
			ThisDataPointers->GetFeatureLabelNext(abc, feature_temp_store, &label);
			int flag = Tree->Function->TestFeature(feature_temp_store, abc);
			ThisDataPointers->ChildrenN[flag]++;
			ThisDataPointers->ChildrenLabelCount[flag][label]++;
			ThisDataPointers->SetSplitFlagNext(flag);
		}
		ThisDataPointers->FastClose();
	}
	else
	{
		ThisDataPointers->FastInit();
		for (int i = 0; i < ThisDataPointers->N; i++)
		{
			ThisDataPointers->GetFeatureLabelNext(abc, feature_temp_store, &label);
			int flag = Tree->Function->TestFeature(feature_temp_store, abc);
			ThisDataPointers->ChildrenN[flag]++;
			ThisDataPointers->ChildrenLabelCount[flag][label]++;
		}
	}

	if (ThisDataPointers->ChildrenN[0] == 0 || ThisDataPointers->ChildrenN[1] == 0)
		return -1;
	else
		return 0;
}

int Node::SplitManyTimes(int times)
{
	ABC = new featuretype[Tree->Function->ABCNum];
	ThisDataPointers->ChildrenLabelCount = new int*[2];
	ThisDataPointers->ChildrenLabelCount[0] = new int[ThisData->K];
	ThisDataPointers->ChildrenLabelCount[1] = new int[ThisData->K];
	double maxdecrease = 0;
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
		
		double decrease;
		if (rec == 0)
		{
			CalEntropy(eout);
			//CalEntropy2(eout);
			decrease = eout[1] - eout[0];
			if (decrease < maxdecrease)
			{
				memcpy(ABC, abc, Tree->Function->ABCNum * sizeof(featuretype));
				maxdecrease = decrease;
			}
		}
		//if (decrease < 0 && decrease < -0.6 + sqrt(Level) / 10.0)
		//	break;
	}

	ThisDataPointers->FastClose();
	delete feature_temp_store;
	delete abc;
	delete eout;

	if (maxdecrease == 0)
	{
		delete ThisDataPointers->ChildrenLabelCount[0];
		delete ThisDataPointers->ChildrenLabelCount[1];
		return -1;
	}
	else
	{
		//ThisDataPointers->NewSplitFlags();
		featuretype* feature_temp_store = new featuretype[ThisData->D];
		int rec = Split(ABC, feature_temp_store, true);
		if (rec < 0)
			printf("WTF2WTF2");
		delete feature_temp_store;
		delete ThisDataPointers->ChildrenLabelCount[0];
		delete ThisDataPointers->ChildrenLabelCount[1];
		return 0;
	}
}

int Node::CalEntropy(double* eout)
{
	// entrapy before split
	double entrapy1 = 0;
	for (int k = 0; k < ThisData->K; k++)
	{
		double percent = (double)ThisDataPointers->LabelCount[k] / ThisDataPointers->N;
		if (percent != 0)
			entrapy1 += -percent * log(percent);
	}

	//static int ed0 = -1, ed1 = -1;
	//if (Index == 0 && Level > ed0)
	//{
	//	printf("de of %d-0: %lf\n", Level, entrapy1 * ThisDataPointers->N);
	//	ed0 = Level;
	//}
	//if (Index == 1 && Level > ed1)
	//{
	//	printf("de of %d-1: %lf\n", Level, entrapy1 * ThisDataPointers->N);
	//	ed1 = Level;
	//}

	// entrapy sum of each part
	double newentrapy[2];
	double entrapy2 = 0;
	for (int p = 0; p < 2; p++)
	{
		double entrapyp = 0;
		for (int k = 0; k < ThisData->K; k++)
		{
			double percent = (double)ThisDataPointers->ChildrenLabelCount[p][k] / ThisDataPointers->ChildrenN[p];
			if (percent != 0)
				entrapyp += -percent * log(percent);
		}
		newentrapy[p] = entrapyp;
		entrapy2 += entrapyp * ThisDataPointers->ChildrenN[p] / ThisDataPointers->N;
	}

	eout[0] = entrapy1;
	eout[1] = entrapy2;
	eout[2] = newentrapy[0];
	eout[3] = newentrapy[1];
	return 0;
}

// using a new measurement proposed on 2016.07.25 to replace Shannon Entropy
int Node::CalEntropy2(double* eout)
{
	// entrapy before split
	double entrapy1 = 0;
	double entrapy2 = 0;

	int side[2];
	int sidec[2][2];
	side[0] = side[1] = 0;
	sidec[0][0] = sidec[0][1] = sidec[1][0] = sidec[1][1] = 0;
	for (int k = 0; k < ThisData->K; k++)
	{
		if (ThisDataPointers->ChildrenLabelCount[0][k] >= ThisDataPointers->ChildrenLabelCount[1][k])
		{
			side[0] += ThisDataPointers->LabelCount[k];
			sidec[0][0] += ThisDataPointers->ChildrenLabelCount[0][k];
			sidec[0][1] += ThisDataPointers->ChildrenLabelCount[1][k];
		}
		else
		{
			side[1] += ThisDataPointers->LabelCount[k];
			sidec[1][0] += ThisDataPointers->ChildrenLabelCount[0][k];
			sidec[1][1] += ThisDataPointers->ChildrenLabelCount[1][k];
		}
	}
		
	entrapy1 = side[0] < side[1] ? side[0] : side[1];
	entrapy2 = (sidec[0][0] < sidec[1][0] ? sidec[0][0] : sidec[1][0]) + (sidec[0][1] < sidec[1][1] ? sidec[0][1] : sidec[1][1]);

	eout[0] = entrapy1;
	eout[1] = entrapy2;
	return 0;
}

int Node::Vote()
{
	if (ThisDataPointers->N == 0)
	{
		printf("WARNING: N == 0 in Vote()\n");
		HistLabels = new double[ThisData->K];
		for (int k = 0; k < ThisData->K; k++)
		{
			HistLabels[k] = 0;
		}
		MajorLabel = -1;
		return -1;
	}

	int* labels = new int[ThisData->K];
	memset(labels, 0, ThisData->K * sizeof(int));
	ThisDataPointers->FastInit();
	for (int i = 0; i < ThisDataPointers->N; i++)
	{
		labels[ThisDataPointers->GetLabelNext()]++;
	}
	ThisDataPointers->FastClose();

	HistLabels = new double[ThisData->K];
	int maxvote = -1;
	int maxk = -1;
	int secondmaxvote = -1;
	int secondmaxk = -1;
	double sumhist = 0;
	for (int k = 0; k < ThisData->K; k++)
	{
		HistLabels[k] = (double)labels[k] / ThisDataPointers->N / ThisData->LabelPercentage[k];

		// some test adjustment
		{
			// makes nodes that got LESS pixels in training weight MORE, 
			// so that their decisions are equally important as nodes that have more voters(more reached samples).
			//HistLabels[k] /= sqrt((double)ThisDataPointers->N);  
		}

		sumhist += HistLabels[k];
		if (labels[k] > maxvote)
		{
			secondmaxvote = maxvote;
			secondmaxk = maxk;
			maxvote = labels[k];
			maxk = k;
		}
		else if (labels[k] > secondmaxvote)
		{
			secondmaxvote = labels[k];
			secondmaxk = k;
		}
	}
	for (int k = 0; k < ThisData->K; k++)
		HistLabels[k] /= sumhist;

	MajorLabel = maxk;
	SecondMajorLabel = secondmaxk;
	SampleReachedCount = new int[ThisData->K];
	memset(SampleReachedCount, 0, sizeof(int)* ThisData->K);
	for (int k = 0; k < ThisData->K; k++)
		SampleReachedCount[k] = labels[k] / ThisData->LabelPercentage[k] / ThisData->K;

	delete labels;
	return maxk;
}
