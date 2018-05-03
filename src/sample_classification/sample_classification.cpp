// gRRF by Weizhi Nai

#include "gRF.h"
#include "Function_Sample1.h"
#include "Data_Sample2.h"
#include "Function_Sample2.h"

void Sample1()
{
	// This sample program generates some training samples using a fixed equation v = R(x_1, x_2, x_3, x_4) = x_1^2 + (x_2 - x_3)^2 + exp(x_4)
	// and then train a regresion forest to fit the training samples
	// and then test the forest with new testing samples.

	// This is a simple sample program which uses fixed pre-calculated feature values.
	// Fixed pre-calculated feature values are not what Random Regression Forest is really meant to be used for.
	// So see Sample2() for a better sample program.



	// plan a forest and set some parameters
	int CandidatesEachNode = 1000;
	int MaxTreeDepth = 12;
	int TreeNumber = 5;
	int MaxThreadNumber = 1;
	int TrainingN = 1000;

	RandomForest forest;
	forest.Plant(TreeNumber);
	forest.SetCandidatesEachNode(CandidatesEachNode);
	forest.SetMaxTreeDepth(MaxTreeDepth);
	forest.SetMaxThreadNumber(MaxThreadNumber);

	Function* decisionfunc = new Function_Sample1();
	forest.SetFunction(decisionfunc);

	// generates training data with fixed feature values
	Data* TrainingData = new Data();
	TrainingData->D = 4;  // x_1, x_2, x_3 and x_4
	TrainingData->N = TrainingN;
	TrainingData->Features = new featuretype[TrainingData->N * TrainingData->D];
	TrainingData->Values = new valuetype[TrainingData->N];

	featuretype* featurep = TrainingData->Features;
	valuetype* valuep = TrainingData->Values;
	for (int d = 0; d < TrainingData->N; d++)
	{
		featurep[0] = (float)rand() / RAND_MAX;
		featurep[1] = (float)rand() / RAND_MAX;
		featurep[2] = (float)rand() / RAND_MAX;
		featurep[3] = (float)rand() / RAND_MAX;
		*valuep = featurep[0] * featurep[0] + (featurep[1] - featurep[2]) * (featurep[1] - featurep[2]) + exp(featurep[3]);

		featurep += TrainingData->D;
		valuep++;
	}

	// train the forest with the training data
	// linkermode could be 1, 2 or 3
	// linkermode=1 is the fastest, but only work for pre-calculated fixed feature values
	// linkermode=2 uses less memory and more disk space. it's a legacy mode and may not be useful
	// linkermode=3 should be your default choise
	int rec = forest.Train(TrainingData, 3);
	if (rec < 0)
	{
		getchar();
		return;
	}

	// generating testing data
	Data* TestingData = new Data();
	TestingData->D = 4;
	TestingData->N = 100;
	TestingData->Features = new featuretype[TestingData->N * TestingData->D];
	TestingData->Values = new valuetype[TestingData->N];

	featurep = TestingData->Features;
	valuep = TestingData->Values;
	for (int d = 0; d < TestingData->N; d++)
	{
		featurep[0] = (float)rand() / RAND_MAX;
		featurep[1] = (float)rand() / RAND_MAX;
		featurep[2] = (float)rand() / RAND_MAX;
		featurep[3] = (float)rand() / RAND_MAX;
		*valuep = featurep[0] * featurep[0] + (featurep[1] - featurep[2]) * (featurep[1] - featurep[2]) + exp(featurep[3]);

		featurep += TestingData->D;
		valuep++;
	}

	// test the testing data using the trained forest
	forest.Test(TestingData);

	// show testing results
	float sumdiff = 0;
	featurep = TestingData->Features;
	valuep = TestingData->Values;
	for (int d = 0; d < TestingData->N; d++)
	{
		printf("(%f,%f,%f,%f) -> %f (ground truth is %f)\n", featurep[0], featurep[1], featurep[2], featurep[3], TestingData->Predictions[d], *valuep);
		sumdiff += abs(TestingData->Predictions[d] - *valuep);

		featurep += TestingData->D;
		valuep++;
	}
	printf("average error: %f\n", sumdiff / TestingData->N);

	getchar();
}

