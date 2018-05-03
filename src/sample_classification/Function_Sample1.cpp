
#include "Function_Sample1.h"
#include "stdafx.h"

Function_Sample1::Function_Sample1()
{
	ABCNum = 2; // abc[0] and abc[1]
}

int Function_Sample1::TestFeature(featuretype* feature, featuretype* abc)
{
	return feature[(int)abc[0]] <= abc[1] ? 0 : 1; // using the selected one dimension feature value and the threshold as the splitting function on every none-leaf node
}

int Function_Sample1::RandomABC(Node* node, featuretype* abc)
{
	
	abc[0] = (featuretype)rand() / RAND_MAX * 4; // randomly select a dimension from the 4 dimensions, x_1, x_2, x_3 and x_4
	abc[1] = (featuretype)rand() / RAND_MAX * 8; // randomly select a threshold
	return 0;
}