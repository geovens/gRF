
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
	valuetype* Values;
	// used by training process to mark which child node (left or right) each sample is supposed/upon forwarding to
	char* SplitFlags;

	featuretype** GetFeaturep;
	valuetype* GetLabelp;
	char* SetSplitFlagp;
	char* GetSplitFlagp;

	int GetFeaturePEI;
	int GetLabelPEI;
	int GetFeatureLabelEI;
	int GetLabelEI;

	int PointerMemoryAlloCount;

	LinkerPointer();
	virtual int InitFromData(Data* data);
	virtual int AddPointer(featuretype* feature, valuetype label);
	virtual int FastInit();
	virtual int FastClose();
	//virtual featuretype* GetFeatureNext(featuretype* abc);
	virtual valuetype GetValueNext();
	virtual void GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out);
	virtual void SetSplitFlagNext(char flag);
	virtual char GetSplitFlagNext();
	virtual int NewSplitFlags();
	//virtual Linker** NewChildren();
	virtual Linker** Split();
	virtual int Load(Node* node);

	virtual int Release();


};