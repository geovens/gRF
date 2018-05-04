
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

	int IndexesMemoryAlloCount;

	LinkerIndexer();
	virtual int InitFromData(Data* data);
	virtual int AddIndex(int index);
	virtual int FastInit();
	virtual int FastClose();
	int GetIndexFast();
	virtual labeltype GetLabelNext();
	virtual void GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out);
	virtual void SetSplitFlagNext(char flag);
	virtual int NewSplitFlags();
	virtual Linker** Split();
	virtual int Load(Node* node);

	virtual int Release();
};