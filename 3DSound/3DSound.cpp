// 3DSound.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SoundNode.h"
#include <SFML/Graphics.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

using namespace Nsound;
using namespace sf;

int main()
{
	// Create a window
	RenderWindow window(VideoMode(1280, 720), "3DSound");
	window.setFramerateLimit(60);

	// Initialize ImGui
    ImGui::SFML::Init(window);

	// Initialize frame clock
	Clock clock;

	while (window.isOpen())
	{
		Time dt = clock.restart();

		Event event;
		while (window.pollEvent(event))
		{
			// Poll ImGui events
			ImGui::SFML::ProcessEvent(event);

			if (event.type == Event::Closed)
				window.close();
		}

		// Update ImGui
		ImGui::SFML::Update(window, dt);

        ImGui::Begin("Amazing 3D Sound Generator");
        if (ImGui::Button("Generate 3D Sound"))
		{
			SoundNode sound("x1.wav", vector<double>(0, 3));
			sound.generate3D();
			// Make a two channel AudioStream with right samplerate
			AudioStream as(sound.getSampleRate(), 2);

			// Assign left sound to the left (AudioStream[0]) buffer in AudioStream 
			as[0] << sound.getChannel(0);
			// Assign right sound to the left (AudioStream[1]) buffer in AudioStream
			as[1] << sound.getChannel(1);

			// Make a playback "source" that plays two channels and has right samplerate and 16bit/sample
			AudioPlayback pb(sound.getSampleRate(), 2, 16);

			// Play the AudioStream in the AudioPlayback
			as >> pb;
		}
        ImGui::End();

		// Draw stuff
		window.clear(Color(46, 46, 46, 255));
        ImGui::SFML::Render(window);
        window.display();
	}

	// Play nice and kill ImGui before exiting
	ImGui::SFML::Shutdown();
	
    return 0;
}
