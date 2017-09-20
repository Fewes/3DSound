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

	float64 sr = 44100.0;

    // Create a new instance of the Sine Generator
    Sine sine(sr);

    Buffer pan;

    pan << sine.generate(1.0, 3.0);

    // Create a stereo AudioStream.
    AudioStream as(sr, 2);

    // Fill it with a 220 Hz sine wave.
    as << 0.5 * sine.generate(4.9, 220);

    // Execute the pan method.
    as.pan(pan);

    // Write the AudioStream to a wave file
    as >> "example2.wav";

    // Play it through the sound card

    AudioPlayback pb(sr, 2, 16);

    as >> pb;

    return 0;
}

