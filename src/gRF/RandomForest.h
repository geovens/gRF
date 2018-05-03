
#pragma once
class RandomTree;
class Data;
class Function;

class RandomForest
{
public:
	int TreeCount;
	RandomTree* Trees;

	__declspec(dllexport) RandomForest();
	__declspec(dllexport) int Plant(int count);
	__declspec(dllexport) int SetFunction(Function* function);
	__declspec(dllexport) int SetCandidatesEachNode(int cen);
	__declspec(dllexport) int SetMaxTreeDepth(int depth);
	__declspec(dllexport) int SetMaxThreadNumber(int thread);
	__declspec(dllexport) int Train(Data* data, int linkermode);
	__declspec(dllexport) int Test(Data* data);
	__declspec(dllexport) int Test(Data* data, int level);
};