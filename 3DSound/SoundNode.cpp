#include "stdafx.h"
#include "SoundNode.h"



SoundNode::SoundNode()
{
	this->setSound(440, 10);
	this->setPos(Vector3f(0,0,0));
	this->setEndPos(Vector3f(0, 0, 0));
}

SoundNode::SoundNode(string filname, Vector3f pos)
{
	this->setSound(filname);
	this->setPos(pos);
	this->setEndPos(pos);

}

SoundNode::SoundNode(const SoundNode& SN)
{
	this->leftEar = SN.leftEar;
	this->rightEar = SN.rightEar;
	this->startPos = SN.startPos;
	this->endPos = SN.endPos;
	this->sr = SN.sr;
}

void SoundNode::setPos(Vector3f pos)
{
	if (startPos == endPos) {
		setEndPos(pos);
	}
	startPos = pos;
	setSpeed();
}

void SoundNode::setPos(float x, float y, float z)
{
	if (startPos == endPos) {
		setEndPos(x, y, z);
	}
	startPos.x = x;
	startPos.y = y;
	startPos.z = z;
	setSpeed();
}

void SoundNode::setEndPos(Vector3f pos)
{
	endPos = pos;
	setSpeed();
}

void SoundNode::setEndPos(float x, float y, float z) {
	endPos.x = x;
	endPos.y = y;
	endPos.z = z;
	setSpeed();
}

void SoundNode::setSpeed() {
	speed.x = (endPos.x - startPos.x) / rightEar.getLength();
	speed.y = (endPos.y - startPos.y) / rightEar.getLength();
	speed.z = (endPos.z - startPos.z) / rightEar.getLength();

}

void SoundNode::updatePos()
{
	position += speed;

}

bool SoundNode::setSound(string filname)
{
	AudioStream audio(filname); //Trycatch?
	sr = audio.getSampleRate();
	if (audio.getNChannels() == 2) {
		leftEar << audio[CHL_L];
		rightEar << audio[CHL_R];
	}else {
		audio.mono();
		leftEar << audio;
		rightEar << audio;
	}

	return true; //TODO send false if sound isnt loaded
}

void SoundNode::setSound(int freq, double time)
{
	//impl freq wave
}

Buffer& SoundNode::getChannel(int channel)
{
	if (channel == CHL_L) {
		return leftEar;
	}else {
		return rightEar;
	}
}

float64& SoundNode::getSampleRate()
{
	return sr;
}


void SoundNode::buildSound(Vector3f* lisnrPos, Vector3f* listenerDir, HRTFCache* hrtfCache)
{

	Buffer rightOutbuff = rightEar*0.0; // Alocate the output Buffer will all zeros.
	Buffer leftOutbuff = leftEar*0.0; // Alocate the output Buffer will all zeros.
	int rnumSampels = rightEar.getLength();//Sen ska den kortaste tas
	int lnumSampels = leftEar.getLength();
	position = startPos;

	for (int i = 0; i < rnumSampels; i++) {
		//Här ska allt som ska göras med ljudet göras
		//För att falta
		updatePos();
		conv(rightEar[i], rightOutbuff, i, 0, hrtfCache->GetHRTF(*lisnrPos, *listenerDir, position, Right));
		conv(leftEar[i], leftOutbuff, i, 1, hrtfCache->GetHRTF(*lisnrPos, *listenerDir, position, Left));

		//rightOutbuff[i] = rightOutbuff[i] * getAmpchange(lisnrPos);
		//rightOutbuff[i] = getAmpchange(lisnrPos, rightOutbuff[i]);
		//leftOutbuff[i] = leftOutbuff[i] * getAmpchange(lisnrPos);
		//leftOutbuff[i] = getAmpchange(lisnrPos, leftOutbuff[i]);	
		//cout << "Before: " << rightOutbuff[i] << endl;
		rightOutbuff[i] = getAmpchange(lisnrPos, rightOutbuff[i]);
		//cout << "After: " << rightOutbuff[i] << endl;
		leftOutbuff[i] = getAmpchange(lisnrPos, leftOutbuff[i]);	
	}
	cout << "done" << endl;
	rightEar = rightOutbuff;
	leftEar = leftOutbuff;
}

void SoundNode::conv(float64 sample, Buffer & outbuffer, int i, int channel, Buffer& HRFT)
{
	for (int j = 0; j < HRFT.getLength() && (j + i) < outbuffer.getLength(); j++) {
		outbuffer[i + j] += sample * HRFT[j];
	}
	
}

float64 SoundNode::getAmpchange(Vector3f* lisnrPos, float64 sample) //not physically correct but works for now
{
	float64 distance = sqrt(pow(position.x - lisnrPos->x, 2) + pow(position.y - lisnrPos->y, 2) + pow(position.z - lisnrPos->z, 2));
	float64 a1 = 1.0f;
	float64 refdist = 0.1;

	/*
	float L2 = 20*log10(sample) - abs((20 * log10(pow((0.1 / distance),2))));
	L2 = pow(10, L2 / 20);
	//float I = 1 / (pow(distance, 2)*PI_ * 4);
	//10 * log(I / Io_);
	

	

	if (distance >= 1) {
		sample *= (1 / distance);
	}
	else {

	}*/

	//cout << "first: " << sample;
	//sample += 1;
	bool isNeg = (sample < 0);
	sample = abs(sample);

	//cout << " after +1: " << sample;
	//sample = pow(10, (20 * log10(sample) - 20 * log10(0.1 / distance)) / 20);
	sample = 20 * log10(sample/a1);
	//cout << " after dB: " << sample;
	sample -= abs(20 * log10(refdist / max(refdist, distance)));
	//cout << " after negative dist: " << sample;
	sample = a1 * pow(10, sample / 20);
	//cout << " after dB^-1: " << sample << endl;
	
	//om mindre än 0 sätt till noll
	if (isNeg) {
		sample *= -1;
	}
	//cout << " result: " << sample << endl;

	

	return sample;
}


