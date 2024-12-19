#include "GrabAndDefend.h"

Vector2 GrabAndDefend::getDirection(Vector2 position, Vector2 target)
{
	Vector2 direction = { target.x - position.x, target.y - position.y };
	float length = sqrt(direction.x * direction.x + direction.y * direction.y);
	direction.x /= length;
	direction.y /= length;
	return direction;
}

Vector2 GrabAndDefend::getRandPosition()
{
	int border = rand() % 4;
	float margin = 100.0f; // Distance outside the screen to spawn enemies
	switch (border)
	{
	case 0: //top
		return { (float)(rand() % screenWidth), -margin };
	case 1: //bottom
		return { (float)(rand() % screenWidth), screenHeight + margin };
	case 2: //left
		return { -margin, (float)(rand() % screenHeight) };
	case 3: //right
		return { screenWidth + margin, (float)(rand() % screenHeight) };
	}
}

void GrabAndDefend::SpawnParticles(Vector2 position, Color color)
{
    const int numParticles = 20;  // Number of particles to spawn
    for (int i = 0; i < numParticles; i++)
    {
        // Random angle and speed for particle
        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = (float)(rand() % 200 + 100);  // Random speed between 100 and 300
        
        Vector2 velocity = {
            cosf(angle) * speed,
            sinf(angle) * speed
        };
        
        // Random lifetime between 0.5 and 1 second
        float lifetime = (float)(rand() % 500 + 500) / 1000.0f;
        
        // Random size between 2 and 4
        float size = (float)(rand() % 3 + 2);
        
        particles.push_back(Particle(position, velocity, lifetime, color, size));
    }
}

void GrabAndDefend::Main()
{
	screenWidth = 1250;
	screenHeight = 1000;

	InitWindow(screenWidth, screenHeight, "Grab And Defend");
	SetTargetFPS(60);

	isInTitleScreen = true;
	titleY = -200;  // Start title above screen
	titleSpeed = 2;  // Speed of title animation

	chest = nullptr;

	Start();

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		Update();
		Draw();

		EndDrawing();
	}

	UnloadTexture(background);
	if (chest != nullptr) {
		delete chest;
		chest = nullptr;
	}
	CloseWindow();
}

void GrabAndDefend::Start()
{
	//Load background
	background = LoadTexture("Resources/grass.png");

	//Load hand textures
	leftHand.handTexture = LoadTexture("Resources/left_hand.png");
	rightHand.handTexture = LoadTexture("Resources/right_hand.png");

	// Set title target Y position to middle of screen
	titleTargetY = screenHeight / 2 - 100;  // Slightly above center

	//initialize hands and chest
	leftHand.position = {0, (float)screenHeight/2};
	leftHand.leftHand = true;
	leftHand.rightHand = false;

	rightHand.position = { (float)screenWidth - rightHand.width, (float)screenHeight / 2 };
	rightHand.leftHand = false;
	rightHand.rightHand = true;

	// Create chest
	chest = new Chest();
	chest->position = { (float)screenWidth / 2 - chest->width / 2, (float)screenHeight / 2 - chest->height / 2 };
	chest->grabbed = false;

	//initialize enemies
	srand(time(NULL));
	for (int i = 0; i < maxEnemies; i++)
	{
		Enemy enemy;
		int border = rand() % 4;
		float margin = 100.0f; // Distance outside the screen to spawn enemies

		switch (border)
		{
		case 0: //top
			enemy.position = { (float)(rand() % screenWidth), -margin };
			break;
		case 1: //bottom
			enemy.position = { (float)(rand() % screenWidth), screenHeight + margin };
			break;
		case 2: //left
			enemy.position = { -margin, (float)(rand() % screenHeight) };
			break;
		case 3: //right
			enemy.position = { screenWidth + margin, (float)(rand() % screenHeight) };
			break;
		}

		enemy.active = true;
		enemies.push_back(enemy);
	}
}

