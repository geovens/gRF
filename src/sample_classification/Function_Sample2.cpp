
#include "Function_Sample2.h"
#include "stdafx.h"

Function_Sample2::Function_Sample2()
{
	ABCNum = 4; // a, b, c, and threshold
}

int Function_Sample2::TestFeature(featuretype* feature, featuretype* abc)
{
	return feature[0] <= abc[3] ? 0 : 1; // using the extracted feature value and a threshold as the splitting function on every none-leaf node. 
	                                     // the feature value is extracted in the Data class, in this sample program in the Data_Sample2 class
}

int Function_Sample2::RandomABC(Node* node, featuretype* abc)
{
	
	abc[0] = (featuretype)rand() / RAND_MAX; // a
	abc[1] = (featuretype)rand() / RAND_MAX; // b
	abc[2] = (featuretype)rand() / RAND_MAX; // c
	abc[3] = (featuretype)rand() / RAND_MAX; // threshold
	return 0;
}