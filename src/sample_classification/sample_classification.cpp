// gRRF by Weizhi Nai

#include "gRF.h"
#include "Function_Sample1.h"
#include "Data_Sample2.h"
#include "Function_Sample2.h"

void Sample1()
{
	// This sample program generates some training samples of only two feature dimensions (x_1, x_2) and a simple ground truth classification rule C(x_1, x_2):
	// when x_1 < 0.5 and x_2 < 0.5, the label is 0;
	// when x_1 < 0.5 and x_2 >= 0.5, the label is 1;
	// when x_1 >= 0.5 and x_2 < 0.5, the label is 2;
	// when x_1 >= 0.5 and x_2 >= 0.5, the label is 3.
	// A random forest classifier is trained to model the training samples
	// and then is tested with new testing samples.

	// This is a simple sample program which uses fixed pre-calculated feature values.
	// Fixed pre-calculated feature values are not what Random Forest is really meant to be used for.
	// So see Sample2() for a better sample program.



	// plan a forest and set some parameters
	int CandidatesEachNode = 100;
	int MaxTreeDepth = 5;
	int TreeNumber = 2;
	int MaxThreadNumber = 1;
	int TrainingN = 1000000;

	RandomForest forest;
	forest.Plant(TreeNumber);
	forest.SetCandidatesEachNode(CandidatesEachNode);
	forest.SetMaxTreeDepth(MaxTreeDepth);
	forest.SetMaxThreadNumber(MaxThreadNumber);

	Function* decisionfunc = new Function_Sample1();
	forest.SetFunction(decisionfunc);

	// generates training data with fixed feature values
	Data* TrainingData = new Data();
	TrainingData->D = 2;  // x_1 and x_2
	TrainingData->K = 4;  // 4 classes: labeled with 0, 1, 2 and 3
	TrainingData->N = TrainingN;
	TrainingData->Features = new featuretype[TrainingData->N * TrainingData->D];
	TrainingData->Labels = new labeltype[TrainingData->N];

	featuretype* featurep = TrainingData->Features;
	labeltype* labelp = TrainingData->Labels;
	for (int d = 0; d < TrainingData->N; d++)
	{
		featurep[0] = (float)rand() / RAND_MAX;
		featurep[1] = (float)rand() / RAND_MAX;
		if (featurep[0] < 0.5 && featurep[1] < 0.5) *labelp = 0;
		if (featurep[0] < 0.5 && featurep[1] >= 0.5) *labelp = 1;
		if (featurep[0] >= 0.5 && featurep[1] < 0.5) *labelp = 2;
		if (featurep[0] >= 0.5 && featurep[1] >= 0.5) *labelp = 3;

		featurep += TrainingData->D;
		labelp++;
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
	TestingData->D = 2;
	TestingData->K = 4;
	TestingData->N = 100;
	TestingData->Features = new featuretype[TestingData->N * TestingData->D];
	TestingData->Labels = new labeltype[TestingData->N];

	featurep = TestingData->Features;
	labelp = TestingData->Labels;
	for (int d = 0; d < TestingData->N; d++)
	{
		featurep[0] = (float)rand() / RAND_MAX;
		featurep[1] = (float)rand() / RAND_MAX;
		if (featurep[0] < 0.5 && featurep[1] < 0.5) *labelp = 0;
		if (featurep[0] < 0.5 && featurep[1] >= 0.5) *labelp = 1;
		if (featurep[0] >= 0.5 && featurep[1] < 0.5) *labelp = 2;
		if (featurep[0] >= 0.5 && featurep[1] >= 0.5) *labelp = 3;

		featurep += TestingData->D;
		labelp++;
	}

	// test the testing data using the trained forest
	forest.Test(TestingData);

	// show testing results
	int wrong = 0;
	featurep = TestingData->Features;
	labelp = TestingData->Labels;
	for (int d = 0; d < TestingData->N; d++)
	{
		labeltype prediction = TestingData->GetPrediction(d);
		printf("(%f,%f) -> label %d (ground truth is label %d)\n", featurep[0], featurep[1], prediction, *labelp);
		wrong += prediction != *labelp ? 1 : 0;

		featurep += TestingData->D;
		labelp++;
	}
	printf("error rate: %f\n", (float)wrong / TestingData->N);

	forest.Release();
	getchar();
}

void Sample2()
{
	// Random Forest Classifier is a classification tool for samples with features of infinite dimensions.
	// Each training sample has an associated label l, and virtually infinite features, l = C(x_1, x_2, ...).
	// Each feature is parameterized with a number of parameters, i.e. x_i = F(a_i, b_i, c_i). a_i, b_i, and c_i are called feature extracting parameters.
	// Some features may have correlation with the label, while most might not.
	// So we want to search for a number of features that have correlation with label and use them for classification.

	// In this sample program we simulate a simple problem: there are a total of 100 classes (l = 0, 1, ..., 99)
	// and when a, b and c are all below 0.2, the feature value x = F(a, b, c) extracted will be a 'meaningful' one which conforms to the euqation x = l*2
	// while otherwise, the feature value x = F(a, b, c) is random and meaningless.
	// Each sample is associated with a fixed meaningful feature value x = l*2 and a fixed meaningless feature value.

	// In summary, to use gRF, you need to derive two classes, Data and Function, and implement 4 functions according to your problem.
	// The Data class represents your training and testing data. You can consider it as a wrapper or a feature extracting interface of your data.
	// In Data class, you need to implement the feature extraction function GetFeature(), taking the feature extracting parameters as input. 
	// You also need to implement GetLabel() in the Data class.
	// In Function class you need to implement RandomABC() and TestFeature().
	// RandomABC() randomly generates feature extracting parameters.
	// TestFeature() defines the splitting function used by all none-leaf nodes, taking feature extracting parameters and feature values (extracted by GetFeature()) as input.
	// The return value of TestFeature() should be either 0 or 1, corresponding to left child node and right child node respectively.



	// plan a forest and set some parameters
	int CandidatesEachNode = 1000; // how many sets of feature extracting parameters are randomly generated when training each node. Each node will choose a 'best' one from these many candidates.
	int MaxTreeDepth = 12; 
	int TreeNumber = 3;
	int MaxThreadNumber = 4; // when using more than 1 thread to train, the output texts are disordered but that is OK.
	int TrainingN = 10000;
	int TestingN = 100;

	RandomForest forest;
	forest.Plant(TreeNumber);
	forest.SetCandidatesEachNode(CandidatesEachNode);
	forest.SetMaxTreeDepth(MaxTreeDepth);
	forest.SetMaxThreadNumber(MaxThreadNumber);
	Function* decisionfunc = new Function_Sample2();
	forest.SetFunction(decisionfunc);

	// generates training data, organized in 3 hierarchical levels
	DataSerial* TrainingData = new DataSerial();
	TrainingData->D = 1;
	TrainingData->K = 100;
	for (int group = 0; group < 5; group++)
	{
		DataSerial* SampleGroup = new DataSerial();
		SampleGroup->D = 1;
		SampleGroup->K = 100;
		for (int d = 0; d < TrainingN / 5; d++)
		{
			Data_Sample2* OneSample = new Data_Sample2();
			OneSample->D = 1;
			OneSample->K = 100;
			OneSample->N = 1;
			OneSample->l = rand() % 100; // 0 ~ 99
			OneSample->x = 2 * OneSample->l;
			OneSample->x_noise = (featuretype)rand() / RAND_MAX * 200;
			SampleGroup->AddElement(OneSample);
		}
		TrainingData->AddElement(SampleGroup);
	}

	// train the forest with the training data
	// linkermode could be 2 or 3. 1 does not work here
	int rec = forest.Train(TrainingData, 3);
	if (rec < 0)
	{
		getchar();
		return;
	}

	// delete the trained tree as if ending the training program
	forest.Release();
	getchar();


	// reload the previously trained tree as if the new testing program starting
	RandomForest forest_test;
	forest_test.Plant(TreeNumber);
	decisionfunc = new Function_Sample2();
	forest_test.SetFunction(decisionfunc);
	forest_test.Load(); // load the previously trained model from ./output/ folder which was automatically saved during training

	// generating testing data, organized in 3 hierarchical levels
	DataSerial* TestingData = new DataSerial();
	TestingData->D = 1;
	TestingData->K = 100;
	for (int group = 0; group < 5; group++)
	{
		DataSerial* SampleGroup = new DataSerial();
		SampleGroup->D = 1;
		SampleGroup->K = 100;
		for (int d = 0; d < TestingN / 5; d++)
		{
			Data_Sample2* OneSample = new Data_Sample2();
			OneSample->D = 1;
			OneSample->K = 100;
			OneSample->N = 1;
			OneSample->l = rand() % 100; // 0 ~ 99
			OneSample->x = 2 * OneSample->l;
			OneSample->x_noise = (featuretype)rand() / RAND_MAX * 200; // from 0 to 200, same range as the meaningful feature values
			SampleGroup->AddElement(OneSample);
		}
		TestingData->AddElement(SampleGroup);
	}

	// test the testing data using the trained forest
	forest_test.Test(TestingData);

	// show testing results
	int wrong = 0;
	for (int d = 0; d < TestingData->N; d++)
	{
		int local_index_out;
		Data_Sample2* thesample = (Data_Sample2*)(TestingData->GetSample(d, &local_index_out));
		
		labeltype prediction;
		prediction = TestingData->GetPrediction(d);
		// or
		prediction = thesample->GetPrediction(local_index_out);
		
		printf("(%f) -> label %d (ground truth is label %d)\n", thesample->x, prediction, thesample->l);
		wrong += prediction != thesample->l ? 1 : 0;
	}
	printf("error rate: %f\n", (float)wrong / TestingData->N);

	forest_test.Release();
	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	Sample1();
	//Sample2();
	return 0;
}