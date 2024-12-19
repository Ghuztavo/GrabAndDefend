#pragma once

#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include "Hand.h"
#include "Chest.h"
#include "Enemy.h"
#include "Particle.h"

#ifndef GRABANDDEFEND_H
#define GRABANDDEFEND_H

class GrabAndDefend
{
private:
	Texture2D background = { 0 };

public:

	int screenWidth = 1250;
	int screenHeight = 1000;

	bool isGameOver = false;
	bool isPaused = false;
	bool isInTitleScreen = true;
	float titleY = -100.0f;  // Start above screen
	float titleTargetY = 0.0f;  // Will be set to middle of screen
	float titleSpeed = 2.0f;

	Hand leftHand;
	int slamCooldown = 2;

	Hand rightHand;
	bool allowGrab = true;
	float grabTimer = 2;
	int timeWhenGrabbed = 0;
	int grabCooldown = 2;
	float timeWhenReleased = 0;

	Chest* chest = nullptr;

	std::vector<Enemy> enemies;
	int maxEnemies = 10;

	std::vector<Particle> particles;

	int playerLife = 100;
	int playerScore = 0;

	int healthBarWidth = 200;
	int healthBarHeight = 20;
	Color healthBarColor = GREEN;

	void Main();
	void Start();
	void Update();
	void Draw();
	void Reset();
	void SpawnParticles(Vector2 position, Color color);
	Vector2 getDirection(Vector2 position, Vector2 target);
	Vector2 getRandPosition();

};

#endif // !GRABANDDEFEND_H