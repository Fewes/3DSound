#include "stdafx.h"
#include "SoundNode.h"



SoundNode::SoundNode()
{
	this->setSound(440, 10);
	this->setPos(vector<double> (1,3));
}

SoundNode::SoundNode(string filname, vector<double> pos)
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

void SoundNode::setPos(vector<double> pos)
{
	position = pos;
}

void SoundNode::setPos(double x, double y, double z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

bool SoundNode::setSound(string filname)
{
	AudioStream audio(filname); //Trycatch?
	sr = audio.getSampleRate();
	if (audio.getNChannels() == 2) {
		leftEar << audio[0];
		rightEar << audio[1];
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
	int elevation = -40; //Hardcoded evelation (level zero)
	Buffer Lb("HRTFdata/L" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for left channel at the right ang and ev
	Buffer Rb("HRTFdata/R" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for right channel at the right ang and ev
	leftEar.convolve(Lb); //Make a convolution on leftaudio with leftchannel hrft;
	rightEar.convolve(Rb); //Make a convolution on rightaudio with rightchannel hrft; 
}


