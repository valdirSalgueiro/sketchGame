#ifndef PLAYER_H
#define PLAYER_H

#include "Vector2D.h"
#include "gl2d.h"
#include "Aabb.h"
#include <vector>

class BulletMLParser;

class Player{
public:
	Player();
	~Player();

	bool update(float time, int direction);
	bool render(float time);
	void init();
	void cleanUp();
	bool collides(Vector2D<float> pos);

	void loadTextures();

private:
	glImage spr[1];
	int loops;

	BulletMLParser* bps[5];
	glImage* shot;

	int direction;




};

#endif