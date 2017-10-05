#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>
#include <math.h>  
#include "HRTFCache.h"

#define CHL_L 0
#define CHL_R 1
#define PI_ 3.1415
#define Io_ pow(10, -12)

using namespace std;
using namespace Nsound;
using namespace sf;

class SoundNode
{
public:
	SoundNode();
	SoundNode(string filname, Vector3f pos);
	SoundNode(const SoundNode& SN);
	void setPos(Vector3f pos);
	void setPos(float x, float y, float z);
	void setEndPos(Vector3f pos);
	void setEndPos(float x, float y, float z);
	bool setSound(string filname/*, string filetype*/);
	void setSound(int freq, double time);
	Buffer& getChannel(int channel);
	float64& getSampleRate();
	void buildSound(Vector3f lisnrPos, Vector3f listenerDir, HRTFCache* hrtfCache);
	
private:
	Vector3f position;
	Vector3f startPos;
	Vector3f endPos;
	Vector3f speed;
	Buffer leftEar;
	Buffer rightEar;
	float64 sr;
	void conv(float64 sample, Buffer& outbuffer, int i, int channel, Buffer* HRFT);
	void setSpeed();
	void updatePos();
	float64 getAmpchange(Vector3f lisnrPos, float64 sample);

};