void Sample2()
{
	// Random Regression Forest is a regression tool for samples with features of infinite dimensions.
	// Each training sample has an associated target value v, and virtually infinite features, v = R(x_1, x_2, ...).
	// Each feature is parameterized with a number of parameters, i.e. x_i = F(a_i, b_i, c_i). a_i, b_i, and c_i are called feature extracting parameters.
	// Some features may have correlation with the target value, while most might not.
	// So we want to search for a number of features that have correlation with target value and use them for regression.

	// In this sample program we simulate a simple problem: when a, b and c are all below 0.1, the feature value x = F(a, b, c) will be a 'meaningful' one which conforms to the euqation v = x^2
	// while otherwise, the feature value x = F(a, b, c) is random and meaningless.
	// Each sample is associated with a fixed meaningful feature value x and a fixed target value v = x^2.

	// In summary, to use gRRF, you need to derive two classes, Data and Function, and implement 4 functions according to your problem.
	// The Data class represents your training and testing data. You can consider it as a wrapper or a feature extracting interface of your data.
	// In Data class, you need to implement the feature extraction function GetFeature(), taking the feature extracting parameters as input. 
	// You also need to implement GetValue() in the Data class.
	// In Function class you need to implement RandomABC() and TestFeature().
	// RandomABC() randomly generates feature extracting parameters.
	// TestFeature() defines the splitting function used by all none-leaf nodes, taking feature extracting parameters and feature values (extracted by GetFeature()) as input.
	// The return value of TestFeature() should be either 0 or 1, corresponding to left child node and right child node respectively.



	// plan a forest and set some parameters
	int CandidatesEachNode = 1000; // how many sets of feature extracting parameters are randomly generated when training each node. Each node will choose a 'best' one from these many candidates.
	int MaxTreeDepth = 12; 
	int TreeNumber = 5;
	int MaxThreadNumber = 4; // when using more than 1 thread to train, the output texts are disordered but that is OK.
	int TrainingN = 1000;
	int TestingN = 100;

	RandomForest forest;
	forest.Plant(TreeNumber);
	forest.SetCandidatesEachNode(CandidatesEachNode);
	forest.SetMaxTreeDepth(MaxTreeDepth);
	forest.SetMaxThreadNumber(MaxThreadNumber);

	Function* decisionfunc = new Function_Sample2();
	forest.SetFunction(decisionfunc);

	// generates training data
	DataSerial* TrainingData = new DataSerial();
	TrainingData->D = 1;
	for (int d = 0; d < TrainingN; d++)
	{
		Data_Sample2* adata = new Data_Sample2();
		adata->D = 1;
		adata->N = 1;
		adata->x = (float)rand() / RAND_MAX;
		adata->v = pow(adata->x, 2);
		TrainingData->AddElement(adata);
	}

	// train the forest with the training data
	// linkermode could be 2 or 3. 1 does not work here
	int rec = forest.Train(TrainingData, 3);
	if (rec < 0)
	{
		getchar();
		return;
	}

	// generating testing data
	DataSerial* TestingData = new DataSerial();
	TestingData->D = 1;
	for (int d = 0; d < TestingN; d++)
	{
		Data_Sample2* adata = new Data_Sample2();
		adata->D = 1;
		adata->N = 1;
		adata->x = (float)rand() / RAND_MAX;
		adata->v = pow(adata->x, 2);
		TestingData->AddElement(adata);
	}

	// test the testing data using the trained forest
	forest.Test(TestingData);

	// show testing results
	float sumdiff = 0;
	for (int d = 0; d < TestingData->N; d++)
	{
		float x = ((Data_Sample2*)(TestingData->Elements[d]))->x;
		float v = ((Data_Sample2*)(TestingData->Elements[d]))->v;
		printf("(%f) -> %f (ground truth is %f)\n", x, TestingData->Predictions[d], v);
		sumdiff += abs(TestingData->Predictions[d] - v);
	}
	printf("average error: %f\n", sumdiff / TestingData->N);

	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	Sample1();
	//Sample2();
	return 0;
}