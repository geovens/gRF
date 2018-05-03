
#pragma once
#include "gRF.h"

class Function_Sample1 : public Function
{
public:
	Function_Sample1();

	// generate random values for feature extracting parameters
	// node: the pointer to the node that is requiring the generation. You can ignore it unless you want different nodes to do different things
	// abc (output): the randomly generated feature extracting parameters should be stored here
	virtual int RandomABC(Node* node, featuretype* abc);

	// define the splitting function.
	// the return value should be either 0 or 1, indicating left child node or right child node respectively.
	virtual int TestFeature(featuretype* feature, featuretype* abc);
};