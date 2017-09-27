#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>

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
	void calcAngleElev();
	bool setSound(string filname/*, string filetype*/);
	void setSound(int freq, double time);
	AudioStream& getChannel(int channel);
	float64& getSampleRate();
	void generate3D();
	
private:
	int angle, elevation;
	Vector3f position;
	AudioStream leftEar;
	AudioStream rightEar;
	float64 sr;
	
};