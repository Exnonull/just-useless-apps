#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#define RAND_MAX 1714636928.0f//32767.0f






typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Neuron_t
{
	u16 length;
	float* w;
} Neuron;

typedef	struct Layer_t
{
	u16 length;
	Neuron** neurons;
} Layer;

typedef struct Network_t
{
	u16 length;
	Layer** layers;
} Network;









void setNeuron (Neuron* neuron, float* weights)
{
	int i;
	for (i = 0; i < neuron->length; i++)
		neuron->w[i] = weights[i];
}

Neuron* newNeuron (int links)
{
	Neuron* neuron = (Neuron*)malloc(sizeof(Neuron));
	if (neuron == NULL) return NULL;

	links++;
	neuron->length = links;
	neuron->w = (float*)malloc(links*sizeof(float));
	if (neuron->w == NULL)
	{
		free(neuron);
		return NULL;
	}

	int i;
	for (i = 0; i < links; i++)
		neuron->w[i] = ((float)rand())/RAND_MAX;

	return neuron;
}







float predictSigmoid (Neuron* neuron, float* x)
{
	float sum = 0;
	int i;
	for (i = 0; i < neuron->length-1; i++){
		sum += x[i] * neuron->w[i];
	}
	sum += 1.0f * neuron->w[neuron->length-1];

	return 1 / (1 + exp(-sum));
}

void editWeights (Neuron* neuron, float* x, float error, float rate)
{
	int i;
	for (i = 0; i < neuron->length-1; i++)
		neuron->w[i] += error * x[i] * rate;
	neuron->w[neuron->length-1] += error * rate;
}

u8 trainSigmoid (Neuron* neuron, float* x, float error, float rate)
{
	float y = x[neuron->length-1];
	if (error == .0f) error = 0.1f;
	if (rate == .0f) rate = 0.1f;

	float current_y = predictSigmoid(neuron, x);
	if (y-error <= current_y && current_y <= y+error) return 1; 

	editWeights(neuron, x, y-current_y, rate);
	return 0;
}

void learnSigmoid (Neuron* neuron, float** x, u16 length, float error, float rate)
{
	u8 success = 0;
	int i;
	while (success == 0) {
		success = 1;
		for (i = 0; i < length; i++)
			if (trainSigmoid(neuron, x[i], error, rate) == 0)
				success = 0;
	}
}











void setNetwork (Network* network, float*** weights)
{
	int i, i2, i3;
	for (i = 0; i < network->length; i++)
		for (i2 = 0; i2 < network->layers[i]->length; i2++)
			setNeuron(network->layers[i]->neurons[i2], weights[i][i2]);
}

Network* newNetwork (int inputLength,  int* layers,int layersLength,  int outputLength)
{
	Network* network = (Network*)malloc(sizeof(Network));
	if (network == NULL) return NULL;


	int i, i2, oldLength = inputLength, length;
	layersLength++;//input(0) + layers(layersLength) + output(1)

	network->length = layersLength;
	network->layers = (Layer**)malloc(layersLength*sizeof(Layer*));
	for (i = 0; i < layersLength; i++) network->layers[i] = (Layer*)malloc(sizeof(Layer));

	if (network->layers == NULL)
	{
		free(network);
		return NULL;
	}

	for (i = 0; i < layersLength; i++)
	{
		if (i == layersLength-1) length = outputLength;
		else length = layers[i];

		network->layers[i]->length = length;
		network->layers[i]->neurons = (Neuron**)malloc(length*sizeof(Neuron*));
		for (i2 = 0; i2 < length; i2++) {
			network->layers[i]->neurons[i2] = newNeuron(oldLength);
		}

		if (i < layersLength - 1) oldLength = layers[i];
	}



	return network;
}










float* predictNetwork (Network* network, float* x)
{
	float* prev = x;
	int i, i2;
	for (i = 0; i < network->length; i++)
	{
		float* curr = (float*)malloc(network->layers[i]->length*sizeof(float));
		for (i2 = 0; i2 < network->layers[i]->length; i2++)
		{
			curr[i2] = predictSigmoid(network->layers[i]->neurons[i2], prev);
			printf("layer[%i] neuron[%i] result: %f\n", i, i2, curr[i2]);
		}
		if (i > 0) free(prev);
		prev = curr;
	}

	return prev;
}

