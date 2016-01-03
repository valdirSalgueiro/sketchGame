#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <math.h>

template <class T>
class Vector2D
{
public:
	//! Default constructor (null vector)
	Vector2D() : x(0), y(0) {}
	//! Constructor with two different values
	Vector2D(T nx, T ny) : x(nx), y(ny) {}
	//! Constructor with the same value for both members
	explicit Vector2D(T n) : x(n), y(n) {}
	//! Copy constructor
	Vector2D(const Vector2D<T>& other) : x(other.x), y(other.y) {}

	// operators

	Vector2D<T> operator-() const { return Vector2D<T>(-x, -y); }

	Vector2D<T>& operator=(const Vector2D<T>& other) { x = other.x; y = other.y; return *this; }


	Vector2D<T> operator+(const Vector2D<T>& other) const { return Vector2D<T>(x + other.x, y + other.y); }

	Vector2D<T>& operator+=(const Vector2D<T>& other) { x+=other.x; y+=other.y; return *this; }
	Vector2D<T> operator+(const T v) const { return Vector2D<T>(x + v, y + v); }
	Vector2D<T>& operator+=(const T v) { x+=v; y+=v; return *this; }


	Vector2D<T> operator-(const Vector2D<T>& other) const { return Vector2D<T>(x - other.x, y - other.y); }

	Vector2D<T>& operator-=(const Vector2D<T>& other) { x-=other.x; y-=other.y; return *this; }
	Vector2D<T> operator-(const T v) const { return Vector2D<T>(x - v, y - v); }
	Vector2D<T>& operator-=(const T v) { x-=v; y-=v; return *this; }


	Vector2D<T> operator*(const Vector2D<T>& other) const { return Vector2D<T>(x * other.x, y * other.y); }
	Vector2D<T>& operator*=(const Vector2D<T>& other) { x*=other.x; y*=other.y; return *this; }
	Vector2D<T> operator*(const T v) const { return Vector2D<T>(x * v, y * v); }
	Vector2D<T>& operator*=(const T v) { x*=v; y*=v; return *this; }

	Vector2D<T> operator/(const Vector2D<T>& other) const { return Vector2D<T>(x / other.x, y / other.y); }
	Vector2D<T>& operator/=(const Vector2D<T>& other) { x/=other.x; y/=other.y; return *this; }
	Vector2D<T> operator/(const T v) const { return Vector2D<T>(x / v, y / v); }
	Vector2D<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

//	//! sort in order x, y. Equality with rounding tolerance.
//	bool operator<=(const Vector2D<T>&other) const
//	{
//		return 	(x<other.x || core::equals(x, other.x)) ||
//				(core::equals(x, other.x) && (y<other.y || core::equals(y, other.y)));
//	}

//	//! sort in order x, y. Equality with rounding tolerance.
//	bool operator>=(const Vector2D<T>&other) const
//	{
//		return 	(x>other.x || core::equals(x, other.x)) ||
//				(core::equals(x, other.x) && (y>other.y || core::equals(y, other.y)));
//	}
//
//	//! sort in order x, y. Difference must be above rounding tolerance.
//	bool operator<(const Vector2D<T>&other) const
//	{
//		return 	(x<other.x && !core::equals(x, other.x)) ||
//				(core::equals(x, other.x) && y<other.y && !core::equals(y, other.y));
//	}
//
//	//! sort in order x, y. Difference must be above rounding tolerance.
//	bool operator>(const Vector2D<T>&other) const
//	{
//		return 	(x>other.x && !core::equals(x, other.x)) ||
//				(core::equals(x, other.x) && y>other.y && !core::equals(y, other.y));
//	}

	bool operator==(const Vector2D<T>& other) const { return equals(other); }
	bool operator!=(const Vector2D<T>& other) const { return !equals(other); }

	// functions

//	//! Checks if this vector equals the other one.
//	/** Takes floating point rounding errors into account.
//	\param other Vector to compare with.
//	\return True if the two vector are (almost) equal, else false. */
//	bool equals(const Vector2D<T>& other) const
//	{
//		return core::equals(x, other.x) && core::equals(y, other.y);
//	}

	Vector2D<T>& set(T nx, T ny) {x=nx; y=ny; return *this; }
	Vector2D<T>& set(const Vector2D<T>& p) { x=p.x; y=p.y; return *this; }

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	T getLength() const { return sqrt( x*x + y*y ); }

	T normalize(){
		 T fLength = sqrt( x * x + y * y);
		 if ( fLength > 0.0f )
		 {
			 T fInvLength = 1.0f / fLength;
			 x *= fInvLength;
			 y *= fInvLength;
		 }

		 return fLength;
	}

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	T getLengthSQ() const { return x*x + y*y; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const Vector2D<T>& other) const
	{
		return x*other.x + y*other.y;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	T getDistanceFrom(const Vector2D<T>& other) const
	{
		return Vector2D<T>(x - other.x, y - other.y).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const Vector2D<T>& other) const
	{
		return Vector2D<T>(x - other.x, y - other.y).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
//	Vector2D<T>& rotateBy(f64 degrees, const Vector2D<T>& center=Vector2D<T>())
//	{
//		degrees *= DEGTORAD64;
//		const f64 cs = cos(degrees);
//		const f64 sn = sin(degrees);
//
//		x -= center.x;
//		y -= center.y;
//
//		set((T)(x*cs - y*sn), (T)(x*sn + y*cs));
//
//		x += center.x;
//		y += center.y;
//		return *this;
//	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const Vector2D<T>& begin, const Vector2D<T>& end) const
	{
		if (begin.x != end.x)
		{
			return ((begin.x <= x && x <= end.x) ||
				(begin.x >= x && x >= end.x));
		}
		else
		{
			return ((begin.y <= y && y <= end.y) ||
				(begin.y >= y && y >= end.y));
		}
	}


	//! x coordinate of vector.
	T x;

	//! y coordinate of vector.
	T y;
};

#endif
