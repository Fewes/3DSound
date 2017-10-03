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

	//leftEar.mono();
	//rightEar.mono();
	//leftEar.normalize(); //Not sure if we really wants to do that
	//rightEar.normalize();
	return true; //TODO send false if sound isnt loaded
}

void SoundNode::setSound(int freq, double time)
{
	
}

Buffer& SoundNode::getChannel(int channel)
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
/*
void SoundNode::generate3D()
{
	int angle = 90; //Hardcoded angle (to the right)
	int elevation = 0; //Hardcoded evelation (level zero)
	Buffer Lb("HRTFdata/elev0/L" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for left channel at the right ang and ev
	Buffer Rb("HRTFdata/elev0/R" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for right channel at the right ang and ev
	leftEar.convolve(Lb); //Make a convolution on leftaudio with leftchannel hrft;
	rightEar.convolve(Rb); //Make a convolution on rightaudio with rightchannel hrft; 
}
*/
void SoundNode::buildSound()
{

	HRFTl = ("HRTF/L/" + std::to_string(0) + "/" + std::to_string(180) + ".wav");
	HRFTr = ("HRTF/R/" + std::to_string(0) + "/" + std::to_string(180) + ".wav");

	rightEar[0];
	leftEar[0];
	Buffer rightOutbuff = rightEar*0.0; // Alocate the output Buffer will all zeros.
	Buffer leftOutbuff = leftEar*0.0; // Alocate the output Buffer will all zeros.
	int rnumSampels = rightEar.getLength();
	int lnumSampels = leftEar.getLength();

	for (int i = 0; i < rnumSampels; i++) {
		//Här ska allt som ska göras med ljudet göras
		//För att falta
		conv(rightEar[i], rightOutbuff, i, CHL_R, HRFTr);

		//cout << "Before: " << rightOutbuff[i] << endl;
		rightOutbuff[i] = rightOutbuff[i]*getAmpchange();
		//cout << "After: " << rightOutbuff[i] << endl;
	}
	cout << "right done" << endl;

	for (int i = 0; i < lnumSampels; i++) {
		//Här ska allt som ska göras med ljudet göras
		//För att falta
		conv(leftEar[i], leftOutbuff, i, CHL_L, HRFTl);

		leftOutbuff[i] = leftOutbuff[i]*getAmpchange();
	}
	cout << "left done" << endl;

	rightEar = rightOutbuff;
	leftEar = leftOutbuff;
}

void SoundNode::conv(float64 sample, Buffer & outbuffer, int i, int channel, Buffer & HRFT)
{
	for (int j = 0; j < HRFT.getLength() && (j + i) < outbuffer.getLength(); j++) {
		outbuffer[i + j] += sample * HRFT[j];
	}
}

float64 SoundNode::getAmpchange() //not physically correct but works for now
{
	float distance = sqrt(pow(position.x, 2) + pow(position.y, 2) + pow(position.z, 2));
	//float I = 1 / (pow(distance, 2)*PI_ * 4);
	return 1 / pow(distance, 2);//10 * log(I / Io_);
}


