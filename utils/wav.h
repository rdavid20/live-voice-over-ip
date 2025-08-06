#ifndef WAV_H
#define WAV_H

#include <stdio.h>
#include <stdint.h>

void write_wav_header(FILE *file, int sample_rate, int bits_per_sample, int channels, int data_size);
void float_to_int16(const float *input, int16_t *output, unsigned long frames);

#endif
