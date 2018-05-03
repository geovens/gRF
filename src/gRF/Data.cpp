
#include "Data.h"
#include "stdafx.h"



//int Data::Split(Data** child)
//{
//	ChildrenN[0] = 0;
//	ChildrenN[1] = 0;
//	for (int i = 0; i < N; i++)
//		ChildrenN[SplitFlags[i]]++;
//
//	for (int i = 0; i < 2; i++)
//	{
//		child[i]->D = D;
//		child[i]->K = K;
//		child[i]->N = ChildrenN[i];
//		//cv::Mat mat = cv::Mat::zeros(child[i]->N, D, CV_64FC1);
//		child[i]->Features = new double[child[i]->N * child[i]->D];
//		child[i]->Labels = new int[child[i]->N];
//		child[i]->LabelCount = new int[K];
//		memset(child[i]->LabelCount, 0, 4 * K);
//	}
//
//	int tick[2] = { 0, 0 };
//	double* childp[2];
//	childp[0] = child[0]->Features;
//	childp[1] = child[1]->Features;
//	double* p = Features;
//	for (int i = 0; i < N; i++)
//	{
//		int flag = SplitFlags[i];
//		memcpy(childp[flag], p, D * 8);
//		child[flag]->Labels[tick[flag]] = Labels[i];
//		child[flag]->LabelCount[Labels[i]]++;
//		tick[flag]++;
//		childp[flag] += D;
//		p += D;
//	}
//
//	return 0;
//}

Data::Data()
{
	//LabelPercentage = NULL;
	ReachedNodes = NULL;
}

int Data::CalLabelPercentage()
{
	if (LabelPercentage != NULL)
		delete LabelPercentage;
	LabelPercentage = new double[K];
	for (int k = 0; k < K; k++)
		LabelPercentage[k] = 0;
	int ei = 0;
	for (int i = 0; i < N; i++)
	{
		labeltype label;
		GetLabel(i, &label, &ei);
		LabelPercentage[label]++;
	}
	for (int k = 0; k < K; k++)
		LabelPercentage[k] /= N;
	return 0;
}

featuretype* Data::GetFeatureP(int index, featuretype* abc, int* ei)
{
	return Features + index * D;
}

void Data::GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei)
{
	//*feature_out = Features + index * D;
	memcpy(feature_out, Features + index * D, sizeof(featuretype)* D);
}

labeltype* Data::GetLabelP(int index, int* ei)
{
	return Labels + index;
}

void Data::GetFeatureLabel(int index, featuretype* abc, featuretype* feature_out, labeltype* label_out, int* ei)
{
	GetFeature(index, abc, feature_out, ei);
	GetLabel(index, label_out, ei);
}

void Data::GetLabel(int index, labeltype* label_out, int* ei)
{
	*label_out = *(Labels + index);
}

int Data::SetReachedNode(int index, Node* node, int* ei)
{
	if (ReachedNodes == NULL)
		ReachedNodes = new Node*[N];
	ReachedNodes[index] = node;
	return 0;
}

Node* Data::GetReachedNode(int index, int* ei)
{
	if (ReachedNodes == NULL)
		return NULL;
	return ReachedNodes[index];
}

int Data::Release()
{
	delete Features;
	delete Labels;
	//delete SplitFlags;
	//delete LabelCount;
	delete this;
	return 0;
}