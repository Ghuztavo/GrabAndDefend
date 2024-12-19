#pragma once

#include <raylib.h>
#include <iostream>

#ifndef CHEST_H
#define CHEST_H

class Chest
{
public:
	int width = 100;
	int height = 80;
	Vector2 position = { 0, 0 };
	bool grabbed = false;
	Texture2D texture = { 0 };  // Initialize to zero

	Chest() {
		LoadChestTexture();
	}

	// Copy constructor
	Chest(const Chest& other) {
		width = other.width;
		height = other.height;
		position = other.position;
		grabbed = other.grabbed;
		LoadChestTexture();  // Load a fresh copy of the texture
	}

	// Assignment operator
	Chest& operator=(const Chest& other) {
		if (this != &other) {
			width = other.width;
			height = other.height;
			position = other.position;
			grabbed = other.grabbed;
			// Don't copy the texture, keep our own
		}
		return *this;
	}

	~Chest() {
		if (texture.id != 0) {
			UnloadTexture(texture);
		}
	}

	void DrawChest() {
		if (texture.id != 0) {
			Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
			Rectangle dest = { position.x, position.y, (float)width, (float)height };
			DrawTexturePro(texture, source, dest, { 0, 0 }, 0, WHITE);
		} else {
			DrawRectangle(position.x, position.y, width, height, BROWN);
		}
	}

private:
	void LoadChestTexture() {
		texture = LoadTexture("Resources/chest.png");
	}
};

#endif // !CHEST_H