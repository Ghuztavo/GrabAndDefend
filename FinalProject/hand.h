#pragma once

#include <raylib.h>
#include <iostream>

#ifndef HAND_H
#define HAND_H

class Hand
{
public:
	int width = 100;
	int height = 140;
	Vector2 position = { 0, 0 };
	int speed = 10;
	bool leftHand = false;
	bool rightHand = false;
	bool grab = false;
	bool slam = false;
	Color handColor = WHITE;
	Texture2D handTexture = { 0 };
	
	// slam circle variables
	float circleRadius = 0.0f;
	float maxCircleRadius = 200.0f;
	float circleGrowthSpeed = 5.0f;
	bool showCircle = false;
	Vector2 slamPosition = { 0, 0 };
	Color slamColor = { 255, 0, 0, 75 };

	~Hand() {
		if (handTexture.id != 0) {
			UnloadTexture(handTexture);
		}
	}

	void DrawHand() {
		if (showCircle) {
			DrawCircle(slamPosition.x + width / 2, slamPosition.y + height / 2, circleRadius, slamColor);
		}
		
		if (handTexture.id != 0) {
			Rectangle source = { 0, 0, (float)handTexture.width, (float)handTexture.height };
			Rectangle dest = { position.x, position.y, (float)width, (float)height };
			DrawTexturePro(handTexture, source, dest, { 0, 0 }, 0, handColor);
		} else {
			DrawRectangle(position.x, position.y, width, height, handColor);
		}
	}
};

#endif // !HAND_H