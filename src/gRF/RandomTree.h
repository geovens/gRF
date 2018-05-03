
#pragma once
#include "Type.h"
#include "stdafx.h"
class Node;
class Function;
class Data;
//#include "windows.h"

class RandomTree
{
public:
	int ID;
	// max allowed depth of tree
	int MaxDepth;
	// real depth of tree
	int Depth;
	// how many times the process that params abc is random selected and entropy is calculated is done on each node.
	int CandidatesEachNode;
	// number of threads used for training
	int MaxThreadNumber;
	// a pointer to the root node
	Node* Root;
	// decision function used to split samples on each node
	Function* Function;
	// the training/testing data
	// there is only one occurence of Data attached on the tree
	Data* ThisData;
	// storage mode
	int PointersMode;

	int ThreadCount;
	std::mutex ThreadCount_mutex;
	//int IsWritingNode;
	FILE* FNode;


	__declspec(dllexport) RandomTree();
	__declspec(dllexport) ~RandomTree();

	__declspec(dllexport) int Plant();
	__declspec(dllexport) int Grow(Node* node);
	__declspec(dllexport) int PlantGrowFull(int depth);
	__declspec(dllexport) int ReadNodeFile();
	__declspec(dllexport) int Train(Data* data, int linkermode);
	__declspec(dllexport) int Test(Data* data);
	__declspec(dllexport) int Test(Data* data, int level);
	__declspec(dllexport) Node* TestFeature(featuretype* feature);
	__declspec(dllexport) Node* TestFeature(featuretype* feature, int level);
	__declspec(dllexport) Node* TestFeature(int index, int* ei, featuretype* feature_temp_store);
	__declspec(dllexport) Node* TestFeature(int index, int level, int* ei, featuretype* feature_temp_store);
	__declspec(dllexport) bool TestFeatureReach(int index, Node* node, int* ei, featuretype* feature);
	__declspec(dllexport) Node* FindNode(int level, unsigned long long index);
	static int NewThread(void* lpParameter);

	int LoadDataPointers(Node* node);

protected:
	int RecursionNewChildNodes(Node* node);
	int RecursionSplitTrainingSet(Node* node);
	int ReadTrainingProcess();
	int WriteNode(Node* node);
};