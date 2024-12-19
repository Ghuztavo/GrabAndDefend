#pragma once

#include <raylib.h>
#include <iostream>

#ifndef ENEMY_H
#define ENEMY_H

class Enemy
{
public:
	float radius = 20.0f;
	Vector2 position = { 0, 0 };
	Vector2 speed = { 2, 2 };
	Color color = YELLOW;
	bool active = false;


	void DrawEnemy() {
		DrawCircle(position.x, position.y, radius, color);
	}
};

#endif // !ENEMY_H