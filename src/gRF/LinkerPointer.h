
#pragma once
#include "Type.h"
#include "Linker.h"
class RandomTree;
class Data;
class Node;

class LinkerPointer : public Linker
{
public:
	

	featuretype** FeaturePointers;
	labeltype* Labels;
	// used by training process to mark which child node (left or right) each sample is supposed/upon forwarding to
	char* SplitFlags;

	featuretype** GetFeaturep;
	labeltype* GetLabelp;
	char* SetSplitFlagp;

	int PointerMemoryAlloCount;

	LinkerPointer();
	virtual int InitFromData(Data* data);
	virtual int AddPointer(featuretype* feature, labeltype label);
	virtual int FastInit();
	virtual int FastClose();
	//virtual featuretype* GetFeatureNext(featuretype* abc);
	virtual labeltype GetLabelNext();
	virtual void GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out);
	virtual void SetSplitFlagNext(char flag);
	virtual int NewSplitFlags();
	//virtual Linker** NewChildren();
	virtual Linker** Split();
	virtual int Load(Node* node);

	virtual int Release();


};