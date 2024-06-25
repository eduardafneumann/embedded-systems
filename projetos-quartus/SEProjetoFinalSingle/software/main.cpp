#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#define debug_printf(...) printf(__VA_ARGS__)

#define IMAGE_SIZE 28
float image[IMAGE_SIZE][IMAGE_SIZE];

#define KERNEL_SIZE_1 3
float first_convolution[KERNEL_SIZE_1][KERNEL_SIZE_1];

#define KERNEL_SIZE_2 3
float second_convolution[KERNEL_SIZE_2][KERNEL_SIZE_2];

#define KERNEL_SIZE_3 3
float third_convolution[KERNEL_SIZE_3][KERNEL_SIZE_3];

#define FLAT_SIZE 484
#define OUTPUT_SIZE 10
float fully_connected[FLAT_SIZE][OUTPUT_SIZE];

void convolution(int output_size, float *output, int input_size, float *input, int kernel_size,
                 float *kernel) {
    for (int i = 0; i < output_size; i++) {
        for (int j = 0; j < output_size; j++) {
            float result = 0;
            for (int ki = 0; ki < kernel_size; ki++) {
                for (int kj = 0; kj < kernel_size; kj++) {
                    result += kernel[ki * kernel_size + kj] * input[(i + ki) * input_size + j + kj];
                }
            }
            *(output + i * output_size + j) = result >= 0 ? result : 0;
        }
    }
}

void multiply(int output_size, float *output, int input_size, float *input, float *weights) {
    for (int i = 0; i < output_size; i++) {
        float result = 0;
        for (int j = 0; j < input_size; j++) {
            result += weights[i + j * output_size] * input[j];
        }
        output[i] = result;
    }

    float sum = 0;
    for (int i = 0; i < output_size; i++) {
        output[i] = expf(output[i]);
        sum += output[i];
    }
    for (int i = 0; i < output_size; i++) {
        output[i] /= sum;
    }
}

void receive_image() {
    printf("\n=============== DIGIT RECOGNITION ===============\n");
    printf("Enter the image:\n");
    for (int i = 0; i < IMAGE_SIZE; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            scanf("%f", &image[i][j]);
        }
    }
}

const int LAYER1_SIZE = IMAGE_SIZE - KERNEL_SIZE_1 + 1;
float layer1_i[IMAGE_SIZE][IMAGE_SIZE];
float layer1_o[LAYER1_SIZE][LAYER1_SIZE];

const int LAYER2_SIZE = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 + 2;
float layer2_i[LAYER1_SIZE][LAYER1_SIZE];
float layer2_o[LAYER2_SIZE][LAYER2_SIZE];

const int LAYER3_SIZE = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 - KERNEL_SIZE_3 + 3;
float layer3_i[LAYER2_SIZE][LAYER2_SIZE];
float layer3_o[LAYER3_SIZE][LAYER3_SIZE];

float output_i[LAYER3_SIZE][LAYER3_SIZE];
float output[OUTPUT_SIZE];

bool layer0_o_ready = false;

bool layer1_i_ready = true;
bool layer1_o_ready = false;

bool layer2_i_ready = false;
bool layer2_o_ready = false;

bool layer3_i_ready = false;
bool layer3_o_ready = false;

bool output_i_ready = false;
bool output_ready = true;

const int N = 1000;
bool done = false;

void layer0() {
    for (int i = 0; i < N; i++) {
        printf("Image %d\n", i);

        layer0_o_ready = true;
        while (layer0_o_ready && !layer1_i_ready) {
            debug_printf("Waiting for layer0_o_ready && !layer1_i_ready\n");
        }
    }
}

void layer1() {
    while (!done) {
        while (!layer0_o_ready) {
            debug_printf("Waiting for !layer0_o_ready\n");
        }
        layer0_o_ready = false;

        debug_printf("Layer 1\n");
        memcpy(&layer1_i[0][0], &image[0][0], sizeof(layer1_i));
        layer1_i_ready = true;

        convolution(LAYER1_SIZE, &layer1_o[0][0], IMAGE_SIZE, &layer1_i[0][0], KERNEL_SIZE_1,
                    &first_convolution[0][0]);

        layer1_o_ready = true;
        while (layer1_o_ready && !layer2_i_ready) {
            debug_printf("Waiting for layer1_o_ready && !layer2_i_ready\n");
        }
    }
}

