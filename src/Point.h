#ifndef POINT_H
#define POINT_H

#include "nv_math.h"
#include "nv_algebra.h"

class Point;
typedef Point Vertex;
typedef Point Normal;
typedef Point TexCoord;
typedef Point Vector;


class Point {
public:
	static const float ACTIVE_RADIUS;

	float x;
	float y;
	float z;
	bool active;

    Point(float xpos = 0.0, float ypos = 0.0, float zpos = 0.0);	

    float length();
    void normalize();
    static Point normalize(const Point &p);
    static float fit(float value, float oldmin, float oldmax, float newmin, float newmax);
    static float dot(const Point a, const Point b);
    static Point cross(const Point a, const Point b);


    // vector projection
    Point project(Point &a);
    Point projectOrtho(Point &a);


    /* operators */

    // vectors on vectors
    bool operator == (const Point & p) const;
    Point operator + (const Point & p) const;
    Point operator - (const Point & p) const;
    void operator += (const Point & p);
    void operator -= (const Point & p);

    // cross and dot!
    Point operator ^ (const Point & p) const;
    float operator * (const Point & p) const;

    // vectors and scalars
    Point operator * (const float scalar) const;
    Point operator / (const float scalar) const;
    void operator *= (const float scalar);
    void operator /= (const float scalar);
    
    // matrices
    Point operator * (const mat4 & M) const;
    Point operator * (const mat3 & M) const;	

    void print();

	virtual ~Point();

};


#endif

