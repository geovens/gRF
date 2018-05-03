
#pragma once
#include "Type.h"
class Node;

class Data
{
public:
	// number of samples
	int N;
	// dimension of feature
	int D;
	// number of classes
	int K;
	// settings of how data is storaged during training
	int StorageMode;

	// features
	featuretype* Features;
	// label of each sample
	labeltype* Labels;
	// predicted label of each sample. 
	// (2016.08: seems like only available after testing with a forest instead of a single tree)
	// format: Predictions[0] is an array that records the most likely labels of each sample
	// Predictions[1] is an array that records the second most likely labels of each sample. i.e. the label with the second largest value in the average histogram by all trees
	// Predictions[2] is an array that records the third most likely labels of each sample
	labeltype** Predictions;
	// predicted label of each sample. 
	// (added in 2016.08. only available after testing with a forest instead of a single tree)
	// format: PredictionHists[i] is an array recording the cumulated histogram by all trees for the i'th sample
	double** PredictionHists;
	// the leaf node that each sample reaches. 
	// (2016.08: if tested with a single tree instead of a forest, this records only the leaf node of the last tree in the forest)
	// format: ReachedNodes[i] is a pointer to the leaf node that the i'th sample reaches
	Node** ReachedNodes;
	// percentage of labels
	double* LabelPercentage;


	// split samples into two groups according to SplitFlags
	// output to *child[0] and *child[1]
	//__declspec(dllexport) virtual int Split(Data** child);

	// generate two new data. should be implemented in child class
	//__declspec(dllexport) virtual Data** NewChildren() = 0;

	__declspec(dllexport) Data();
	__declspec(dllexport) virtual int CalLabelPercentage();
	__declspec(dllexport) virtual featuretype* GetFeatureP(int index, featuretype* abc, int* ei);
	__declspec(dllexport) virtual void GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei);
	__declspec(dllexport) virtual labeltype* GetLabelP(int index, int* ei);
	__declspec(dllexport) virtual void GetFeatureLabel(int index, featuretype* abc, featuretype* feature_out, labeltype* label_out, int* ei);
	__declspec(dllexport) virtual void GetLabel(int index, labeltype* label_out, int* ei);
	__declspec(dllexport) virtual int SetReachedNode(int index, Node* prediction, int* ei);
	__declspec(dllexport) virtual Node* GetReachedNode(int index, int* ei);

	__declspec(dllexport) virtual int Release();
};