void layer2() {
    while (!done) {
        while (!layer1_o_ready) {
            debug_printf("Waiting for !layer1_o_ready\n");
        }
        layer1_o_ready = false;

        debug_printf("Layer 2\n");
        memcpy(&layer2_i[0][0], &layer1_o[0][0], sizeof(layer2_i));
        layer2_i_ready = true;

        convolution(LAYER2_SIZE, &layer2_o[0][0], LAYER1_SIZE, &layer2_i[0][0], KERNEL_SIZE_2,
                    &second_convolution[0][0]);

        layer2_o_ready = true;
        while (layer2_o_ready && !layer3_i_ready) {
            debug_printf("Waiting for layer2_o_ready && !layer3_i_ready\n");
        }
    }
}

void layer3() {
    while (!done) {
        while (!layer2_o_ready) {
            debug_printf("Waiting for !layer2_o_ready\n");
        }
        layer2_o_ready = false;

        debug_printf("Layer 3\n");
        memcpy(&layer3_i[0][0], &layer2_o[0][0], sizeof(layer3_i));
        layer3_i_ready = true;

        convolution(LAYER3_SIZE, &layer3_o[0][0], LAYER2_SIZE, &layer3_i[0][0], KERNEL_SIZE_3,
                    &third_convolution[0][0]);

        layer3_o_ready = true;
        while (layer3_o_ready && !output_i_ready) {
            debug_printf("Waiting for layer3_o_ready && !output_i_ready\n");
        }
    }
}

int total_correct = 0;
int total_wrong = 0;

void output_layer() {
    while (!done) {
        while (!layer3_o_ready) {
            debug_printf("Waiting for !layer3_o_ready\n");
        }
        layer3_o_ready = false;

        debug_printf("Output layer\n");
        memcpy(&output_i[0][0], &layer3_o[0][0], sizeof(output_i));
        output_i_ready = true;

        multiply(OUTPUT_SIZE, &output[0], FLAT_SIZE, &layer3_o[0][0], &fully_connected[0][0]);
        output_ready = true;

        debug_printf("\nPrediction results: ");
        float max = output[0];
        int prediction = 0;
        for (int i = 1; i < OUTPUT_SIZE; i++) {
            debug_printf("%f ", output[i]);
            if (output[i] > max) {
                max = output[i];
                prediction = i;
            }
        }

        debug_printf("\nThe image is predicted to be the number: %d\n", prediction);
        if (prediction == 7) {
            total_correct++;
        } else {
            total_wrong++;
        }

        printf("Right: %d\tWrong: %d\tTotal: %d\n", total_correct, total_wrong, total_correct + total_wrong);
    }
}

int main() {
#include "/home/allan/Documents/USP/Disciplinas/SE/SEProjetoFinalSingle/software/CPU0/cnn/first_convolution.txt"
#include "/home/allan/Documents/USP/Disciplinas/SE/SEProjetoFinalSingle/software/CPU0/cnn/fully_connected.txt"
#include "/home/allan/Documents/USP/Disciplinas/SE/SEProjetoFinalSingle/software/CPU0/cnn/second_convolution.txt"
#include "/home/allan/Documents/USP/Disciplinas/SE/SEProjetoFinalSingle/software/CPU0/cnn/third_convolution.txt"
#include <string.h>

    receive_image();
    printf("Image received\n");

    std::thread t1(layer1);
    std::thread t2(layer2);
    std::thread t3(layer3);
    std::thread t4(output_layer);
    std::thread t5(layer0);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    printf("\nTotal correct: %d\n", total_correct);
    printf("Total wrong: %d\n", total_wrong);
    if (total_correct + total_wrong != N) {
        printf("\nError: %d\n", total_correct + total_wrong);
    }
}