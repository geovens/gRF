#pragma once
#include "Type.h"
class RandomTree;
class Data;
class Node;

class Linker
{
public:
	int N;
	Data* ThisData;
	Node* ThisNode;
	void* Info;

	// number of samples in each class
	int* LabelCount;

	// used by training process to record numbers when testing different abcs in orther to calculate entropy
	int ChildrenN[2];
	int** ChildrenLabelCount;

	virtual int InitFromData(Data* data) = 0;
	virtual int FastInit() = 0;
	virtual int FastClose() = 0;
	//virtual featuretype* GetFeatureNext(featuretype* abc);
	virtual labeltype GetLabelNext() = 0;
	virtual void GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out) = 0;
	virtual void SetSplitFlagNext(char flag) = 0;
	virtual int NewSplitFlags() = 0;
	//virtual Linker** NewChildren() = 0;
	virtual Linker** Split() = 0;
	virtual int Load(Node* node) = 0;

	virtual int Release() = 0;
};