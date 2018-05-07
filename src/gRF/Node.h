
#pragma once
#include "Type.h"
class Data;
class Linker;
class RandomTree;

class Node
{
public:
	// pointer to the tree this is belonging to
	RandomTree* Tree;
	unsigned long long Index;
	// root node is level 0. leaf nodes are level depth.
	int Level;
	// randomly generated params for splitting
	featuretype* ABC;
	// whether this is a leaf node
	bool IsLeaf;
	//
	int* SampleReachedCount;
	// histogram of labels
	double* HistLabels;
	// the label of the mojority population.
	int MajorLabel;
	int SecondMajorLabel;
	// whether this node has been trained
	int Trained;
	// type of this node
	int Type;

	// data of the entire tree;
	Data* ThisData;
	// pointers to data that reached this node
	Linker* ThisDataPointers;

	Node* Parent;
	Node* Left;
	Node* Right;

	int* (*FuncRandomABC)(Node* node, Data* data, featuretype* abc);

	Node();

	int Split(featuretype* abc, featuretype* feature_temp_store, bool setsplitflags = false);
	int SplitManyTimes(int times);
	int CalEntropy(double* eout);
	int CalEntropy2(double* eout);

	// split samples into two groups according to ABC test
	// output to *child[0] and *child[1]
	int SplitPointers(Linker** child);

	int Vote();

	void Release();
};