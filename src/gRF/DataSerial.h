
#pragma once
#include "Data.h"

class DataSerial : public Data
{
public:
	
	int ElementCount;
	Data** Elements;

	int ElementsMemoryAlloCount;

	/*double* GetFeaturep;
	double* GetFeaturepend;
	int GetFeatureEI;
	int* GetLabelp;
	int* GetLabelpend;
	int GetLabelEI;
	int* SetSplitFlagp;
	int* SetSplitFlagpend;
	int SetSplitFlagEI;*/

	int* EN;

	__declspec(dllexport) DataSerial();

	__declspec(dllexport) int AddElement(Data* data);
	
	__declspec(dllexport) virtual featuretype* GetFeatureP(int index, featuretype* abc);
	__declspec(dllexport) virtual void GetFeature(int index, featuretype* abc, featuretype* feature_out);
	__declspec(dllexport) virtual labeltype* GetLabelP(int index);
	__declspec(dllexport) virtual void GetFeatureLabel(int index, featuretype* abc, featuretype* feature_out, labeltype* label_out);
	__declspec(dllexport) virtual void GetLabel(int index, labeltype* label_out);

	__declspec(dllexport) virtual int SetPrediction(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction(int index);
	__declspec(dllexport) virtual int SetPrediction2(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction2(int index);
	__declspec(dllexport) virtual int SetPrediction3(int index, labeltype prediction);
	__declspec(dllexport) virtual labeltype GetPrediction3(int index);
	__declspec(dllexport) virtual int SetReachedNode(int index, Node* prediction);
	__declspec(dllexport) virtual Node* GetReachedNode(int index);

	//__declspec(dllexport) virtual int FastInit();
	//__declspec(dllexport) virtual double* GetFeatureFast();
	//__declspec(dllexport) virtual int GetLabelFast();
	//__declspec(dllexport) virtual void SetSplitFlagFast(int flag);

	//__declspec(dllexport) virtual int NewSplitFlags();
	//__declspec(dllexport) virtual int GetLabelCount(int k);

	////virtual int SetChildrenN0();
	//__declspec(dllexport) virtual Data** NewChildren();
	//__declspec(dllexport) virtual int Split(Data** child);

	__declspec(dllexport) virtual int Release();
};