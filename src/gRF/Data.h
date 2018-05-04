
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

private:
	// predicted label of each sample. 
	// (2016.08: seems like only available after testing with a forest instead of a single tree)
	labeltype* Predictions;
	// 2nd predicted label of each sample. 
	// (2016.08: seems like only available after testing with a forest instead of a single tree)
	labeltype* Predictions2;
	// 3rd predicted label of each sample. 
	// (2016.08: seems like only available after testing with a forest instead of a single tree)
	labeltype* Predictions3;
	// the leaf node that each sample reaches. 
	// (2016.08: if tested with a single tree instead of a forest, this records only the leaf node of the last tree in the forest)
	// format: ReachedNodes[i] is a pointer to the leaf node that the i'th sample reaches
	Node** ReachedNodes;

public:
	// predicted label of each sample. 
	// (added in 2016.08. only available after testing with a forest instead of a single tree)
	// format: PredictionHists[i] is an array recording the cumulated histogram by all trees for the i'th sample
	double** PredictionHists;
	// percentage of labels
	double* LabelPercentage;


	// split samples into two groups according to SplitFlags
	// output to *child[0] and *child[1]
	//__declspec(dllexport) virtual int Split(Data** child);

	// generate two new data. should be implemented in child class
	//__declspec(dllexport) virtual Data** NewChildren() = 0;

	__declspec(dllexport) Data();

	__declspec(dllexport) virtual int CalLabelPercentage();
	__declspec(dllexport) virtual featuretype* GetFeatureP(int index, featuretype* abc);
	__declspec(dllexport) virtual void GetFeature(int index, featuretype* abc, featuretype* feature_out);
	__declspec(dllexport) virtual labeltype* GetLabelP(int index);
	__declspec(dllexport) virtual void GetFeatureLabel(int index, featuretype* abc, featuretype* feature_out, labeltype* label_out);
	__declspec(dllexport) virtual void GetLabel(int index, labeltype* label_out);

	__declspec(dllexport) virtual Data* GetSample(int index, int* local_index_out);
	__declspec(dllexport) virtual int SetPrediction(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction(int index);
	__declspec(dllexport) virtual int SetPrediction2(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction2(int index);
	__declspec(dllexport) virtual int SetPrediction3(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction3(int index);
	__declspec(dllexport) virtual int SetReachedNode(int index, Node* prediction);
	__declspec(dllexport) virtual Node* GetReachedNode(int index);

	__declspec(dllexport) virtual int Release();
};