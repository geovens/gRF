
#include "LinkerIndexerFile.h"
#include "Data.h"
#include "Node.h"
#include "RandomTree.h"
#include "stdafx.h"

LinkerIndexerFile::LinkerIndexerFile()
{
	Indexes = NULL;
	ReadBuffPend = ReadBuff + 508;
	GetFeatureLabelEI = 0;
}

int LinkerIndexerFile::InitFromData(Data* data)
{
	ThisData = data;
	N = data->N;

	// temp!
	// the root node should not store a indexes file!
	strcpy(IndexesFileName, ".\\internal\\node-0-0-");
	strcpy(SplitFlagsFileName, ".\\internal\\node-0-0-");
	strcat(IndexesFileName, "indexes.bin");
	strcat(SplitFlagsFileName, "splitflags.bin");
	Indexes = fopen(IndexesFileName, "wb+");
	fwrite(&N, 1, 4, Indexes);
	//fclose(Indexes);
	//FastInit();
	GetLabelEI = 0;
	for (int i = 0; i < data->N; i++)
	{
		fwrite(&i, 4, 1, Indexes);
	}
	//FastClose();
	fclose(Indexes);
	Indexes = NULL;

	return 0;
}

char* LinkerIndexerFile::Read4()
{
	if (ReadBuffP <= ReadBuffPend)
	{
		char* out = ReadBuffP;
		ReadBuffP += 4;
		return out;
	}
	else
	{
		int r = fread(ReadBuff, 1, 8192, Indexes);
		ReadBuffPend = ReadBuff + r - 4;
		ReadBuffP = ReadBuff + 4;
		return ReadBuff;
	}
}

int LinkerIndexerFile::FastInit()
{
	if (Indexes != NULL)
	{
		fseek(Indexes, 4, SEEK_SET);
	}
	else
	{
		//while (Indexes == NULL)
		Indexes = fopen(IndexesFileName, "rb+");
		fseek(Indexes, 4, SEEK_SET);
	}
	if (SplitFlagsFile != NULL)
		fseek(SplitFlagsFile, 0, SEEK_SET);
	else
	{
		SplitFlagsFile = fopen(SplitFlagsFileName, "rb+");
	}
	ReadBuffP = ReadBuff + 65536;
	//ReadBuffPend = ReadBuff - 1;
	GetFeaturePEI = 0;
	GetLabelPEI = 0;
	GetFeatureLabelEI = 0;
	GetLabelEI = 0;
	return 0;
}

int LinkerIndexerFile::FastClose()
{
	if (Indexes != NULL)
		fclose(Indexes);
	if (SplitFlagsFile != NULL)
		fclose(SplitFlagsFile);
	Indexes = NULL;
	SplitFlagsFile = NULL;
	return 0;
}

int LinkerIndexerFile::GetIndexFast()
{
	/*int index;
	fread(&index, 4, 1, Indexes);
	return index;*/
	return *(int*)Read4();
}
//featuretype* DataPointersFile::GetFeaturePNext(featuretype* abc)
//{
//	int index;
//	int r = fread(&index, 1, 4, Indexes);
//	if (r != 4)
//		printf("ERROR: fread failed in DataPointersFile::GetFeaturePNext()\n");
//	return ThisData->GetFeatureP(index, abc, &GetFeaturePEI);
//}
valuetype LinkerIndexerFile::GetValueNext()
{
	int index;
	int r = fread(&index, 1, 4, Indexes);
	if (r != 4)
		printf("ERROR: fread failed in DataPointersFile::GetLabelNext()\n");
	valuetype v;
	ThisData->GetValue(index, &v, &GetLabelEI);
	return v;
}
void LinkerIndexerFile::GetFeatureValueNext(featuretype* abc, featuretype* feature_out, valuetype* value_out)
{
	int index = *(int*)Read4();
	ThisData->GetFeatureValue(index, abc, feature_out, value_out, &GetFeatureLabelEI);
}
void LinkerIndexerFile::SetSplitFlagNext(char flag)
{
	fwrite(&flag, 1, 1, SplitFlagsFile);
	//fputc((char)flag, SplitFlagsFile);
}
char LinkerIndexerFile::GetSplitFlagNext()
{
	int flag = 0;
	int r = fread(&flag, 1, 1, SplitFlagsFile);
	if (r != 1)
		printf("ERROR: fread failed in DataPointersFile::GetSplitFlagNext()\n");
	/*int flag;
	flag = fgetc(SplitFlagsFile);*/
	return flag;
}

//int DataPointersFile::NewWriteIndex()
//{
//	if (Indexes != NULL)
//		fclose(Indexes);
//	if (SplitFlagsFile != NULL)
//		fclose(SplitFlagsFile);
//	Indexes = fopen(strstr(IndexesFileName, "indexes.bin"), "r+");
//	SplitFlagsFile = fopen(strstr(IndexesFileName, "splitflags.bin"), "r+");
//}
//
//int DataPointersFile::NewReadIndex()
//{
//	if (Indexes != NULL)
//		fclose(Indexes);
//	if (SplitFlagsFile != NULL)
//		fclose(SplitFlagsFile);
//	Indexes = fopen(strstr(IndexesFileName, "indexes.bin"), "r");
//	SplitFlagsFile = fopen(strstr(IndexesFileName, "splitflags.bin"), "r");
//}

//int DataPointersFile::AddIndexFast(int index)
//{
//	fwrite(&index, 4, 1, Indexes);
//}

