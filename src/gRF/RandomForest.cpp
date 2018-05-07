
#include "RandomForest.h"
#include "RandomTree.h"
#include "Node.h"
#include "Data.h"

RandomForest::RandomForest()
{
	TreeCount = 0;
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
	double* hist = new double[data->K];
	for (int k = 0; k < data->K; k++)
		hist[k] = 0;

	data->PredictionHists = new double*[data->N];


	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].ThisData = data;
		// temp for display
		//double rec = Trees[n].Test(data);
		//printf("recognition rate of tree no.%d: %lf\n", n, rec);
	}

	featuretype* feature_temp_store = new featuretype[data->D];
	for (int i = 0; i < data->N; i++)
	{
		data->PredictionHists[i] = new double[data->K];
		for (int k = 0; k < data->K; k++)
			hist[k] = 0;

		Node* node = NULL;
		for (int n = 0; n < TreeCount; n++)
		{
			node = Trees[n].TestFeature(i, feature_temp_store);
			for (int k = 0; k < data->K; k++)
			{
				//hist[k] += node->SampleReachedCount[k];
				hist[k] += node->HistLabels[k];
			}
		}

		data->SetReachedNode(i, node);
		memcpy(data->PredictionHists[i], hist, sizeof(double) * data->K);

		double maxvote = 0;
		int maxk = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (hist[k] > maxvote)
			{
				maxvote = hist[k];
				maxk = k;
			}
		}
		data->SetPrediction(i, maxk);

		double maxvote2 = 0;
		int maxk2 = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (k != maxk && hist[k] > maxvote2)
			{
				maxvote2 = hist[k];
				maxk2 = k;
			}
		}
		if (maxk2 == 0 && maxvote2 == 0)
			data->SetPrediction2(i, -1);
		else
			data->SetPrediction2(i, maxk2);

		double maxvote3 = 0;
		int maxk3 = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (k != maxk && k != maxk2 && hist[k] > maxvote3)
			{
				maxvote3 = hist[k];
				maxk3 = k;
			}
		}
		if (maxk3 == 0 && maxvote3 == 0)
			data->SetPrediction3(i, -1);
		else
			data->SetPrediction3(i, maxk3);
	}
	delete hist;
	delete feature_temp_store;
	return 0;
}

int RandomForest::Test(Data* data, int level)
{
	double* hist = new double[data->K];
	for (int k = 0; k < data->K; k++)
		hist[k] = 0;
	data->PredictionHists = new double*[data->N];

	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].ThisData = data;
		// temp for display
		//double rec = Trees[n].Test(data);
		//printf("recognition rate of tree no.%d: %lf\n", n, rec);
	}

	featuretype* feature_temp_store = new featuretype[data->D];
	for (int i = 0; i < data->N; i++)
	{
		data->PredictionHists[i] = new double[data->K];
		for (int k = 0; k < data->K; k++)
			hist[k] = 0;

		Node* node = NULL;
		for (int n = 0; n < TreeCount; n++)
		{
			node = Trees[n].TestFeature(i, level, feature_temp_store);
			for (int k = 0; k < data->K; k++)
			{
				//hist[k] += node->SampleReachedCount[k];
				hist[k] += node->HistLabels[k];
			}
		}

		data->SetReachedNode(i, node);
		memcpy(data->PredictionHists[i], hist, sizeof(double) * data->K);

		double maxvote = 0;
		int maxk = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (hist[k] > maxvote)
			{
				maxvote = hist[k];
				maxk = k;
			}
		}
		data->SetPrediction(i, maxk);

		double maxvote2 = 0;
		int maxk2 = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (k != maxk && hist[k] > maxvote2)
			{
				maxvote2 = hist[k];
				maxk2 = k;
			}
		}
		if (maxk2 == 0 && maxvote2 == 0)
			data->SetPrediction2(i, -1);
		else
			data->SetPrediction2(i, maxk2);

		double maxvote3 = 0;
		int maxk3 = 0;
		for (int k = 0; k < data->K; k++)
		{
			if (k != maxk && k != maxk2 && hist[k] > maxvote3)
			{
				maxvote3 = hist[k];
				maxk3 = k;
			}
		}
		if (maxk3 == 0 && maxvote3 == 0)
			data->SetPrediction3(i, -1);
		else
			data->SetPrediction3(i, maxk3);
	}
	delete hist;
	delete feature_temp_store;
	return 0;
}

void RandomForest::Release()
{
	for (int n = 0; n < TreeCount; n++)
	{
		Trees[n].Release();
	}
	TreeCount = 0;
}
