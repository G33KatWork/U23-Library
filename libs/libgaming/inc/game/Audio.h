#ifndef __AUDIO_H__
#define __AUDIO_H__

typedef void AudioCallbackFunction(void *context,int buffer);

void InitializeAudio(void);
void DeinitializeAudio(void);
_Bool IsAudioInitialized(void);
void SetAudioVolume(int volume);
void OutputAudioSample(int16_t sample);
#endif
