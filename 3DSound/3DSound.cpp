// 3DSound.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Window.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>

using namespace Nsound;

int main()
{
	sf::Window window(sf::VideoMode(800, 600), "My window");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}

	
	//To olny test right and left ear sound
	AudioStream LAudio("x1.wav"); //Load left ear audio. Need to be AudioStream to control numofchannels. 
	LAudio.mono(); //Force it to Mono to be sure it ony have one channel
	AudioStream RAudio("Temperature_in.wav");;//Load right ear audio. Need to be AudioStream to control numofchannels. 
	RAudio.mono(); //Force it to Mono to be sure it ony have one channel

	RAudio.normalize(); //Normalize the sound to get a better level
	LAudio.normalize();


	/*
	//Check the HRFT. (For me it works :O 
	AudioStream RAudio("x1.wav"); //Load right ear audio. Need to be AudioStream to control numofchannels
	RAudio.mono(); //Force it to Mono to be sure it ony have one channel
	AudioStream LAudio("x1.wav"); //Only nesscary to make one left and one right if we want to handel them seperate before convolution
	LAudio.mono(); 

	int angle = 90; //Hardcoded angle (to the right)
	int elevation = 0; //Hardcoded evelation (level zero)
	Buffer Lb("HRTFdata/L" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for left channel at the right ang and ev
	Buffer Rb("HRTFdata/R" + std::to_string(elevation) + "e0" + std::to_string(angle) + "a.wav"); //Load hrftdata for right channel at the right ang and ev

	LAudio.convolve(Lb); //Make a convolution on leftaudio with leftchannel hrft;
	RAudio.convolve(Rb); //Make a convolution on rightaudio with rightchannel hrft; 
	*/


	float64 sr = 44100.0; //Set the samplerate (this should accually be dynamic TODO)

	AudioStream as(sr, 2); //Make a two channel audiostream with right samplerate

	as[0] << LAudio; //Asign left sound to the left (audiostreamname[0]) buffer in audiostream 
	as[1] << RAudio; //Asign right sound to the left (audiostreamname[1]) buffer in audiostream

	AudioPlayback pb(sr, 2, 16); //Make a playback "source" that plays two channels and has right samplerate and 16bit/sample

	as >> pb; //Play the audiostream in the audioplayback

    return 0;
}

//ab.plot("testing");
//FFTransform t(44100.0);
/*FFTChunkVector abf = t.fft(ab, 2);
FFTChunkVector Lbf = t.fft(Lb, 2);

//FFTChunkVector resultf = abf*Lbf;
std::cout << abf.getOriginalSize();
Buffer result = t.ifft(resultf);
*/
