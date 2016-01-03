/******************************************************************************
*******************************************************************************

	Space Impakto DS
	Relminator
	http://rel.betterwebber.com


	Viper class


*******************************************************************************
******************************************************************************/


#ifndef AABB_H
#define AABB_H

class AABB 
{

public:
	AABB()
	{
	}
	~AABB()
	{
	}

	inline int Intersects( const AABB &other ) const
	{
	
		if (x2 < other.x1) { return false; }
		if (x1 > other.x2) { return false; }
		if (y2 < other.y1) { return false; }
		if (y1 > other.y2) { return false; }
		return true;

}
	
	inline void Init( int x, int y, int width, int height )
	{
		
		x1 = x;
		y1 = y;
		x2 = ( x + width ) - 1;
		y2 = ( y + height ) - 1;
	
	}
	
	inline AABB Get() const
	{
		return *this;
	}
	
	inline void Resize( int x, int y )
	{
		
		x1 += x;
		y1 += y; 
		x2 -= x;
		y2 -= y;
		
	}
	
	int x1;
	int y1;
	int x2;
	int y2;
};

#endif // AABB_H
