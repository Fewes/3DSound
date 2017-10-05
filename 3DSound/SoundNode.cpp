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

	//cout << "| x:" << speed.x << "| y:" << speed.y << "| z: " << speed.z << endl;
}

void SoundNode::updatePos(int x)
{
	position.x = startPos.x + speed.x * x;
	position.y = startPos.y + speed.y * x;
	position.z = startPos.z + speed.z * x;

	//cout << "| x:" << position.x << "| y:" << position.y << "| z: " << position.z << endl;
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
void SoundNode::buildSound(Vector3f lisnrPos)
{

	HRFTl = ("HRTF/L/" + std::to_string(0) + "/0" + std::to_string(90) + ".wav"); //Temps
	HRFTr = ("HRTF/R/" + std::to_string(0) + "/0" + std::to_string(90) + ".wav"); //Temps
	
	Buffer rightOutbuff = rightEar*0.0; // Alocate the output Buffer will all zeros.
	Buffer leftOutbuff = leftEar*0.0; // Alocate the output Buffer will all zeros.
	int rnumSampels = rightEar.getLength();//Sen ska den l�ngsta tas
	int lnumSampels = leftEar.getLength();

	for (int i = 0; i < rnumSampels; i++) { //Left och right channels ska g�ras samtidigt i framtiden. 
		//H�r ska allt som ska g�ras med ljudet g�ras
		//F�r att falta
		updatePos(i);
		conv(rightEar[i], rightOutbuff, i, CHL_R, HRFTr);
		
		//cout << "Before: " << rightOutbuff[i] << endl;
		//rightOutbuff[i] = rightOutbuff[i] * getAmpchange(lisnrPos);
		rightOutbuff[i] = getAmpchange(lisnrPos, rightOutbuff[i]);
		//cout << "After: " << rightOutbuff[i] << endl;
	}
	cout << "right done" << endl;

	for (int i = 0; i < lnumSampels; i++) {
		//H�r ska allt som ska g�ras med ljudet g�ras
		//F�r att falta
		updatePos(i);
		conv(leftEar[i], leftOutbuff, i, CHL_L, HRFTl);

		//leftOutbuff[i] = leftOutbuff[i] * getAmpchange(lisnrPos);
		leftOutbuff[i] = getAmpchange(lisnrPos, leftOutbuff[i]);
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

float64 SoundNode::getAmpchange(Vector3f lisnrPos, float64 sample) //not physically correct but works for now
{
	float distance = sqrt(pow(position.x - lisnrPos.x, 2) + pow(position.y - lisnrPos.y, 2) + pow(position.z - lisnrPos.z, 2));

	float L2 = 20*log10(sample) - abs((20 * log10(pow((0.1 / distance),2))));
	L2 = pow(10, L2 / 20);
	//float I = 1 / (pow(distance, 2)*PI_ * 4);
	return L2;//10 * log(I / Io_);
}


