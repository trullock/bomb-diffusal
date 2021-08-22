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

	Serial.print("Playing: ");
	Serial.println(path);

	this->audio.connecttoFS(SPIFFS, path.c_str());
	this->playing = true;
}

void Sfx::playbackFinished()
{
	self->playing = false;
	self->queue[self->queueHead] = 0;
	self->playQueue();
}

void audio_eof_mp3(const char *info)
{
	Sfx::playbackFinished();
}


void Sfx::selfDesctructionIn(byte mins)
{
	// only announce 10,20,30,etc and 1,2,3,4,5,6,7,8,9
	if(mins % 10 != 0 && mins > 10)
		return;

	this->enqueue(Sounds::SelfDestructionIn);
	
	byte sound = 100 + mins;
	this->enqueue(sound);

	//this->enqueue(mins == 1 ? Sounds::Min : Sounds::Mins);
}

void Sfx::detonation10sCountdown()
{
	this->enqueue(Sounds::DetonationCountdown);
}