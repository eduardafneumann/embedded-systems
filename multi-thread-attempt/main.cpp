#include <thread>
#include <mutex>
#include <math.h>
#include <stdio.h>

void copy_layer(std::mutex *layer_pre_mut, std::mutex *layer_pos_mut,
                bool *layer_pre_is_processed, bool *layer_pos_is_copied,
                int layer_size, float *layer_pos, float *layer_pre)
{
    while (true)
    {
        (*layer_pre_mut).lock();
        (*layer_pos_mut).lock();

        if (*layer_pre_is_processed)
        {
            if (!(*layer_pos_is_copied))
            {
                for (int i = 0; i < layer_size; i++)
                {
                    for (int j = 0; j < layer_size; j++)
                    {
                        layer_pre[i * layer_size + j] = layer_pos[i * layer_size + j];
                    }
                }

                *layer_pos_is_copied = true;
                *layer_pre_is_processed = false;
            }
        }
        (*layer_pre_mut).unlock();
        (*layer_pos_mut).unlock();
    }
}

void convolution(std::mutex *input_mut, std::mutex *output_mut,
                 bool *input_is_copied, bool *output_is_processed,
                 int output_size, float *output,
                 int input_size, float *input,
                 int kernel_size, float *kernel)
{
    while (true)
    {
        (*input_mut).lock();
        (*output_mut).lock();
        if (!(*output_is_processed) && *input_is_copied)
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
            *output_is_processed = true;
            *input_is_copied = false;
        }
        (*input_mut).unlock();
        (*output_mut).unlock();
    }
}

void multiply(std::mutex *input_mut, std::mutex *output_mut,
              bool *input_is_copied, bool *output_is_processed,
              int output_size, float *output,
              int input_size, float *input,
              float *weights)
{
    while (true)
    {
        (*input_mut).lock();
        (*output_mut).lock();

        if (*input_is_copied)
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

            *output_is_processed = true;
            *input_is_copied = false;

            float max = output[0];
            int prediction = 0;
            for (int i = 1; i < output_size; i++)
            {
                printf("%f ", output[i]);
                if (output[i] > max)
                {
                    max = output[i];
                    prediction = i;
                }
            }
            printf("\n%d\n", prediction);
        }

        (*input_mut).unlock();
        (*output_mut).unlock();
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
    float layer1_output[layer1_size][layer1_size];
    float layer1_input[layer1_size][layer1_size];

    int layer2_size = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 + 2;
    float layer2_output[layer2_size][layer2_size];
    float layer2_input[layer2_size][layer2_size];

    int layer3_size = IMAGE_SIZE - KERNEL_SIZE_1 - KERNEL_SIZE_2 - KERNEL_SIZE_3 + 3;
    float layer3_output[layer3_size][layer3_size];
    float layer3_input[layer3_size][layer3_size];

    int flat_size = layer3_size * layer3_size;
    float flat_output[flat_size];
    float flat_input[flat_size];

    float output[OUTPUT_SIZE];

    bool image_is_ready = true, l1_copied = false, l2_copied = false, l3_copied = false;
    bool l1_processed = false, l2_processed = false, l3_processed = false, last_layer_processed = false;

    std::mutex image_mut, l1_output_mut, l1_input_mut, l2_output_mut, l2_input_mut, l3_output_mut, l3_input_mut, last_layer_mut;
    std::mutex results_count_mut;

    std::thread t_convolution1(convolution,
                               &image_mut, &l1_output_mut,
                               &image_is_ready, &l1_processed,
                               layer1_size, &layer1_output[0][0],
                               IMAGE_SIZE, &image[0][0],
                               KERNEL_SIZE_1, &first_convolution[0][0]);

    std::thread t_copy1(copy_layer,
                        &l1_output_mut, &l1_input_mut,
                        &l1_processed, &l1_copied,
                        layer1_size, &layer1_output[0][0], &layer1_input[0][0]);

    std::thread t_convolution2(convolution,
                               &l1_input_mut, &l2_output_mut,
                               &l1_copied, &l2_processed,
                               layer2_size, &layer2_output[0][0],
                               layer1_size, &layer1_input[0][0],
                               KERNEL_SIZE_2, &second_convolution[0][0]);

    std::thread t_copy2(copy_layer,
                        &l2_output_mut, &l2_input_mut,
                        &l2_processed, &l2_copied,
                        layer2_size, &layer2_output[0][0], &layer2_input[0][0]);

    std::thread t_convolution3(convolution,
                               &l2_input_mut, &l3_output_mut,
                               &l2_copied, &l3_processed,
                               layer3_size, &layer3_output[0][0],
                               layer2_size, &layer2_input[0][0],
                               KERNEL_SIZE_3, &third_convolution[0][0]);

    std::thread t_copy3(copy_layer,
                        &l3_output_mut, &l3_input_mut,
                        &l3_processed, &l3_copied,
                        layer3_size, &layer3_output[0][0], &layer3_input[0][0]);

    std::thread t_multiply(multiply,
                           &l3_input_mut, &last_layer_mut,
                           &l3_copied, &last_layer_processed,
                           OUTPUT_SIZE, &output[0],
                           flat_size, &layer3_input[0][0],
                           &fully_connected[0][0]);

    while (true)
    {
        image_mut.lock();
        if (l1_processed && !image_is_ready)
        {

            image_is_ready = true;
        }
        image_mut.unlock();
    }

    t_convolution1.join();
    return 0;
}
