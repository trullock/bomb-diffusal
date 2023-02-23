#include "sfx.h"
#include <sounds.h>
#include <SPIFFS.h>
#include "pins.h"

Sfx::Sfx()
{
	this->out = new AudioOutputI2S();
	out->SetChannels(1);
	out->SetOutputModeMono(true);
	out->SetGain(0.5);
	out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
	
	this->mp3 = new AudioGeneratorMP3();

	for(uint8_t i = 0; i < QUEUE_SIZE; i++)
		this->queue[i] = NULL;
	this->queueHead = 0;
	this->queueTail = 0;

	this->playing = false;
	this->file = 0;

	if(!SPIFFS.begin(false)){
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
}

void Sfx::loop()
{	
	if (this->mp3->isRunning()) {
    	if (!this->mp3->loop()) 
		{
			this->mp3->stop();
			this->playing = false;
			this->playbackFinished();
			this->playQueue();
		}
	}
}

void Sfx::enqueue(ISound* sound)
{
	if(sound->mode == SFX_ENQUEUE_MODE__INTERRUPT)
	{
		// wipe whole queue, do this before playbackFinished() so we dont trigger any callbacks for whats playing
		this->queueTail = this->queueHead;
		for(uint8_t i = 0; i < QUEUE_SIZE; i++)
		{
			if(this->queue[i] == NULL)
				continue;
				
			delete this->queue[i];
			this->queue[i] = NULL;
		}

		if(this->mp3->isRunning())
		{
			this->playing = false;
			this->mp3->stop();
			this->playbackFinished();
		}
	}


	// TODO: this will misbehave if you try to enqueue more than the queue size, what should happen when you queue the 17th sound?
	this->queueTail++;
	if(this->queueTail == QUEUE_SIZE)
		this->queueTail = 0;

	Serial.print("Enqueuing ");
	Serial.print(sound->sound);
	Serial.print(" at queue position: ");
	Serial.println(this->queueTail);
	this->queue[this->queueTail] = sound;
	this->playQueue();
}

void Sfx::playQueue()
{
	if(this->playing)
		return;
		
	// if we have something to play
	if(this->queueHead != this->queueTail)
	{
		this->queueHead++;
		if(this->queueHead == QUEUE_SIZE)
			this->queueHead = 0;
	}

	ISound* sound = this->queue[this->queueHead];
	if(sound == NULL)
		return;

	String path = "/";
	path += sound->sound;
	path += ".mp3";

	Serial.print("Playing: ");
	Serial.println(path);

	if(this->file)
		delete this->file;
	this->file = new AudioFileSourceSPIFFS(path.c_str());

	if(!mp3->begin(file, out))
	{			
		Serial.print("Failed to play: ");
		Serial.println(path);
		return;
	}

	this->playing = true;
}

void Sfx::playbackFinished()
{
	this->playing = false;
	if(this->file)
	{
		delete this->file;
		this->file = NULL;
	}

	if(this->queue[this->queueHead] != NULL)
	{
		// invoke playback-finished callback
		(*this->queue[this->queueHead])();
		delete this->queue[this->queueHead];
		this->queue[this->queueHead] = NULL;
	}
}
