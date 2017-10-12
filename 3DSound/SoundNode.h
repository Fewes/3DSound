#pragma once

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>
#include <math.h>  
#include "HRTFCache.h"
#include <algorithm>
#include "Utils.h"

#define CHL_L 0
#define CHL_R 1
#define PI_ 3.1415
#define Io_ pow(10, -12)

using namespace std;
using namespace Nsound;
using namespace sf;

class CFSound
{
public:
	Buffer*	src;
	Buffer*	dst;

	Buffer*	buf1;
	Buffer*	buf2;
	Buffer	HRTF1;
	Buffer	HRTF2;

	size_t	chunkSize;
	size_t	chunkIndex;
	size_t	sampleIndex;

	bool	dirty = true;

	CFSound (Buffer* s, Buffer* d, Buffer hrtf1, size_t cSize = 12)
	{
		src = s;
		dst = d;
		buf1 = new Buffer(s->getLength());
		buf2 = new Buffer(s->getLength());
		HRTF1 = hrtf1;
		chunkSize = cSize;
	}

	~CFSound ()
	{
		delete(buf1);
		delete(buf2);
	}

	void conv(Buffer* ear, Buffer & outbuffer, int i, int channel, Buffer& HRFT)
	{
		/*for (int j = 0; j < HRFT.getLength() && (j + i) < outbuffer.getLength(); j++) {
			outbuffer[i + j] += sample * HRFT[j];
		}*/

		for (int j = 0; j <= HRFT.getLength() && i - j >= 0; j++) {
			outbuffer[i] += (*ear)[i - j] * HRFT[j];
		}
	}

	void step ()
	{
		conv(src, *buf1, sampleIndex, 0, HRTF1);
		conv(src, *buf2, sampleIndex, 0, HRTF2);

		float fade = ((float)chunkIndex) / ((float)chunkSize);

		fade = 0;

		(*dst)[sampleIndex] = lerp((*buf1)[sampleIndex], (*buf2)[sampleIndex], 1 - fade);

		sampleIndex++;
		chunkIndex++;

		if (chunkIndex > chunkSize)
		{
			swap();
			chunkIndex = 0;
		}
	}

	void swap ()
	{
		auto tmp1 = buf1;
		buf1 = buf2;
		buf2 = tmp1;
		auto tmp2 = HRTF1;
		HRTF1 = HRTF2;
		HRTF2 = tmp2;

		dirty = true;
	}

private:
	
};

class SoundNode
{
public:
	SoundNode();
	SoundNode(string filname, Vector3f pos);
	SoundNode(const SoundNode& SN);
	void		setPos(Vector3f pos);
	void		setPos(float x, float y, float z);
	void		setEndPos(Vector3f pos);
	void		setEndPos(float x, float y, float z);
	bool		setSound(string filname/*, string filetype*/);
	void		setSound(int freq, double time);
	Buffer&		getChannel(int channel);
	float64&	getSampleRate();
	void		buildSound(Vector3f* lisnrPos, Vector3f* listenerDir, HRTFCache* hrtfCache);
	
private:
	Vector3f	position;
	Vector3f	startPos;
	Vector3f	endPos;
	Vector3f	speed;
	Buffer		leftEar;
	Buffer		rightEar;

	float64		sr;
	//void		conv(float64 sample, Buffer& outbuffer, int i, int channel, Buffer& HRFT);
	void		conv(Buffer* ear, Buffer& outbuffer, int i, int channel, Buffer& HRFT);
	void		setSpeed();
	void		updatePos();
	float64		getAmpchange(Vector3f* lisnrPos, float64 sample);

};
