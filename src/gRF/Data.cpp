
#include "Data.h"
#include "stdafx.h"


Data::Data()
{
	ReachedNodes = NULL;
}

featuretype* Data::GetFeatureP(int index, featuretype* abc, int* ei)
{
	return Features + index * D;
}

void Data::GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei)
{
	memcpy(feature_out, Features + index * D, sizeof(featuretype)* D);
}

valuetype* Data::GetValueP(int index, int* ei)
{
	return Values + index;
}

void Data::GetFeatureValue(int index, featuretype* abc, featuretype* feature_out, valuetype* value_out, int* ei)
{
	/*
	memcpy(feature_out, Features + index * D, sizeof(featuretype) * D);
	*label_out = *(Values + index);
	*/
	GetFeature(index, abc, feature_out, ei);
	GetValue(index, value_out, ei);
}

void Data::GetValue(int index, valuetype* value_out, int* ei)
{
	*value_out = *(Values + index);
}

int Data::SetReachedNode(int index, Node* node, int* ei)
{
	if (ReachedNodes == NULL)
		ReachedNodes = new Node*[N];
	ReachedNodes[index] = node;
	return 0;
}

Node* Data::GetReachedNode(int index, int* ei)
{
	if (ReachedNodes == NULL)
		return NULL;
	return ReachedNodes[index];
}

int Data::Release()
{
	delete Features;
	delete Values;
	//delete SplitFlags;
	//delete LabelCount;
	delete this;
	return 0;
}