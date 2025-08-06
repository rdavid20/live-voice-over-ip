#include "wav.h"
#include <stdint.h>
#include <stdio.h>

void write_wav_header(FILE *file, int sample_rate, int bits_per_sample, int channels, int data_size) {
  int byte_rate = sample_rate * channels * bits_per_sample / 8;
  int block_align = channels * bits_per_sample / 8;

  fwrite("RIFF", 1, 4, file);
  int chunk_size = 36 + data_size;
  fwrite(&chunk_size, 4, 1, file);
  fwrite("WAVE", 1, 4, file);

  fwrite("fmt ", 1, 4, file);
  int subchunk1_size = 16;
  short audio_format = 1;
  fwrite(&subchunk1_size, 4, 1, file);
  fwrite(&audio_format, 2, 1, file);
  fwrite(&channels, 2, 1, file);
  fwrite(&sample_rate, 4, 1, file);
  fwrite(&byte_rate, 4, 1, file);
  fwrite(&block_align, 2, 1, file);
  fwrite(&bits_per_sample, 2, 1, file);

  fwrite("data", 1, 4, file);
  fwrite(&data_size, 4, 1, file);

}

void float_to_int16(const float *input, int16_t *output, unsigned long frames) {
  for (unsigned long i = 0; i < frames; ++i) {
    float sample = input[i];
    if (sample > 1.0f) {
      sample = 1.0f;
    } else if (sample < -1.0f)  {
      sample = -1.0f;
    }
    output[i] = (int16_t)(sample * 32767.0f);
  }
}
