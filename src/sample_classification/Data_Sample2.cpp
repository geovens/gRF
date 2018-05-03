
#include "Data_sample2.h"

Data_Sample2::Data_Sample2()
{
	D = 1;
}

void Data_Sample2::GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei)
{
	if (abc[0] < 0.1 && abc[1] < 0.1 && abc[2] < 0.1)
		feature_out[0] = x; // provide the meaningful feature value
	else
		feature_out[0] = (featuretype)rand() / RAND_MAX; // provide meaningless feature values
}

void Data_Sample2::GetValue(int index, valuetype* value_out, int* ei)
{
	*value_out = v;
}