float** predictFullNetwork (Network* network, float* x)
{
	float** full = (float**)malloc(network->length*sizeof(float*));
	float* prev = x;
	int i, i2;
	for (i = 0; i < network->length; i++)
	{
		float* curr = (float*)malloc(network->layers[i]->length*sizeof(float));
		for (i2 = 0; i2 < network->layers[i]->length; i2++)
		{
			curr[i2] = predictSigmoid(network->layers[i]->neurons[i2], prev);
			//printf("layer[%i] neuron[%i] result: %f\n", i, i2, curr[i2]);
		}
		prev = curr;
		full[i] = curr;
	}
	//printf("\n");

	return full;
}

//*
u8 trainNetwork (Network* network, float* x, float* y, float accuracy, float rate)
{
	if (accuracy == .0f) accuracy = .1f;
	if (rate == .0f) rate = .5f;

	float** full = predictFullNetwork(network, x);
	float* result = full[network->length-1];
	int i, success = 1;
	for (i = 0; i < network->length; i++)
		if (y[i]-accuracy <= result[i] && result[i] <= y[i]+accuracy)
		{
			success = 0;
			break;
		}
	if (success) return 1;



	float* cur_layer;
	float* prev_layer;
	int i2, i3;
	float* oldErrors = NULL;
	float* curErrors = NULL;
	for (i = network->length-1; i >= 0; i--)
	{
		cur_layer = full[i];

		if (i == 0) prev_layer = x;
		else prev_layer = full[i-1];

		if (oldErrors != NULL) free(oldErrors);
		oldErrors = curErrors;
		curErrors = (float*)malloc(network->layers[i]->length*sizeof(float));

		for (i2 = 0; i2 < network->layers[i]->length; i2++)
		{
			Neuron* neuron = network->layers[i]->neurons[i2];
			float current_y = cur_layer[i2];
			float* z = prev_layer;
			float error = 0;

			if (i == network->length-1)//last layer
			{
				error = y[i2] - current_y;
				curErrors[i2] = error;
			}
			else//hidden layer
			{
				Layer* nextLayer = network->layers[i+1];
				for (i3 = 0; i3 < nextLayer->length; i3++)
					error += oldErrors[i2] * nextLayer->neurons[i3]->w[i2];//sum(error[i]*weight[i])
				curErrors[i2] = error;
			}

			editWeights(neuron, z, error, rate);
		}
	}

	return 0;
}//*/

void learnNetwork (Network* network, float** x, float** y, u16 length, float error, float rate, int cycles)
{
	if (cycles == 0) cycles = 5000;
	int iter = 0;
	int i;
	int success = 0;
	while (success == 0)
	{
		success = 1;
		for (i = 0; i < length; i++)
			if (trainNetwork(network, x[i], y[i], error, rate) == 0)
				success = 0;

		iter++;
		if (iter >= cycles) break;
	}
}









/*//Sigmoid
	srand(time(NULL));
	const int links = 2;//2 связи (2 входа)
	Neuron* neuron = newNeuron(links);



	u8 trainSize = 4;
	float** x = (float**)malloc(trainSize*sizeof(float*));
	x[0] = (float[3]){1.0f, 1.0f, .0f};
	x[1] = (float[3]){1.0f, .0f, .0f};
	x[2] = (float[3]){.0f, 1.0f, .0f};
	x[3] = (float[3]){.0f, .0f, 1.0f};
	learnSigmoid(neuron, x, trainSize, .01f, .0f);



	float* x2 = (float[2]){.0f, .0f};
	printf("result: %f", predictSigmoid(neuron, x2));
*/

int main() {
	srand(time(NULL));
	Network* network = newNetwork(3,  (int[1]){3},1,  1);

	u8 trainSize = 7;
	float** x = (float**)malloc(trainSize*sizeof(float*));
	float** y = (float**)malloc(trainSize*sizeof(float*));

	x[0] = (float[3]){.0f, .0f, .0f};
	y[0] = (float[1]){1.0f};

	x[1] = (float[3]){.5f, .5f, .5f};
	y[1] = (float[1]){1.0f};

	x[2] = (float[3]){1.0f, 1.0f, 1.0f};
	y[2] = (float[1]){1.0f};

	x[3] = (float[3]){1.0f, .0f, .0f};
	y[3] = (float[1]){0.0f};

	x[4] = (float[3]){1.0f, .0f, 1.0f};
	y[4] = (float[1]){0.0f};

	x[5] = (float[3]){.0f, .5f, .0f};
	y[5] = (float[1]){0.0f};

	x[6] = (float[3]){.0f, 1.0f, .5f};
	y[6] = (float[1]){0.0f};


	learnNetwork(network, x, y, trainSize, .01f, .0f, 0);

	//printf("train: %i", learnNetwork(network, x, y, .01f, .0f));
	printf("result: %f", predictNetwork(network, (float[3]){.0f, .0f, .0f})[0]);

	return 0;
}








