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

	memset(this->queue, 0, sizeof(this->queue));
	this->queueHead = 0;
	this->queueTail = 0;

	this->playing = false;

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
		}
	}
}

void Sfx::enqueue(byte sound, byte mode)
{
	// if we are playing next or interrupting
	if(mode != SFX_ENQUEUE_MODE__DEFAULT)
	{
		this->queueTail = this->queueHead;
		// wipe the whole queue, doesnt matter that we're wiping the head as its already playing
		memset(this->queue, 0, sizeof(this->queue));
	}
	
	if(mode == SFX_ENQUEUE_MODE__INTERRUPT)
	{
		if(this->mp3->isRunning())
		{
			this->playing = false;
			this->mp3->stop();
		}
	}

	// TODO: this will misbehave is you try to enqueue more than the queue size, what should happen when you queue the 17th sound?
	this->queueTail++;
	if(this->queueTail == sizeof(this->queue))
		this->queueTail = 0;

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
		if(this->queueHead == sizeof(this->queue))
			this->queueHead = 0;
	}

	byte queueValue = this->queue[this->queueHead];
	if(queueValue == 0)
		return;

	String path = "/";
	path += queueValue;
	path += ".mp3";

	Serial.print("Playing: ");
	Serial.println(path);

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
	delete this->file;
	this->queue[this->queueHead] = 0;
	this->playQueue();
}

void Sfx::selfDesctructionIn(byte mins)
{
	Serial.print("Playing: Self destruction in ");
	Serial.print(mins);
	Serial.println(" mins(s)");

	this->enqueue(Sounds::SelfDestructionIn);
	
	byte sound = 0;

	// This doesnt support 11-19, but thats ok because we should never be doing that

	byte ones = mins % 10;
	byte tens = mins / 10;

	if(tens > 0)
	{
		sound = 100 + (tens * 10);
		this->enqueue(sound);
	}

	if(ones > 0)
	{
		sound = 100 + ones;
		this->enqueue(sound);
	}

	this->enqueue(mins == 1 ? Sounds::Minute : Sounds::Minutes);
}

void Sfx::detonation10sCountdown()
{
	Serial.println("Playing: Detonation Countdown");
	this->enqueue(Sounds::DetonationCountdown);
}