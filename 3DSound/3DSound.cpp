// 3DSound.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Window.hpp>
#include <Nsound/NsoundAll.h>

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

	int angle = 0;
	int elevation = 0;
	AudioStream a("x1.wav");
	AudioStream L("HRTFdata/L" + std::to_string(elevation) + "e00" + std::to_string(angle) + "a.wav");
	AudioStream R("HRTFdata/R" + std::to_string(elevation) + "e00" + std::to_string(angle) + "a.wav");

	float64 sr = 44100.0;
    // Play it through the sound card
	AudioPlayback pb(sr, 2, 16);
    a >> pb;

    return 0;
}

