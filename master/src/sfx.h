#ifndef sfx_h
#define sfx_h

#include <Arduino.h>
#include <Audio.h>

#define SFX_Self_Destruction_In 1

class Sfx {

	static Sfx* self;
	Audio audio;
	byte queue[16];
	byte queueHead;
	byte queueTail;
	bool playing;

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
	 */
	void enqueue(byte sound);

	/**
	 * Announce "Self destruction in X min(s)"
	 */
	void selfDesctructionIn(byte mins);

	/**
	 * Announce "Detonation in 10, 9, 8, 7, 6, 5, 4, 3, 2, 1"
	 */
	void detonation10sCountdown();

	/**
	 * Hack to hook into Audio.h's callback mechanism
	 * Should be called once a single sound has finished playing
	 */
	static void playbackFinishedHandler();

	/**
	 * Must be regularly called to maintain audio playback
	 */
	void loop();
};

#endif