#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>

using namespace std;
using namespace Nsound;

class SoundNode
{
public:
	SoundNode();
	SoundNode(string filname, vector<double> pos);
	SoundNode(const SoundNode& SN);
	void setPos(vector<double> pos);
	void setPos(double x, double y, double z);
	bool setSound(string filname/*, string filetype*/);
	void setSound(int freq, double time);
	AudioStream& getChannel(int channel);
	float64& getSampleRate();
	void generate3D();
	
private:
	vector<double> position;
	AudioStream leftEar;
	AudioStream rightEar;
	float64 sr;
};
