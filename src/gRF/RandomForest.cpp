
#include "RandomForest.h"
#include "RandomTree.h"
#include "Node.h"
#include "Data.h"

RandomForest::RandomForest()
{
}

int RandomForest::Plant(int count)
{
	TreeCount = count;
	Trees = new RandomTree[count]();
	for (int n = 0; n < count; n++)
	{
		Trees[n].ID = n;
		Trees[n].Plant();
	}
	return 0;
}

int RandomForest::SetFunction(Function* function)
{
	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].Function = function;
	}
	return 0;
}

int RandomForest::SetCandidatesEachNode(int cen)
{
	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].CandidatesEachNode = cen;
	}
	return 0;
}

int RandomForest::SetMaxTreeDepth(int depth)
{
	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].MaxDepth = depth;
	}
	return 0;
}

int RandomForest::SetMaxThreadNumber(int thread)
{
	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].MaxThreadNumber = thread;
	}
	return 0;
}

int RandomForest::Train(Data* data, int linkermode)
{
	_mkdir("./output");
	if (linkermode == 2) _mkdir("./internal");

	for (int n = 0; n < TreeCount; n++)
	{
		printf("Tree No. %d\n", n);
		int rec = Trees[n].Train(data, linkermode);
		if (rec < 0) return rec;
	}
	return 0;
}

int RandomForest::Test(Data* data)
{
	data->ReachedNodes = new Node*[data->N];
	data->Predictions = new valuetype[data->N];

	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].ThisData = data;
		// temp for display
		//double rec = Trees[n].Test(data);
		//printf("recognition rate of tree no.%d: %lf\n", n, rec);
	}

	int* ei = new int[TreeCount];
	memset(ei, 0, sizeof(int) * TreeCount);
	int EInode = 0;
	featuretype* feature_temp_store = new featuretype[data->D];
	for (int i = 0; i < data->N; i++)
	{
		valuetype sum = 0;
		int sumn = 0;
		Node* node = NULL;
		for (int n = 0; n < TreeCount; n++)
		{
			node = Trees[n].TestFeature(i, ei + n, feature_temp_store);
			sum += node->AverageValue * node->N;
			//sum += node->AverageValue;
			sumn += node->N;
			if (node->N == 0) printf("WARNING: N==0");
		}

		data->Predictions[i] = sum / sumn;
		//data->Predictions[i] = sum / TreeCount;
		data->SetReachedNode(i, node, &EInode);
	}
	return 0;
}

int RandomForest::Test(Data* data, int level)
{
	data->ReachedNodes = new Node*[data->N];
	data->Predictions = new valuetype[data->N];

	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].ThisData = data;
		// temp for display
		//double rec = Trees[n].Test(data);
		//printf("recognition rate of tree no.%d: %lf\n", n, rec);
	}

	int* ei = new int[TreeCount];
	memset(ei, 0, sizeof(int)* TreeCount);
	int EInode = 0;
	featuretype* feature_temp_store = new featuretype[data->D];
	for (int i = 0; i < data->N; i++)
	{
		valuetype sum = 0;
		int sumn = 0;
		Node* node = NULL;
		for (int n = 0; n < TreeCount; n++)
		{
			node = Trees[n].TestFeature(i, level, ei + n, feature_temp_store);
			sum += node->AverageValue * node->N;
			//sum += node->AverageValue;
			sumn += node->N;
		}

		data->Predictions[i] = sum / sumn;
		//data->Predictions[i] = sum / TreeCount;
		data->SetReachedNode(i, node, &EInode);
	}
	return 0;
}