void GrabAndDefend::Update()
{
    float deltaTime = GetFrameTime();

    // Update particles
    for (int i = particles.size() - 1; i >= 0; i--)
    {
        particles[i].Update(deltaTime);
        if (!particles[i].IsAlive())
        {
            particles.erase(particles.begin() + i);
        }
    }

	// Title screen logic
	if (isInTitleScreen) {
		// Animate title
		if (titleY < titleTargetY) {
			titleY += titleSpeed;
			if (titleY > titleTargetY) {
				titleY = titleTargetY;  // Prevent overshooting
			}
		}

		// Check for game start
		if (IsKeyPressed(KEY_ENTER)) {
			isInTitleScreen = false;
			Reset();  // Initialize game state
			return;
		}
		return;  // Don't process other updates while in title screen
	}

	// Check for pause toggle
	if (IsKeyPressed(KEY_P)) {
		isPaused = !isPaused;
		return;
	}

	// Check for game restart
	if (isGameOver) {
		if (IsKeyPressed(KEY_ENTER)) {
			Reset();
			return;
		}
		return;  // Don't process other updates if game is over
	}

	// Don't update if game is paused
	if (isPaused) {
		return;
	}

	//leftHand movement
	if (IsKeyDown(KEY_A))
	{
		leftHand.position.x -= leftHand.speed;
	}
	if (IsKeyDown(KEY_D))
	{
		leftHand.position.x += leftHand.speed;
	}
	if (IsKeyDown(KEY_W))
	{
		leftHand.position.y -= leftHand.speed;
	}
	if (IsKeyDown(KEY_S))
	{
		leftHand.position.y += leftHand.speed;
	}

	// Clamp left hand position to window boundaries
	leftHand.position.x = std::max(0.0f, std::min(leftHand.position.x, (float)screenWidth - leftHand.width));
	leftHand.position.y = std::max(0.0f, std::min(leftHand.position.y, (float)screenHeight - leftHand.height));

	if (IsKeyPressed(KEY_SPACE) && !leftHand.showCircle)
	{
		leftHand.slam = true;
		leftHand.showCircle = true;
		leftHand.circleRadius = 0.0f;
		leftHand.slamPosition = leftHand.position;
	}

	if (leftHand.showCircle)
	{
		leftHand.circleRadius += leftHand.circleGrowthSpeed;
		
		// Check collision with enemies
		Vector2 slamCenter = { 
			leftHand.slamPosition.x + leftHand.width/2, 
			leftHand.slamPosition.y + leftHand.height/2 
		};
		
		for (Enemy& enemy : enemies)
		{
			if (enemy.active)
			{
				// Calculate distance between slam center and enemy
				float dx = enemy.position.x - slamCenter.x;
				float dy = enemy.position.y - slamCenter.y;
				float distance = sqrt(dx * dx + dy * dy);
				
				// If enemy is within slam radius
				if (distance <= leftHand.circleRadius + enemy.radius)
				{
					// Store the current position for particles
					Vector2 deathPosition = enemy.position;

					// Deactivate enemy and respawn at random position
					enemy.position = getRandPosition();
					playerScore += 10; // Add points for hitting enemy

					// Spawn particles at the stored death position
					SpawnParticles(deathPosition, YELLOW);

					// Check if we need to increase max enemies
					int newMaxEnemies = 10 + (playerScore / 1000);
					if (newMaxEnemies > maxEnemies) {
						maxEnemies = newMaxEnemies;
						// Add new enemy
						Enemy newEnemy;
						newEnemy.position = getRandPosition();
						newEnemy.active = true;
						enemies.push_back(newEnemy);
					}
				}
			}
		}

		if (leftHand.circleRadius >= leftHand.maxCircleRadius)
		{
			leftHand.showCircle = false;
			leftHand.slam = false;
		}
	}

	//rightHand movement
	Vector2 mousePos = GetMousePosition();
	rightHand.position = { 
		std::max(0.0f, std::min(mousePos.x - rightHand.width/2, (float)screenWidth - rightHand.width)),
		std::max(0.0f, std::min(mousePos.y - rightHand.height/2, (float)screenHeight - rightHand.height))
	};

	//grabbing
	// Check if we need to release due to timer
	if (rightHand.grab && GetTime() > grabTimer + timeWhenGrabbed)
	{
		if (chest != nullptr) {
			chest->grabbed = false;
		}
		rightHand.grab = false;
		allowGrab = false;
		timeWhenReleased = GetTime();
		rightHand.handColor = RED;  // Set to red when entering cooldown
	}

	// Update grab cooldown
	if (!rightHand.grab && GetTime() > timeWhenReleased + grabCooldown)
	{
		allowGrab = true;
		rightHand.handColor = WHITE;  // Set back to white when cooldown is over
	}

	// Handle mouse input only if we're allowed to grab
	if (allowGrab)
	{
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			if (chest != nullptr && !rightHand.grab && CheckCollisionRecs({ rightHand.position.x, rightHand.position.y, (float)rightHand.width, (float)rightHand.height }, { chest->position.x, chest->position.y, (float)chest->width, (float)chest->height }))
			{
				chest->grabbed = true;
				rightHand.grab = true;
				timeWhenGrabbed = GetTime();
			}
		}
		else 
		{
			if (chest != nullptr) {
				chest->grabbed = false;
			}
			rightHand.grab = false;
		}
	}

	// Update chest position if grabbed
	if (chest != nullptr && chest->grabbed && rightHand.grab)
	{
		chest->position = { rightHand.position.x, rightHand.position.y };
	}

	// Update enemies
	// Move enemies towards chest or right hand if chest is grabbed
	for (Enemy& enemy : enemies)
	{
		Vector2 target;
		if (chest != nullptr && chest->grabbed) {
			// Target center of right hand
			target = {
				rightHand.position.x + rightHand.width / 2,
				rightHand.position.y + rightHand.height / 2
			};
		} else {
			// Target center of chest
			if (chest != nullptr) {
				target = {
					chest->position.x + chest->width / 2,
					chest->position.y + chest->height / 2
				};
			}

			// Check collision with chest when not grabbed
			if (chest != nullptr && CheckCollisionCircleRec(
				enemy.position,
				enemy.radius,
				{ chest->position.x, chest->position.y, (float)chest->width, (float)chest->height }))
			{
				playerLife = std::max(0, playerLife - 10);
				// Spawn particles at enemy position
                SpawnParticles(enemy.position, ORANGE);
				// Respawn enemy at random position after hitting chest
				enemy.position = getRandPosition();

				// Check if player died
				if (playerLife <= 0) {
					isGameOver = true;
				}
			}
		}
		
		Vector2 direction = getDirection(enemy.position, target);
		
		// Update enemy position
		enemy.position.x += direction.x * enemy.speed.x;
		enemy.position.y += direction.y * enemy.speed.y;
	}
}

