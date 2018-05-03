#pragma once
#include "Node.h"
#include "stdafx.h"

class Function
{
public:
	// store the Data that is being splitting for RandomABC and TestFeature to use
	//Data* Data;
	// other user-defined information
	void* Info;
	// number of random params ABC
	int ABCNum;
	// generate a random params of splitting function
	virtual int RandomABC(Node* node, featuretype *abc) = 0;
	// test feature flag using random params abc
	virtual int TestFeature(featuretype* feature, featuretype* abc) = 0;


private:
	featuretype* abcrange = NULL;
};