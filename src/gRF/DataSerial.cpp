
#include "DataSerial.h"
#include "stdafx.h"

DataSerial::DataSerial()
{
	N = 0;
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

featuretype* DataSerial::GetFeatureP(int index, featuretype* abc, int* ei)
{
	//if (index >= EN[GetFeaturePEIndex])
	//{
	//	if (GetFeaturePEIndex == ElementCount - 1 || index < EN[GetFeaturePEIndex + 1])
	//		return Elements[GetFeaturePEIndex]->GetFeatureP(index - EN[GetFeaturePEIndex]);
	//	else
	//	{
	//		GetFeaturePEIndex++;
	//		return GetFeatureP(index);
	//	}
	//}
	//else
	{
		if (index < EN[*ei])
			*ei = 0;
		while (index >= EN[*ei + 1])
			(*ei)++;
		return Elements[*ei]->GetFeatureP(index - EN[*ei], abc, NULL);
	}
}
void DataSerial::GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei)
{
	//if (index < EN[*ei])
	//	*ei = 0;
	while (index >= EN[*ei + 1])
		(*ei)++;
	Elements[*ei]->GetFeature(index - EN[*ei], abc, feature_out, NULL);
}

valuetype* DataSerial::GetValueP(int index, int* ei)
{
	if (index < EN[*ei])
		*ei = 0;
	while (index >= EN[*ei + 1])
		(*ei)++;
	return Elements[*ei]->GetValueP(index - EN[*ei], NULL);
}
void DataSerial::GetFeatureValue(int index, featuretype* abc, featuretype* feature_out, valuetype* value_out, int* ei)
{
	while (index >= EN[*ei + 1])
		(*ei)++;
	//Elements[*ei]->GetFeature(index - EN[*ei], abc, feature_out, NULL);
	//*label_out = Elements[*ei]->GetLabel(index - EN[*ei], NULL);
	Elements[*ei]->GetFeatureValue(index - EN[*ei], abc, feature_out, value_out, NULL);
}

void DataSerial::GetValue(int index, valuetype* value_out, int* ei)
{
	if (index < EN[*ei])
		*ei = 0;
	while (index >= EN[*ei + 1])
		(*ei)++;
	Elements[*ei]->GetValue(index - EN[*ei], value_out, NULL);
}

int DataSerial::SetReachedNode(int index, Node* prediction, int* ei)
{
	if (index < EN[*ei])
		*ei = 0;
	while (index >= EN[*ei + 1])
		(*ei)++;
	return Elements[*ei]->SetReachedNode(index - EN[*ei], prediction, NULL);
}

Node* DataSerial::GetReachedNode(int index, int* ei)
{
	if (index < EN[*ei])
		*ei = 0;
	while (index >= EN[*ei + 1])
		(*ei)++;
	return Elements[*ei]->GetReachedNode(index - EN[*ei], NULL);
}

//int DataSerial::SetChildrenN0()
//{
//	for (int e = 0; e < ElementN; e++)
//		Elements[e]->SetChildrenN0();
//	return 0;
//}

