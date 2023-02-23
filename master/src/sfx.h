#ifndef sfx_h
#define sfx_h

#include <Arduino.h>
#include "../lib/Audio/AudioFileSourceSPIFFS.h"
#include "../lib/Audio/AudioGeneratorMP3.h"
#include "../lib/Audio/AudioOutputI2S.h"

#define SFX_ENQUEUE_MODE__DEFAULT 0
#define SFX_ENQUEUE_MODE__INTERRUPT 1


class ISound
{
public:
	byte sound;
	byte mode;
	virtual void operator() () = 0;
	virtual ~ISound() {}
};
 
class Sound : public ISound
{
public:
	
	Sound(byte sound, byte mode = SFX_ENQUEUE_MODE__DEFAULT)
	{
		this->sound = sound;
		this->mode = mode;
	}

	virtual void operator () () override 
	{
	}
};

template<typename T>
class SoundWithCallback : public ISound
{
	T* instance = 0;
	void (*function)(T*);
public:
	
	SoundWithCallback(byte sound, byte mode, T* instance, void (*function)(T*))
		: 	instance(instance), 
			function(function)
	{
		this->sound = sound;
		this->mode = mode;
	}

	virtual void operator () () override 
	{ 
		(*function)(instance); 
	}
};


class Sfx {

	#define QUEUE_SIZE 16
	ISound* queue[QUEUE_SIZE];
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
	Sfx();

	/** 
	 * Enqueues (and plays) the given sound
	 * Use mode to control queuing behaviour
	 */
	void enqueue(ISound* sound);

	/**
	 * Must be regularly called to maintain audio playback
	 */
	void loop();
};

#endif