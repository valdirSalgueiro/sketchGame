#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <vector>
#include <map>

#include "Vector2D.h"
#include <sstream>
#include <string>
#include "gl2d.h"

#include "Player.h"

#include "font/PreCompile.h"
#include "font/FontAtlas.h"
#include "font/FTBitmapFont.h"


class Engine{
public:

	enum DIRECTION{
		RIGHT = 1<<0,
		LEFT  = 1<<1,
		UP = 1<<2,
		DOWN = 1<<3
	};

	~Engine();
	void init(int width, int height);
	void deinit();
	void update(float time);
	void render(float time);
	void playing(float time);

	void loadTextures();

	void handleInput(int type,int direction_);

	void reset();

	enum STATE{
		LOGO1,
		LOGO2,
		TITLE,
		INSTRUCT,
		STARTER,
		PLAYING,
		GAMEOVER
	};

	bool doubleTouch, shownScore;
	STATE gameState;

	Vector2D<float> controlKnobPos;
	Vector2D<float> controlBasePos;

	int width, height;

private:


	glImage* controlBase;
	glImage* controlKnob;

	Player* player;

	bool started;

	bool hasInit;
};

void addObj(class Object* object);
void addPlayerObj(Object* object);
extern int turn;

#endif