void GrabAndDefend::Reset()
{
	// Reset game state
	playerLife = 100;
	playerScore = 0;
	isGameOver = false;
	maxEnemies = 10;  // Reset to initial enemy count

	// Reset hands positions
	leftHand.position = { 0, (float)screenHeight / 2 };
	rightHand.position = { (float)screenWidth - rightHand.width, (float)screenHeight / 2 };

	// Reset chest
	if (chest != nullptr) {
		chest->position = { (float)screenWidth / 2 - chest->width / 2, (float)screenHeight / 2 - chest->height / 2 };
		chest->grabbed = false;
	}

	// Reset hand states
	rightHand.grab = false;
	allowGrab = true;
	rightHand.handColor = WHITE;

	// Reset enemies
	enemies.clear();
	for (int i = 0; i < maxEnemies; i++) {
		Enemy enemy;
		enemy.position = getRandPosition();
		enemy.active = true;
		enemies.push_back(enemy);
	}
}

void GrabAndDefend::Draw()
{
	Rectangle destRect = { 0, 0, (float)screenWidth, (float)screenHeight };
	DrawTexturePro(background, { 0, 0, (float)background.width, (float)-background.height }, destRect, { 0, 0 }, 0, WHITE);

    // Draw title screen
    if (isInTitleScreen) {
        const char* titleText = "GRAB AND DEFEND";
        const char* startText = "Press ENTER to Start";
        const char* instructionText = "Use WASD to move left hand";
        const char* instruction2Text = "Use MOUSE to move right hand";
        const char* instruction3Text = "Press LEFT MOUSE to grab the chest";
        const char* instruction4Text = "Press SPACE to slam with left hand";
        
        // Draw semi-transparent black overlay
        DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 180 });

        // Draw animated title
        int titleFontSize = 80;
        int startFontSize = 30;
        int instructionFontSize = 20;
        
        int titleWidth = MeasureText(titleText, titleFontSize);
        int startWidth = MeasureText(startText, startFontSize);
        
        DrawText(titleText, 
            (screenWidth - titleWidth) / 2, 
            (int)titleY,
            titleFontSize, RED);

        // Only show "Press ENTER" and instructions when title reaches its target
        if (titleY >= titleTargetY) {
            DrawText(startText,
                (screenWidth - startWidth) / 2,
                titleY + 100,
                startFontSize, WHITE);

            // Draw instructions
            int instructionY = titleY + 200;
            DrawText(instructionText,
                screenWidth / 2 - MeasureText(instructionText, instructionFontSize) / 2,
                instructionY,
                instructionFontSize, WHITE);

            DrawText(instruction2Text,
                screenWidth / 2 - MeasureText(instruction2Text, instructionFontSize) / 2,
                instructionY + 30,
                instructionFontSize, WHITE);

            DrawText(instruction3Text,
                screenWidth / 2 - MeasureText(instruction3Text, instructionFontSize) / 2,
                instructionY + 60,
                instructionFontSize, WHITE);

            DrawText(instruction4Text,
                screenWidth / 2 - MeasureText(instruction4Text, instructionFontSize) / 2,
                instructionY + 90,
                instructionFontSize, WHITE);
        }

        return;  // Don't draw game elements during title screen
    }

    // Draw enemies first (behind other objects)
	for (Enemy& enemy : enemies)
	{
		if (enemy.active)
		{
			enemy.DrawEnemy();
		}
	}

    // Draw particles
    for (const Particle& particle : particles)
    {
        particle.Draw();
    }

	if (chest != nullptr) {
		chest->DrawChest();
	}
	leftHand.DrawHand();
	rightHand.DrawHand();

	// Draw the score and health bar
	const char* scoreText = TextFormat("Score: %d", playerScore);
	DrawText(scoreText, 20, 20, 30, BLACK);

	// Draw health bar background
	float healthBarX = (screenWidth - healthBarWidth) / 2;
	float healthBarY = 20;
	DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, GRAY);

	// Draw current health (scales with playerLife)
	float currentHealthWidth = (float)healthBarWidth * (playerLife / 100.0f);
	DrawRectangle(healthBarX, healthBarY, (int)currentHealthWidth, healthBarHeight, healthBarColor);

	// Draw health bar border
	DrawRectangleLinesEx({ healthBarX, healthBarY, (float)healthBarWidth, (float)healthBarHeight }, 2, BLACK);

	// Draw health value
	const char* healthText = TextFormat("%d%%", playerLife);
	int textWidth = MeasureText(healthText, 20);
	DrawText(healthText, healthBarX + (healthBarWidth - textWidth) / 2, healthBarY + healthBarHeight + 5, 20, BLACK);

	// Draw pause screen
	if (isPaused) {
		const char* pauseText = "PAUSED";
		const char* resumeText = "Press P to Resume";
		
		// Draw semi-transparent black overlay
		DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 150 });

		// Draw pause text
		int pauseFontSize = 60;
		int resumeFontSize = 30;
		int pauseWidth = MeasureText(pauseText, pauseFontSize);
		int resumeWidth = MeasureText(resumeText, resumeFontSize);

		DrawText(pauseText, 
			(screenWidth - pauseWidth) / 2, 
			screenHeight / 2 - pauseFontSize,
			pauseFontSize, WHITE);

		DrawText(resumeText,
			(screenWidth - resumeWidth) / 2,
			screenHeight / 2 + 20,
			resumeFontSize, WHITE);
	}

	// Draw game over screen
	if (isGameOver) {
		const char* gameOverText = "GAME OVER";
		const char* restartText = "Press ENTER to restart";
		
		// Draw semi-transparent black overlay
		DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 180 });

		// Draw game over text
		int gameOverFontSize = 60;
		int restartFontSize = 30;
		int gameOverWidth = MeasureText(gameOverText, gameOverFontSize);
		int restartWidth = MeasureText(restartText, restartFontSize);

		DrawText(gameOverText, 
			(screenWidth - gameOverWidth) / 2, 
			screenHeight / 2 - gameOverFontSize,
			gameOverFontSize, RED);

		DrawText(restartText,
			(screenWidth - restartWidth) / 2,
			screenHeight / 2 + 20,
			restartFontSize, WHITE);

		// Draw final score
		const char* finalScoreText = TextFormat("Final Score: %d", playerScore);
		int scoreWidth = MeasureText(finalScoreText, restartFontSize);
		DrawText(finalScoreText,
			(screenWidth - scoreWidth) / 2,
			screenHeight / 2 + 60,
			restartFontSize, WHITE);
	}
}