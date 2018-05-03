
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
	int GetFeaturePEIndex;
	int GetLabelPEIndex;
	int GetFeatureLabelEIndex;
	int GetLabelEIndex;

	__declspec(dllexport) DataSerial();

	__declspec(dllexport) int AddElement(Data* data);
	
	__declspec(dllexport) virtual featuretype* GetFeatureP(int index, featuretype* abc, int* ei);
	__declspec(dllexport) virtual void GetFeature(int index, featuretype* abc, featuretype* feature_out, int* ei);
	__declspec(dllexport) virtual valuetype* GetValueP(int index, int* ei);
	__declspec(dllexport) virtual void GetFeatureValue(int index, featuretype* abc, featuretype* feature_out, valuetype* value_out, int* ei);
	__declspec(dllexport) virtual void GetValue(int index, valuetype* value_out, int* ei);
	__declspec(dllexport) virtual int SetReachedNode(int index, Node* prediction, int* ei);
	__declspec(dllexport) virtual Node* GetReachedNode(int index, int* ei);

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