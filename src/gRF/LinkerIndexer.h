
#pragma once
#include "stdafx.h"
#include "Linker.h"
class RandomTree;
class Data;

class LinkerIndexer : public Linker
{
public:
	
	int* Indexes;
	char* SplitFlags;

	int GetFeatureindex;
	int GetFeatureLabelindex;
	int GetLabelindex;
	int SetSplitFlagindex;
	int GetSplitFlagindex;

	int GetFeaturePEI;
	int GetLabelPEI;
	int GetFeatureLabelEI;
	int GetLabelEI;

	int IndexesMemoryAlloCount;

	LinkerIndexer();
	virtual int InitFromData(Data* data);
	virtual int AddIndex(int index);
	virtual int FastInit();
	virtual int FastClose();
	int GetIndexFast();
	virtual valuetype GetValueNext();
	virtual void GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out);
	virtual void SetSplitFlagNext(char flag);
	virtual char GetSplitFlagNext();
	virtual int NewSplitFlags();
	virtual Linker** Split();
	virtual int Load(Node* node);

	virtual int Release();
};