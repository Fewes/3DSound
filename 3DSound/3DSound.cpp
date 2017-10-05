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
	float soundEndPos[3]{ 2, 0, 0 };

	/*
	float elev = 0;
	float pan = 0;
	int elevIndex = 0;
	int panIndex = 0;
	*/

	Texture listenerTex, soundTex, soundEndTex;
	listenerTex.loadFromFile("Sprites/Listener.png");
	soundTex.loadFromFile("Sprites/Sound.png");
	soundEndTex.loadFromFile("Sprites/SoundEnd.png");
	Sprite listenerSprite, soundSprite, soundEndSprite;
	listenerSprite.setTexture(listenerTex);
	soundSprite.setTexture(soundTex);
	soundEndSprite.setTexture(soundEndTex);

	listenerSprite.setOrigin(listenerTex.getSize().x * 0.5f, listenerTex.getSize().y * 0.5f);
	soundSprite.setOrigin(soundTex.getSize().x * 0.5f, soundTex.getSize().y * 0.5f);
	soundEndSprite.setOrigin(soundEndTex.getSize().x * 0.5f, soundEndTex.getSize().y * 0.5f);

	float ppm = 512; // Texture pixels per meter
	float ppmInv = 1 / ppm;

	listenerSprite.setScale(ppmInv, ppmInv);
	soundSprite.setScale(ppmInv, ppmInv);
	soundEndSprite.setScale(ppmInv, ppmInv);

	sf::View view;
	vec2 viewPos = vec2(0, 0);
	float viewScale = 0.01f;
	bool viewFollowsListener = false;

	const float sliderSens = 0.001f;

	vec2i lastMousePos;

	while (window.isOpen())
	{
		Time elapsed = clock.restart();
		float dt = elapsed.asSeconds();

		Event event;
		while (window.pollEvent(event))
		{
			// Poll ImGui events
			ImGui::SFML::ProcessEvent(event);

			if (event.type == Event::Closed)
				window.close();
			else if (event.type == sf::Event::MouseWheelMoved)
            {
				if (event.mouseWheel.delta > 0)
                    viewScale /= 1.25;
				else
					viewScale *= 1.25;
            }
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::F)
				{
					viewPos = vec2(0, 0);
					viewScale = 0.01f;
				}
			}
		}

		vec2i mousePos = Mouse::getPosition(window);
		vec2i diff = mousePos - lastMousePos;
		vec2 diffScaled = vec2(diff.x * viewScale, diff.y * viewScale);
		if (Mouse::isButtonPressed(Mouse::Right) || Mouse::isButtonPressed(Mouse::Middle))
		{
			viewPos -= diffScaled;
		}

		if (Keyboard::isKeyPressed(Keyboard::Left)  || Keyboard::isKeyPressed(Keyboard::A))
			listenerAngle -= 270 * dt;
		if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
			listenerAngle += 270 * dt;
		while (listenerAngle > 180)
			listenerAngle -= 360;
		while (listenerAngle < -180)
			listenerAngle += 360;
		if (Keyboard::isKeyPressed(Keyboard::Up)	|| Keyboard::isKeyPressed(Keyboard::W))
		{
			listenerSprite.move(VecFromAng(listenerSprite.getRotation()) * 2.f * dt);
			listenerPos[0] = listenerSprite.getPosition().x;
			listenerPos[1] = listenerSprite.getPosition().y;
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)	|| Keyboard::isKeyPressed(Keyboard::S))
		{
			listenerSprite.move(-VecFromAng(listenerSprite.getRotation()) * 2.f * dt);
			listenerPos[0] = listenerSprite.getPosition().x;
			listenerPos[1] = listenerSprite.getPosition().y;
		}

		// Update ImGui
		ImGui::SFML::Update(window, elapsed);

        ImGui::Begin("Amazing 3D Sound Generator");
			ImGui::Spacing();
			ImGui::DragFloat3("Listener Position",	listenerPos, sliderSens);
			ImGui::SliderFloat("Listener Angle", &listenerAngle, -180, 180);
			listenerDir[0] = cos(listenerAngle * DEGTORAD);
			listenerDir[1] = sin(listenerAngle * DEGTORAD);
			listenerDir[2] = 0;
			ImGui::DragFloat3("Sound Position",		soundPos,	 sliderSens);
			ImGui::DragFloat3("Soundend Position", soundEndPos, sliderSens);
			ImGui::Checkbox("View Follows Listener", &viewFollowsListener);

			ImGui::Spacing();

			if (ImGui::Button("Generate 3D Sound"))
			{
				SoundNode sound("x1.wav", a2v(soundPos));
				sound.setEndPos(a2v(soundEndPos));
				sound.setPos(a2v(soundPos));
				sound.buildSound(&(a2v(listenerPos)), &(Normalize(a2v(listenerDir))), &hrtfCache);
				// Make a two channel AudioStream with right samplerate
				AudioStream as(sound.getSampleRate(), 2);
				// Assign left sound to the left (AudioStream[0]) buffer in AudioStream 
				as[0] << sound.getChannel(0);
				// Assign right sound to the left (AudioStream[1]) buffer in AudioStream
				as[1] << sound.getChannel(1);
				// Play the AudioStream in the AudioPlayback
				AudioPlayback pb(sound.getSampleRate(), 2, 16);
				as >> pb;
			}

			hrtfCache.GetHRTF(a2v(listenerPos), Normalize(a2v(listenerDir)), a2v(soundPos), Right);
			/*
			ImGui::LabelText("Elevation", to_string(elev).c_str());
			ImGui::LabelText("Pan", to_string(pan).c_str());
			ImGui::LabelText("Elevation Index", to_string(elevIndex).c_str());
			ImGui::LabelText("Pan Index", to_string(panIndex).c_str());
			*/
        ImGui::End();

		// Update view transform
		vec2 viewExtents = vec2(window.getSize().x * viewScale, window.getSize().y * viewScale);
		vec2 vp;
		if (viewFollowsListener)
			vp = listenerSprite.getPosition();
		else
			vp = viewPos;
		view.reset(sf::FloatRect(vp.x - viewExtents.x * 0.5f, vp.y - viewExtents.y * 0.5f, viewExtents.x, viewExtents.y));
		if (viewFollowsListener)
			view.setRotation(listenerSprite.getRotation());
		else
			view.setRotation(0);
		window.setView(view);

		// Update sprite transforms
		listenerSprite.setPosition(vec2(listenerPos[0], listenerPos[1]));
		listenerSprite.setRotation(listenerAngle);
		soundSprite.setPosition(vec2(soundPos[0], soundPos[1]));
		soundEndSprite.setPosition(vec2(soundEndPos[0], soundEndPos[1]));

		// Draw stuff
		window.clear(Color(46, 46, 46, 255));
		window.draw(listenerSprite);
		window.draw(soundSprite);
		window.draw(soundEndSprite);
        ImGui::SFML::Render(window);
        window.display();

		// Update history
		lastMousePos = mousePos;
	}

	// Play nice and kill ImGui before exiting
	ImGui::SFML::Shutdown();
	
    return 0;
}