//int DataPointersFile::GetIndexFast()
//{
//	int index;
//	fread(&index, 4, 1, Indexes);
//	return index;
//}

//int DataPointers::NewSplitFlags()
//{
//	SplitFlags = new int[N];
//	memset(SplitFlags, 0, N * sizeof(int));
//	return 0;
//}

//int DataPointers::GetLabelCount(int k)
//{
//	return LabelCount[k];
//}

//DataPointers** DataPointers::NewChildren()
//{
//	DataPointers** child = new DataPointers*[2];
//	child[0] = new DataPointers();
//	child[1] = new DataPointers();
//	return child;
//}

//Linker** LinkerIndexerFile::NewChildren()
//{
//
//}
int LinkerIndexerFile::NewSplitFlags()
{
	SplitFlagsFile = fopen(SplitFlagsFileName, "wb+");
	return 0;
}
Linker** LinkerIndexerFile::Split()
{
	LinkerIndexerFile** child = new LinkerIndexerFile*[2];
	child[0] = new LinkerIndexerFile();
	child[1] = new LinkerIndexerFile();
	ChildrenN[0] = 0;
	ChildrenN[1] = 0;

	for (int i = 0; i < 2; i++)
	{
		child[i]->ThisData = ThisData;
		child[i]->Info = Info;
		char ifn[1024] = ".\\internal\\node-";
		char pa[30];
		sprintf(pa, "%d-%d-", ThisNode->Level + 1, ThisNode->Index * 2 + i);
		strcat(ifn, pa);
		strcpy(child[i]->IndexesFileName, ifn);
		strcpy(child[i]->SplitFlagsFileName, ifn);
		strcat(child[i]->IndexesFileName, "indexes.bin");
		strcat(child[i]->SplitFlagsFileName, "splitflags.bin");

		child[i]->Indexes = fopen(child[i]->IndexesFileName, "wb+");
	}

	FastInit();
	int zero = 0;
	fwrite(&zero, 1, 4, child[0]->Indexes);
	fwrite(&zero, 1, 4, child[1]->Indexes);
	for (int i = 0; i < N; i++)
	{
		int index = GetIndexFast();
		int flag = GetSplitFlagNext();
		ChildrenN[flag]++;
		fwrite(&index, 4, 1, child[flag]->Indexes);
	}
	FastClose();

	child[0]->N = ChildrenN[0];
	child[1]->N = ChildrenN[1];

	fseek(child[0]->Indexes, 0, SEEK_SET);
	fseek(child[1]->Indexes, 0, SEEK_SET);
	fwrite(&(child[0]->N), 1, 4, child[0]->Indexes);
	fwrite(&(child[1]->N), 1, 4, child[1]->Indexes);
	fclose(child[0]->Indexes);
	fclose(child[1]->Indexes);
	child[0]->Indexes = NULL;
	child[1]->Indexes = NULL;

	return (Linker**)child;
}

int LinkerIndexerFile::Load(Node* node)
{
	ThisNode = node;
	ThisData = node->ThisData;
	char ifn[1024] = ".\\internal\\node-";
	char pa[30];
	sprintf(pa, "%d-%d-", ThisNode->Level, ThisNode->Index);
	strcat(ifn, pa);
	strcpy(IndexesFileName, ifn);
	strcpy(SplitFlagsFileName, ifn);
	strcat(IndexesFileName, "indexes.bin");
	strcat(SplitFlagsFileName, "splitflags.bin");

	//int n;
	//FILE* findex = fopen(IndexesFileName, "r");
	//int r = fread(&n, 1, 4, findex);
	//if (r != 4)
	//{
	//	printf("WORNING: error reading indexes file when trying to resume. ignore the file and redo testing of all samples in training set.\n");
	//	printf("WORNING: this might be buggy!\n");
	//	fclose(findex);
	//	LoadByTest(node);
	//	//return DataPointers::Load(node);
	//}
	//fclose(findex);
	//N = n;
	//if (n == 0)
	//{
	//	printf("WORNING: indexes file is incomplete when trying to resume. ignore the file and redo testing of all samples in training set.\n");
	//	printf("WORNING: this might be buggy!\n");
	//	fclose(findex);
	//	LoadByTest(node);
	//	//return DataPointers::Load(node);
	//}

	LoadByTest(node);

	return 0;
}

int LinkerIndexerFile::LoadByTest(Node* node)
{
	FILE* findex = fopen(IndexesFileName, "wb");
	int zero = 0;
	fwrite(&zero, 1, 4, findex);

	N = 0;
	FastInit();
	int ei = 0;
	featuretype* feature_temp_store = new featuretype[ThisData->D];
	for (int i = 0; i < ThisData->N; i++)
	{
		//featuretype* feature = ThisData->GetFeatureP(i, NULL, &GetFeaturePEI);
		Node* n = node->Tree->TestFeature(i, node->Level, &ei, feature_temp_store);
		if (n == node)
		{
			N++;
			fwrite(&i, 1, 4, findex);
		}
	}
	delete feature_temp_store;
	FastClose();

	fseek(findex, 0, SEEK_SET);
	fwrite(&N, 1, 4, findex);
	fclose(findex);

	return 0;
}

int LinkerIndexerFile::Release()
{
	if (Indexes != NULL)
		fclose(Indexes);
	if (SplitFlagsFile != NULL)
		fclose(SplitFlagsFile);
	delete this;
	return 0;
}