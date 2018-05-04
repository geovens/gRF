
#include "Data_sample2.h"

Data_Sample2::Data_Sample2()
{
	D = 1;
}

void Data_Sample2::GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei)
{
	if (abc[0] < 0.2 && abc[1] < 0.2 && abc[2] < 0.2)
		feature_out[0] = x; // provide the meaningful feature value
	else
		feature_out[0] = x_noise; // provide meaningless feature values
}

void Data_Sample2::GetLabel(int index, labeltype* label_out, int* ei)
{
	*label_out = l;
}