//int DataSerial::FastInit()
//{
//	GetFeaturep = Elements[0]->Features;
//	GetFeaturepend = GetFeaturep + Elements[0]->N * Elements[0]->D;
//	GetFeatureEI = 0;
//	while (GetFeaturep >= GetFeaturepend)
//	{
//		GetFeatureEI++;
//		//if (GetFeatureEI == ElementN) break;
//		GetFeaturep = Elements[GetFeatureEI]->Features;
//		GetFeaturepend = GetFeaturep + Elements[GetFeatureEI]->N * Elements[GetFeatureEI]->D;
//	}
//
//	GetLabelp = Elements[0]->Labels;
//	GetLabelpend = Elements[0]->Labels + Elements[0]->N;
//	GetLabelEI = 0;
//	while (GetLabelp >= GetLabelpend)
//	{
//		GetLabelEI++;
//		GetLabelp = Elements[GetLabelEI]->Labels;
//		GetLabelpend = GetLabelp + Elements[GetLabelEI]->N;
//	}
//
//	SetSplitFlagp = Elements[0]->SplitFlags;
//	SetSplitFlagpend = Elements[0]->SplitFlags + Elements[0]->N;
//	SetSplitFlagEI = 0;
//	while (SetSplitFlagp >= SetSplitFlagpend)
//	{
//		SetSplitFlagEI++;
//		//if (SetSplitFlagEI == ElementN) break;
//		SetSplitFlagp = Elements[SetSplitFlagEI]->SplitFlags;
//		SetSplitFlagpend = SetSplitFlagp + Elements[SetSplitFlagEI]->N;
//	}
//	return 0;
//}
//
//double* DataSerial::GetFeatureFast()
//{
//	double* r = GetFeaturep;
//	GetFeaturep += D;
//	while (GetFeaturep >= GetFeaturepend)
//	{
//		GetFeatureEI++;
//		if (GetFeatureEI == ElementN) break;
//		GetFeaturep = Elements[GetFeatureEI]->Features;
//		GetFeaturepend = GetFeaturep + Elements[GetFeatureEI]->N * Elements[GetFeatureEI]->D;
//	}
//	return r;
//}
//
//int DataSerial::GetLabelFast()
//{
//	int r = *GetLabelp;
//	GetLabelp++;
//	while (GetLabelp >= GetLabelpend)
//	{
//		GetLabelEI++;
//		if (GetLabelEI == ElementN) break;
//		GetLabelp = Elements[GetLabelEI]->Labels;
//		GetLabelpend = GetLabelp + Elements[GetLabelEI]->N;
//	}
//	return r;
//}
//
//void DataSerial::SetSplitFlagFast(int flag)
//{
//	*SetSplitFlagp = flag;
//	SetSplitFlagp++;
//	while (SetSplitFlagp >= SetSplitFlagpend)
//	{
//		SetSplitFlagEI++;
//		if (SetSplitFlagEI == ElementN) break;
//		SetSplitFlagp = Elements[SetSplitFlagEI]->SplitFlags;
//		SetSplitFlagpend = SetSplitFlagp + Elements[SetSplitFlagEI]->N;
//	}
//}
//
//int DataSerial::NewSplitFlags()
//{
//	for (int e = 0; e < ElementN; e++)
//	{
//		Elements[e]->NewSplitFlags();
//	}
//	return 0;
//}
//
//int DataSerial::GetLabelCount(int k)
//{
//	int c = 0;
//	for (int e = 0; e < ElementN; e++)
//	{
//		c += Elements[e]->GetLabelCount(k);
//	}
//	return c;
//}
//
//Data** DataSerial::NewChildren()
//{
//	DataSerial** child_xy = new DataSerial*[2];
//	child_xy[0] = new DataSerial();
//	child_xy[1] = new DataSerial();
//	return (Data**)child_xy;
//}
//
//int DataSerial::Split(Data** child)
//{
//	for (int e = 0; e < ElementN; e++)
//	{
//		Data** echild = Elements[e]->NewChildren();
//		((DataSerial*)child[0])->AddElement(echild[0]);
//		((DataSerial*)child[1])->AddElement(echild[1]);
//	}
//
//	for (int i = 0; i < 2; i++)
//	{
//		child[i]->D = D;
//		child[i]->K = K;
//		child[i]->N = ChildrenN[i];
//		//cv::Mat mat = cv::Mat::zeros(child[i]->N, D, CV_64FC1);
//		//child[i]->Features = new cv::Mat(mat);
//		//child[i]->Labels = new int[child[i]->N];
//		child[i]->LabelCount = new int[K];
//		memset(child[i]->LabelCount, 0, 4 * K);
//	}
//
//	for (int e = 0; e < ElementN; e++)
//	{
//		Data* lrchild[2];
//		lrchild[0] = ((DataSerial*)child[0])->Elements[e];
//		lrchild[1] = ((DataSerial*)child[1])->Elements[e];
//		Elements[e]->Split(lrchild);
//		for (int k = 0; k < K; k++)
//		{
//			child[0]->LabelCount[k] += lrchild[0]->LabelCount[k];
//			child[1]->LabelCount[k] += lrchild[1]->LabelCount[k];
//		}
//	}
//
//	return 0;
//
//	//int tick[2] = { 0, 0 };
//	//for (int i = 0; i < N; i++)
//	//{
//	//	int flag = GetSplitFlag(i);
//	//	//memcpy(childp[flag], p, D * 8);
//	//	((DataSerial*)child[0])->SetFeature(tick[flag], GetFeature(i));
//	//	//child[flag]->Labels[tick[flag]] = Labels[i];
//	//	((DataSerial*)child[0])->SetLabel(tick[flag], GetLabel(i));
//	//	child[flag]->LabelCount[GetLabel(i)]++;
//	//	tick[flag]++;
//	//	//childp[flag] += D;
//	//	//p += D;
//	//}
//
//	//return 0;
//}

int DataSerial::Release()
{
	for (int e = 0; e < ElementCount; e++)
	{
		Elements[e]->Release();
	}
	return 0;
}