// File:			player.cpp
// Author:			Nixon Kwok
// Contributors:	a
//
// Description:	Player class with SDL_Texture, physical, physics specifications
//				for the moving character. State and location parameters for the
//				sprite sheet texture are kept in SDL_Rect structs.
//
// Notes:
//

#include "player.h"
#include "user_interface.h"

Player::Player() : Sprite(
	"sprite.png",	// SPRITE_SHEET_FILENAME
	24,				// SPRITE_SHEET_WIDTH
	28,				// SPRITE_SHEET_HEIGHT
	8,				// SPRITE_WIDTH
	14,				// SPRITE_HEIGHT
	4				// SPRITE_GROW
) {
	spriteState = 0;

	spriteStateRect;
	spriteStateRect.w = SPRITE_WIDTH;
	spriteStateRect.h = SPRITE_HEIGHT;
	UpdateStateRect();

	spriteLocRect;
	locX = 0;
	locY = (double) (UserInterface::ScreenProperties::INIT_SCREEN_HEIGHT - (SPRITE_HEIGHT * SPRITE_GROW));
	UpdateLocVariables();
	spriteLocRect.w = SPRITE_WIDTH * SPRITE_GROW;
	spriteLocRect.h = SPRITE_HEIGHT * SPRITE_GROW;

	distanceToMove = 0.0;

	screenFloor = spriteLocRect.y;

	vertVel = 0.0;
}

Player::~Player() {
	SDL_DestroyTexture(spriteSheet);
	spriteSheet = nullptr;
	int i = 0;
	for (i; i < 1000; i++)
		std::cout << "player deleted" << std::endl;
}

void Player::AttemptJump() {
	if (numberJumps > 0) {
		numberJumps--;
		vertVel = jumpStrength;
	}
}

void Player::MoveSprite(char direction) {
	if (IsOffGround()) {
		if (direction == 'R') {
			distanceToMove = jumpDistance;
			spriteState = 2;
		} else {
			distanceToMove = jumpDistance *-1;
			spriteState = 5;
		}
		UpdateStateRect();
		return;
	}

	if (direction == 'R') {
		switch (spriteState) {
			case 0: {
				spriteState = 1;	// into lean
				distanceToMove = leanDistance;
				break;
			}
			case 1: {
				spriteState = 2;	// into lunge
				distanceToMove = lungeDistance;
				break;
			}
			case 2: {
				spriteState = 1;	// into lunge prep
				distanceToMove = lungeDistance;
				break;
			}
			default: {
				spriteState = 1;	// other case; turn around into lean, 0 movement
				break;
			}
		}
	} else if (direction == 'L') {
		switch (spriteState) {
			case 3: {
				spriteState = 4;	// into lean
				distanceToMove = leanDistance *-1;
				break;
			}
			case 4: {
				spriteState = 5;	// into lunge
				distanceToMove = lungeDistance *-1;
				break;
			}
			case 5: {
				spriteState = 4;	// into lunge prep
				distanceToMove = lungeDistance *-1;
				break;
			}
			default: {
				spriteState = 4;	// other case; turn around into lean, 0 movement
				break;
			}
		}
	}
	UpdateStateRect();
}

void Player::StopSprite() {
	distanceToMove = 0;
	if (spriteState <= 2) {
		spriteState = 0;
	} else {
		spriteState = 3;
	}

	UpdateStateRect();
}

void Player::UpdateStateRect() {
	spriteStateRect.x = (spriteState % 3) * SPRITE_WIDTH;
	if (spriteState <= 2) {
		spriteStateRect.y = 0;
	} else {
		spriteStateRect.y = SPRITE_HEIGHT;
	}

	if (stateQueue.size() == QUEUE_SIZE) {
		stateQueue.pop();
	}
	stateQueue.push(spriteStateRect);
}

void Player::UpdateLocRect() {
	locX += distanceToMove;
	locY -= vertVel;
	if (IsOffGround()) {
		vertVel += gravity;
	}
	if (locY > screenFloor)	{
		locY = screenFloor;
		vertVel = 0.0;

		numberJumps++;
	}

	UpdateLocVariables();
	std::cout << "x: " << locX << " y: " << locY <<
		" state: " << spriteState <<
		" vertVel: " << vertVel <<
		" IsOffGround: " << IsOffGround() << std::endl;
	//std::cout << static_cast<int>(Movement::JUMP) << std::endl;
}

void Player::UpdateLocVariables() {
	spriteLocRect.x = static_cast<int>(locX + 0.5);
	spriteLocRect.y = static_cast<int>(locY + 0.5);
	if (locationQueue.size() == QUEUE_SIZE) {
		locationQueue.pop();
	}
	locationQueue.push(spriteLocRect);
}

bool Player::IsOffGround() {
	return locY < screenFloor;
}

void Player::GiveInstruction(uMovementType moveFlags) {
	if (moveFlags & Movement::JUMP) {
		this->AttemptJump();
	}

	if (moveFlags & Movement::LEFT && moveFlags & Movement::RIGHT) {
		this->StopSprite();
	} else if (moveFlags & Movement::LEFT) {
		this->MoveSprite('L');
	} else if (moveFlags & Movement::RIGHT) {
		this->MoveSprite('R');
	} else {
		this->StopSprite();
	}

	this->UpdateLocRect();
}

SDL_Rect Player::GetQueueStateRect() {
	return stateQueue.front();
}

SDL_Rect Player::GetQueueLocRect() {
	return locationQueue.front();
}
