
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

	int GetFeaturePEI;
	int GetLabelPEI;
	int GetFeatureLabelEI;
	int GetLabelEI;

	LinkerIndexerFile();
	virtual int InitFromData(Data* data);
	char* Read4();
	virtual int FastInit();
	virtual int FastClose();
	int GetIndexFast();
	//virtual featuretype* GetFeaturePNext(featuretype* abc);
	virtual valuetype GetValueNext();
	virtual void GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out);
	virtual void SetSplitFlagNext(char flag);
	virtual char GetSplitFlagNext();
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