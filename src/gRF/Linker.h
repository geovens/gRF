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

	// used by training process to record values when testing different abcs in orther to calculate average value of children
	int ChildrenN[2];

	virtual int InitFromData(Data* data) = 0;
	virtual int FastInit() = 0;
	virtual int FastClose() = 0;
	//virtual featuretype* GetFeatureNext(featuretype* abc);
	virtual valuetype GetValueNext() = 0;
	virtual void GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out) = 0;
	virtual void SetSplitFlagNext(char flag) = 0;
	virtual char GetSplitFlagNext() = 0;
	virtual int NewSplitFlags() = 0;
	//virtual Linker** NewChildren() = 0;
	virtual Linker** Split() = 0;
	virtual int Load(Node* node) = 0;

	virtual int Release() = 0;
};