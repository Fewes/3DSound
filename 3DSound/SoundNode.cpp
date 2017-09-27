#include "stdafx.h"
#include "SoundNode.h"



SoundNode::SoundNode()
{
	this->setSound(440, 10);
	this->setPos(Vector3f(0,0,0));
}

SoundNode::SoundNode(string filname, Vector3f pos)
{
	this->setSound(filname);
	this->setPos(pos);
}

SoundNode::SoundNode(const SoundNode& SN)
{
	this->leftEar = SN.leftEar;
	this->rightEar = SN.rightEar;
	this->position = SN.position;
	this->sr = SN.sr;
}

void SoundNode::setPos(Vector3f pos)
{
	position = pos;
}

void SoundNode::setPos(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
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

	leftEar.mono();
	rightEar.mono();
	//leftEar.normalize(); //Not sure if we really wants to do that
	//rightEar.normalize();
	return true; //TODO send false if sound isnt loaded
}

void SoundNode::setSound(int freq, double time)
{
	
}

AudioStream& SoundNode::getChannel(int channel)
{
	if (channel == 0) {
		return leftEar;
	}else {
		return rightEar;
	}
}

float64& SoundNode::getSampleRate()
{
	return sr;
}

void SoundNode::generate3D()
{
	int angle = 90; //Hardcoded angle (to the right)
	int elevation = 0; //Hardcoded evelation (level zero)
	Buffer Lb("HRTFdata/elev0/L" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for left channel at the right ang and ev
	Buffer Rb("HRTFdata/elev0/R" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for right channel at the right ang and ev
	leftEar.convolve(Lb); //Make a convolution on leftaudio with leftchannel hrft;
	rightEar.convolve(Rb); //Make a convolution on rightaudio with rightchannel hrft; 
}



void conv(float64 sample, Buffer outbuffer, int i, int channel) {
	/*Faltningsfunktionen
	ladda falt fil blalbabla
	*/
	Buffer HRFT;
	if (channel == 0) {
		HRFT = ("HRTFdata/L" + std::to_string(90) + "e0" + std::to_string(-40) + "a.wav");
	}else {
		HRFT = ("HRTFdata/R" + std::to_string(90) + "e0" + std::to_string(-40) + "a.wav");
	}
	
	for (int j = 0; j < HRFT.getLength() && (j + i) < outbuffer.getLength(); j++) {
		outbuffer[i + j] += sample * HRFT[j];
	}
}

void SoundNode::buildSound()
{
	Buffer rightbuffer = rightEar[0];
	Buffer leftbuffer = leftEar[0];
	Buffer rightOutbuff = rightbuffer*0.0; // Alocate the output Buffer will all zeros.
	Buffer leftOutbuff = leftbuffer*0.0; // Alocate the output Buffer will all zeros.
	int rnumSampels = rightbuffer.getLength();
	int lnumSampels = leftbuffer.getLength();

	for (int i = 0; i < rnumSampels; i++) {
		//Här ska allt som ska göras med ljudet göras
		//För att falta
		conv(rightbuffer[i], rightOutbuff, i, CHL_R); //rightoutbuff ska skickas som reference
	}
	for (int i = 0; i < lnumSampels; i++) {
		//Här ska allt som ska göras med ljudet göras
		//För att falta
		conv(leftbuffer[i], leftOutbuff, i, CHL_L); //leftoutbuff ska skickas som reference
	}
}


