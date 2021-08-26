#ifndef sfx_h
#define sfx_h

#include <Arduino.h>
#include "../lib/Audio/AudioFileSourceSPIFFS.h"
#include "../lib/Audio/AudioGeneratorMP3.h"
#include "../lib/Audio/AudioOutputI2S.h"

#define SFX_ENQUEUE_MODE__DEFAULT 0
#define SFX_ENQUEUE_MODE__PLAYNEXT 1
#define SFX_ENQUEUE_MODE__INTERRUPT 2

class Sfx {

	byte queue[16];
	byte queueHead;
	byte queueTail;
	bool playing;

	AudioGeneratorMP3 *mp3;
	AudioFileSourceSPIFFS *file;
	AudioOutputI2S *out;

	/**
	 * Plays whatever is next in the queue
	 */
	void playQueue();
	
	/**
	 * Should be called once a single sound has finished playing
	 */
	void playbackFinished();

public:
	Sfx(uint8_t bclk, uint8_t lrc, uint8_t dout);

	/** 
	 * Enqueues (and plays) the given sound
	 * Use mode to control queuing behaviour
	 */
	void enqueue(byte sound, byte mode = 0);

	/**
	 * Announce "Self destruction in X min(s)"
	 */
	void selfDesctructionIn(byte mins);

	/**
	 * Announce "Detonation in 10, 9, 8, 7, 6, 5, 4, 3, 2, 1"
	 */
	void detonation10sCountdown();

	/**
	 * Must be regularly called to maintain audio playback
	 */
	void loop();
};

#endif