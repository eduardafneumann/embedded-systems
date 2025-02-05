#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void convolution(int output_size, float *output, int input_size, float *input, int kernel_size, float *kernel)
{
    for (int i = 0; i < output_size; i++)
    {
        for (int j = 0; j < output_size; j++)
        {
            float result = 0;
            for (int ki = 0; ki < kernel_size; ki++)
            {
                for (int kj = 0; kj < kernel_size; kj++)
                {
                    result += kernel[ki * kernel_size + kj] * input[(i + ki) * input_size + j + kj];
                }
            }
            *(output + i * output_size + j) = result >= 0 ? result : 0;
        }
    }
}

void multiply(int output_size, float *output, int input_size, float *input, float *weights)
{
    for (int i = 0; i < output_size; i++)
    {
        float result = 0;
        for (int j = 0; j < input_size; j++)
        {
            result += weights[i + j * output_size] * input[j];
        }
        output[i] = result;
    }

    float sum = 0;
    for (int i = 0; i < output_size; i++)
    {
        output[i] = expf(output[i]);
        sum += output[i];
    }
    for (int i = 0; i < output_size; i++)
    {
        output[i] /= sum;
    }
}

int main()
{
#include "../weights/first_convolution.txt"
#include "../weights/second_convolution.txt"
#include "../weights/third_convolution.txt"
#include "../weights/fully_connected.txt"
#include "../images/image0.txt"

    int layer1_size = IMAGE_SIZE - KERNEL_SIZE_1 + 1;
    float layer1[layer1_size][layer1_size];

    int layer2_size = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 + 2;
    float layer2[layer2_size][layer2_size];

    int layer3_size = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 - KERNEL_SIZE_3 + 3;
    float layer3[layer3_size][layer3_size];

    int flat_size = layer3_size * layer3_size;
    float flat[flat_size];

    float output[OUTPUT_SIZE];

    convolution(layer1_size, &layer1[0][0], IMAGE_SIZE, &image[0][0], KERNEL_SIZE_1, &first_convolution[0][0]);
    convolution(layer2_size, &layer2[0][0], layer1_size, &layer1[0][0], KERNEL_SIZE_2, &second_convolution[0][0]);
    convolution(layer3_size, &layer3[0][0], layer2_size, &layer2[0][0], KERNEL_SIZE_3, &third_convolution[0][0]);
    multiply(OUTPUT_SIZE, &output[0], flat_size, &layer3[0][0], &fully_connected[0][0]);

    float max = output[0];
    int prediction = 0;
    for (int i = 1; i < OUTPUT_SIZE; i++)
    {
        printf("%f ", output[i]);
        if (output[i] > max)
        {
            max = output[i];
            prediction = i;
        }
    }
    printf("\n%d\n", prediction);

    return 0;
}
