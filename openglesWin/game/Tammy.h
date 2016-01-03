#ifndef Tammy_H
#define Tammy_H

#include "Enemy.h"
#include "Vector2D.h"
#include "gl2d.h"
#include "Aabb.h"

#include <vector>

#include "Animation.h"

class Tammy{
public:
	Tammy();
	~Tammy();

	bool update(float time);
	bool render(float time);
	void init();
	void cleanUp();

	void loadTextures();

	std::vector< Vector2D<float> > path;
	Vector2D<float> oldPosition;

	int pathIter;
	float x;

	std::vector<Animation*> animations;	
	Vector2D<float> pos;
	
	int loops;
	int numframes;

	glImage sprTammy[30];

};

#endif