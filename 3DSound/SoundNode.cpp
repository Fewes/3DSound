#include "stdafx.h"
#include "SoundNode.h"
#include <experimental/filesystem> 
#include <string>



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
void SoundNode::calcAngleElev() {
	this->angle = round(atan(asin(position[0]) / acos(position[1])));
	this->elevation = round(atan(asin(position[2]) / acos(angle)));
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
	int counter;
	bool htrfa, htrfe, cor_a, cor_e;
	htrfe = false;
	htrfa = false;
	cor_a = false;
	cor_e = false;
	string fixedangle;
	string location = "../3DSound/HRTFdata/elev";


	//people are stoopid
	while (!cor_a) {
		if (angle < 360 && angle >= 0) {
			cor_a = true;
		}
		else {
			cout << "Please! Enter an angle between 0 and 359: ";
		}


	}
	while (!cor_e) {
		if (elevation >= -40 && elevation <= 90) {
			cor_e = true;
		}
		else {
			cout << "Please! Enter an elevation between -40 and 90: ";
		}

	}
	//if (angle < 10) {
	//	fixedangle = "" + to_string(angle);
	//}
	//else if (angle < 100) {
	//	fixedangle = "" + to_string(angle);
	//}
	//else {
		fixedangle = to_string(angle);
	//}


	//find elevation folder
	while (!htrfe) {
		if (std::experimental::filesystem::exists(location + to_string(elevation))) {
			cout << "elevation is: " << elevation << endl;
			htrfe = true;
		}
		else {
			cout << "elevation is: " << elevation << endl;
			elevation++;
		}
	}
	//find angle in folder
	counter = 0;
	while (!htrfa) {
		if (std::experimental::filesystem::exists(location + std::to_string(elevation) + "/L" + std::to_string(elevation) + "e" + fixedangle + "a.wav")) {
			//cout << "angle is: " << fixedangle << endl;
			htrfa = true;
		}
		else {
			//cout << "angle is: " << fixedangle << endl;
			angle++;
			counter++;
			if (counter >= 100) {
				angle = 0;
			}

			if (angle < 10) {
				fixedangle = "00" + to_string(angle);
			}
			else if (angle < 100) {
				fixedangle = "0" + to_string(angle);
			}
			else {
				fixedangle = to_string(angle);
			}


		}
		if (fixedangle == "010") {
			break;
		}
	}

	Buffer Lb(location + "L" + std::to_string(elevation) + "e0" + fixedangle + "a.wav"); //Load hrftdata for left channel at the right ang and ev
	Buffer Rb(location + "R" + std::to_string(elevation) + "e0" + fixedangle + "a.wav"); //Load hrftdata for right channel at the right ang and ev
	leftEar.convolve(Lb); //Make a convolution on leftaudio with leftchannel hrft;
	rightEar.convolve(Rb); //Make a convolution on rightaudio with rightchannel hrft; 
}


