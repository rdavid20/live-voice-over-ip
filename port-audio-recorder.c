#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "portaudio.h"

#include "utils/wav.h"

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#define SAMPLE_RATE 48000
#define FRAMES_PER_BUFFER 256
#define NUM_SECODNS 5
#define NUM_CHANNELS 1

#define OUTPUT_LATENCY 250.0

typedef struct {
  FILE *file;
  int totalFramesRecorded;
} paUserData;

static int paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
  (void) timeInfo;
  (void) statusFlags;

  paUserData *data = (paUserData*)userData;
  const float *in = (const float*)inputBuffer;
  float *out = (float*)outputBuffer;

  if (inputBuffer == NULL) {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
      out[i] = 0.0f;
    }

    int16_t silence[FRAMES_PER_BUFFER * NUM_CHANNELS] = {0};
    fwrite(silence, sizeof(int16_t), framesPerBuffer * NUM_CHANNELS, data->file);
  } else {
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
      out[i] = in[i];
    }

    int16_t buffer[FRAMES_PER_BUFFER];
    float_to_int16(in, buffer, framesPerBuffer);
    fwrite(buffer, sizeof(int16_t), framesPerBuffer * NUM_CHANNELS, data->file);
  }
  data->totalFramesRecorded += framesPerBuffer;
  return paContinue;
}

int main() {
  PaStream *stream;
  PaError err;
  short buffer[FRAMES_PER_BUFFER];
  paUserData data = {0};

  err = Pa_Initialize();
  if (err != paNoError) {
    perror("portaudio init");
    return 1;
  }

  int numDevices = Pa_GetDeviceCount();
  for (int i = 0; i < numDevices; ++i) {
      const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
      printf("Device %d: %s (Input channels: %d)\n", i, deviceInfo->name, deviceInfo->maxInputChannels);
  }

  PaStreamParameters inputParams;
  inputParams.device = 8;
  inputParams.channelCount = 1;
  inputParams.sampleFormat = paFloat32;
  inputParams.suggestedLatency = Pa_GetDeviceInfo(8)->defaultLowInputLatency;
  inputParams.hostApiSpecificStreamInfo = NULL;

  PaStreamParameters outputParams;
  outputParams.device = 3;
  outputParams.channelCount = 1;
  outputParams.sampleFormat = paFloat32;
  outputParams.suggestedLatency = OUTPUT_LATENCY / 1000;
  outputParams.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&stream,
                    &inputParams,
                    &outputParams,
                    SAMPLE_RATE,
                    FRAMES_PER_BUFFER,
                    paClipOff,
                    paCallback,
                    &data);
  if (err != paNoError) {
    perror("Open stream");
    return 1;
  }

  err = Pa_StartStream(stream);
  if (err != paNoError) {
    perror("Start stream");
    return 1;
  }

  data.file = fopen("output.wav", "wb+");
  write_wav_header(data.file, SAMPLE_RATE, 16, 1, 0);
  long data_start = ftell(data.file);

  printf("Recording... \n");
  Pa_Sleep(5000);

  err = Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  long data_end = ftell(data.file);
  int data_size = data_end - data_start;
  fseek(data.file, 0, SEEK_SET);
  write_wav_header(data.file, SAMPLE_RATE, 16, 1, data_size);
  fclose(data.file);

  return 0;
}
