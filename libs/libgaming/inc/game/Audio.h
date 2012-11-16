#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>
#include <stdbool.h>

/*! @addtogroup libgaming
 * @{ */

/*! @addtogroup Audio
 * @brief This header is used to define function for simple audio output.
 * @{ */

/*! @defgroup Gaming_AudioFreq
 * @{ */

#define Gaming_AudioFreq_192k   ((uint32_t)192000)
#define Gaming_AudioFreq_96k    ((uint32_t)96000)
#define Gaming_AudioFreq_48k    ((uint32_t)48000)
#define Gaming_AudioFreq_44k    ((uint32_t)44100)
#define Gaming_AudioFreq_32k    ((uint32_t)32000)
#define Gaming_AudioFreq_22k    ((uint32_t)22050)
#define Gaming_AudioFreq_16k    ((uint32_t)16000)
#define Gaming_AudioFreq_11k    ((uint32_t)11025)
#define Gaming_AudioFreq_8k     ((uint32_t)8000)

/*! @} */

typedef void AudioCallbackFunction(void *context, int buffer);

/*!
 *	@brief Used to initialize audio
 *	@param freq Defines the playback frequency. This parameter can be a value of @ref Gaming_AudioFreq
 *
 *	This configures and starts the audio codec. It should be possible to output samples to the audio jack with @ref OutputAudioSample after that. In its current implementation, all audio samples must be 16 bit.
 */
void InitializeAudio(uint16_t freq);
void DeinitializeAudio(void);
bool IsAudioInitialized(void);
void SetAudioVolume(int volume);

// Power up and down the audio hardware.
void AudioOn();
void AudioOff();

/*!
 *	@brief Output a sample to the sound codec
 *	@param sample The sample to write
 *
 *	@todo Document sample format, structure, etc.
 */
void OutputAudioSample(int16_t sample);
void OutputAudioSampleWithoutBlocking(int16_t sample);

void PlayAudioWithCallback(AudioCallbackFunction *callback,void *context);
void StopAudio();

void ProvideAudioBuffer(void *samples,int numsamples);
bool ProvideAudioBufferWithoutBlocking(void *samples,int numsamples);

/*! @} */
/*! @} */

#endif
