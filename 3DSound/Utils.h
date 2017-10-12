#pragma once
#include <SFML/System.hpp>

#ifndef PI
#define PI 3.1415926535f
#endif

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

#define CHL_L 0
#define CHL_R 1

enum Channel
{
	Right,
	Left
};

typedef sf::Vector3f vec3;
typedef sf::Vector2f vec2;
typedef sf::Vector2i vec2i;

inline float lerp(float a, float b, float d)
{
	return a * d + b * (1 - d);
}

inline Nsound::float64 lerp64(Nsound::float64 a, Nsound::float64 b, Nsound::float64 d)
{
	return a * d + b * (1 - d);
}

// Normalize a vector
inline vec3 Normalize(vec3 v)
{
	float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return v * (1.f / l);
}

inline float sign(float f)
{
	return f < 0 ? -1 : 1;
}

// Get a normalized vector from angle a (degrees)
inline vec2 VecFromAng(float a)
{
	return vec2 (cos(a * DEGTORAD), sin(a * DEGTORAD));
}

// Dot product
inline float Dot(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cross product
inline vec3 Cross(vec3 a, vec3 b)
{
	return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

// Project a vector onto a plane
inline vec3 Project(vec3 v, vec3 n)
{
	return v - Dot(v, n) * n;
}

// Angle between two vectors (in degrees)
inline float Angle(vec3 a, vec3 b)
{
	return acos(Dot(Normalize(a), Normalize(b))) * RADTODEG;
}

// Convert float array to sf::Vector3f
inline vec3 a2v(float f[])
{
	return vec3(f[0], f[1], f[2]);
}

// Check which side of a plane a point lies
inline bool PointSide(vec3 n, vec3 a)
{
	float dot = Dot(-a, n);
	return dot > 0;
}