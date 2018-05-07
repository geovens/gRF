
#include "DataSerial.h"
#include "stdafx.h"

DataSerial::DataSerial()
{
	N = 0;
	ElementCount = 0;
	EN = NULL;
	Elements = NULL;
	ElementsMemoryAlloCount = 0;
}

int DataSerial::AddElement(Data* data)
{
	while (ElementCount >= ElementsMemoryAlloCount - 1)
	{
		Data** newelements = new Data*[ElementsMemoryAlloCount * 2 + 1];
		memcpy(newelements, Elements, ElementCount * sizeof(Data*));
		if (Elements != NULL)
			delete Elements;
		Elements = newelements;

		int* newen = new int[ElementsMemoryAlloCount * 2 + 1];
		memcpy(newen, EN, (ElementCount) * sizeof(int));
		if (EN != NULL)
			delete EN;
		EN = newen;

		ElementsMemoryAlloCount = ElementsMemoryAlloCount * 2 + 1;
	}
	Elements[ElementCount] = data;
	EN[ElementCount] = N;
	ElementCount++;
	N += data->N;
	EN[ElementCount] = N;
	//if (LabelCount == NULL)
	//	LabelCount = new double[K];
	//for (int k = 0; k < K; k++)
	//	LabelCount[k] += data->LabelCount[k];
	return 0;
}

featuretype* DataSerial::GetFeatureP(int index, featuretype* abc)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetFeatureP(index - EN[ei], abc);
}
void DataSerial::GetFeature(int index, featuretype* abc, featuretype* feature_out)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	Elements[ei]->GetFeature(index - EN[ei], abc, feature_out);
}

labeltype* DataSerial::GetLabelP(int index)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetLabelP(index - EN[ei]);
}
void DataSerial::GetFeatureLabel(int index, featuretype* abc, featuretype* feature_out, labeltype* label_out)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	Elements[ei]->GetFeatureLabel(index - EN[ei], abc, feature_out, label_out);
}

void DataSerial::GetLabel(int index, labeltype* label_out)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	Elements[ei]->GetLabel(index - EN[ei], label_out);
}

Data* DataSerial::GetSample(int index, int* local_index_out)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetSample(index - EN[ei], local_index_out);;
}

int DataSerial::SetReachedNode(int index, Node* prediction)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->SetReachedNode(index - EN[ei], prediction);
}

Node* DataSerial::GetReachedNode(int index)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetReachedNode(index - EN[ei]);
}

int DataSerial::SetPrediction(int index, labeltype prediction)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->SetPrediction(index - EN[ei], prediction);
}

labeltype DataSerial::GetPrediction(int index)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetPrediction(index - EN[ei]);
}

int DataSerial::SetPrediction2(int index, labeltype prediction)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->SetPrediction2(index - EN[ei], prediction);
}

labeltype DataSerial::GetPrediction2(int index)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetPrediction2(index - EN[ei]);
}

int DataSerial::SetPrediction3(int index, labeltype prediction)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->SetPrediction3(index - EN[ei], prediction);
}

labeltype DataSerial::GetPrediction3(int index)
{
	int ei = 0;
	while (index >= EN[ei + 1])
		ei++;
	return Elements[ei]->GetPrediction3(index - EN[ei]);
}

int DataSerial::Release()
{
	for (int e = 0; e < ElementCount; e++)
	{
		Elements[e]->Release();
	}
	if (EN != NULL)
		delete EN;
	if (Elements != NULL)
		delete Elements;
	return 0;
}