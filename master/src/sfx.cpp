#include "sfx.h"
#include "../lib/sounds.h"
#include <SPIFFS.h>
// #include "WiFi.h"

Sfx* Sfx::self;

Sfx::Sfx(uint8_t bclk, uint8_t lrc, uint8_t dout)
{
	audio.setPinout(bclk, lrc, dout);
    audio.setVolume(10); // 0...21

	memset(this->queue, 0, sizeof(this->queue));
	this->queueHead = 0;
	this->queueTail = 0;

	this->playing = false;

	self = this;

	if(!SPIFFS.begin(false)){
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
}

void Sfx::loop()
{	
	this->audio.loop();
}

void Sfx::enqueue(byte sound)
{
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

	this->queueHead++;
	if(this->queueHead == sizeof(this->queue))
		this->queueHead = 0;

	byte queueValue = this->queue[this->queueHead];
	if(queueValue == 0)
		return;

	String path = "";
	path += queueValue;
	path += ".mp3";

	if(!this->audio.connecttoFS(SPIFFS, path.c_str()))
	{			
		Serial.print("Failed to play: ");
		Serial.println(path);
	}

	this->playing = true;
}

void Sfx::playbackFinished()
{
	this->playing = false;
	this->queue[this->queueHead] = 0;
	this->playQueue();
}


void Sfx::playbackFinishedHandler()
{
	self->playbackFinished();
}
void audio_eof_mp3(const char *info)
{
	Sfx::playbackFinishedHandler();
}


void Sfx::selfDesctructionIn(byte mins)
{
	Serial.print("Playing: Self destruction in ");
	Serial.print(mins);
	Serial.println("mins(s)");

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
		this->enqueue(ones);
	}

	this->enqueue(mins == 1 ? Sounds::Minute : Sounds::Minutes);
}

void Sfx::detonation10sCountdown()
{
	this->enqueue(Sounds::DetonationCountdown);
}