#ifndef GM_UTIL_H
#define GM_UTIL_H

#include <math.h>
#include <stdlib.h>
#include "Vector2D.h"
//#include <android/log.h>


enum DIRECTION{
	RIGHT = 1<<0,
	LEFT  = 1<<1,
	UP = 1<<2,
	DOWN = 1<<3,
	STOPPED= 0,
};

enum FONT{
	AHB_44,
	AHB_36,
	AHB_20,
	DIGIT_X
};

static const float M11=0.0;
static const float M12=1.0;
static const float M13=0.0;
static const float M14=0.0;
static const float M21=-0.5;
static const float M22=0.0;
static const float M23=0.5;
static const float M24=0.0;
static const float M31=1.0;
static const float M32=-2.5;
static const float M33=2.0;
static const float M34=-0.5;
static const float M41=-0.5;
static const float M42=1.5;
static const float M43=-1.5;
static const float M44=0.5;


static Vector2D<float> catmullRomSpline(float x, Vector2D<float> v0,Vector2D<float> v1,
	Vector2D<float> v2,Vector2D<float> v3) {
		Vector2D<float> c1,c2,c3,c4;
		c1 = v1*M12;
		c2 = v0*M21 + v2*M23;
		c3 = v0*M31 + v1*M32 + v2*M33 + v3*M34;
		c4 = v0*M41 + v1*M42 + v2*M43 + v3*M44;
		return(((c4*x + c3)*x +c2)*x + c1);
}

static bool circlesColliding(int x1,int y1,int radius1, int x2,int y2, int radius2)
{
	//compare the distance to combined radii
	int dx = x2 - x1;
	int dy = y2 - y1;
	int radii = radius1 + radius2;
	if ( ( dx * dx )  + ( dy * dy ) < radii * radii )
	{
		return true;
	}
	else
	{
		return false;
	}
}



#endif