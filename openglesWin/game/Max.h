#ifndef Max_H
#define Max_H

#include "Enemy.h"
#include "Vector2D.h"
#include "gl2d.h"
#include "Aabb.h"

#include <vector>

#include "Animation.h"

class Max{
public:
	Max();
	~Max();

	bool update(float time);
	bool render(float time);
	void init();
	void cleanUp();

	void loadTextures();

	std::vector< Vector2D<float> > path;
	Vector2D<float> oldPosition;

	int pathIter;
	float x;

	int loops;

	std::vector<Animation*> animations;	
	Vector2D<float> pos;
	
	int numframes;
	glImage sprMax[34];

};

#endif