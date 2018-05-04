
#pragma once
#include "stdafx.h"
#include "Linker.h"
class RandomTree;
class Data;

class LinkerIndexerFile : public Linker
{
public:
	char IndexesFileName[1024];
	char SplitFlagsFileName[1024];
	FILE* Indexes;
	FILE* SplitFlagsFile;

	char ReadBuff[8192];
	char* ReadBuffP;
	char* ReadBuffPend;

	LinkerIndexerFile();
	virtual int InitFromData(Data* data);
	char* Read4();
	virtual int FastInit();
	virtual int FastClose();
	int GetIndexFast();
	//virtual featuretype* GetFeaturePNext(featuretype* abc);
	virtual labeltype GetLabelNext();
	virtual void GetFeatureLabelNext(featuretype* abc, featuretype* feature_out, labeltype* label_out);
	virtual void SetSplitFlagNext(char flag);
	char GetSplitFlagNext();
	//virtual Linker** NewChildren();
	virtual int NewSplitFlags();
	virtual Linker** Split();
	virtual int Load(Node* node);
	int LoadByTest(Node* node);

	virtual int Release();
	//int NewWriteIndex();
	//int NewReadIndex();
	//int AddIndexFast(int index);
	//int GetIndexFast();
};