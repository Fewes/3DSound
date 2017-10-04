// 3DSound.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SoundNode.h"
#include <SFML/Graphics.hpp>
#include <Nsound/NsoundAll.h>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "HRTFCache.h"

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

	// Initialize HRTF cache
	HRTFCache hrtfCache;

	float listenerPos[3] {0, 0, 0};
	float listenerDir[3] {1, 0, 0};
	float listenerAngle = 0;
	float soundPos[3] {2, 0, 0};

	float elev = 0;
	float pan = 0;

	const float sliderSens = 0.001f;

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
				SoundNode sound("x1.wav", Vector3f(1, 1, 1));
				sound.setEndPos(Vector3f(-1, -1, -1));
				AudioPlayback pb(sound.getSampleRate(), 2, 16);
				sound.buildSound(Vector3f(0, 0, 0));
				// Make a two channel AudioStream with right samplerate
				AudioStream as(sound.getSampleRate(), 2);

				// Assign left sound to the left (AudioStream[0]) buffer in AudioStream 
				as[0] << sound.getChannel(0);
				// Assign right sound to the left (AudioStream[1]) buffer in AudioStream
				as[1] << sound.getChannel(1);

				// Play the AudioStream in the AudioPlayback
				as >> pb;
			}

			ImGui::DragFloat3("Listener Position",	listenerPos, sliderSens);
			ImGui::SliderFloat("Listener Angle", &listenerAngle, -180, 180);
			listenerDir[0] = cos(listenerAngle * DEGTORAD);
			listenerDir[1] = sin(listenerAngle * DEGTORAD);
			listenerDir[2] = 0;
			ImGui::DragFloat3("Sound Position",		soundPos,	 sliderSens);

			hrtfCache.GetHRTF(a2v(listenerPos), Normalize(a2v(listenerDir)), a2v(soundPos), elev, pan);
			ImGui::LabelText("Elevation", to_string(elev).c_str());
			ImGui::LabelText("Pan", to_string(pan).c_str());
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
