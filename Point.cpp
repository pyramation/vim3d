#include <math.h>
#include "Point.h"
#include <stdio.h>

const float Point::ACTIVE_RADIUS = 1.1;

Point::Point(float xpos=0.0, float ypos=0.0, float zpos=0.0) {
    x = xpos;
    y = ypos;
    z = zpos;
    active = false;
}

Point::Point(float xpos=0.0, float ypos=0.0) {
    x = xpos;
    y = ypos;
    z = 0.0;
    active = false;
}

Point::Point() {
    Point(0.0,0.0,0.0);
}

float Point::length() {
    return sqrtf( (x * x) + (y * y) + (z * z) );
}

void Point::normalize() {
    float mag = length();
    if (mag == 0) return;
    x /= mag; y /= mag; z /= mag;
}

// need to look into this more, this is orthogonal projection
// does it work for all vectors?
Point Point::projectOrtho(Point &a) {
    // this * transpose(this) = projection matrix
    // [ x*x x*y x*z ][ax]   [ax*x*x + ay*x*y + az*x*z]
    // [ x*y y*y y*z ][ay] = [ax*x*y + ay*y*y + az*y*z]
    // [ x*z y*z z*z ][az]   [ax*x*z + ay*y*z + az*z*z]
    Point p(this->x, this->y, this->z);
    p.normalize();

    return Point(
            a.x*p.x*p.x + a.y*p.x*p.y + a.z*p.x*p.z,
            a.x*p.x*p.y + a.y*p.y*p.y + a.z*p.y*p.z,
            a.x*p.x*p.z + a.y*p.y*p.z + a.z*p.z*p.z
         );
}


Point Point::project(Point &a) {
  Point p(this->x, this->y, this->z);
  p.normalize();
  Point v(a.x, a.y, a.z);
  v.normalize();
  float cosTheta = v*p;
  return p*(cosTheta*a.length());

}

Point Point::normalize(const Point &p) {
    Point a = p;
    a.normalize();
    return a;
}

float Point::fit(float value, float oldmin, float oldmax, float newmin, float newmax) {
    if (value <= oldmin) return newmin;
    if (value >= oldmax) return newmax;
    value = ((value - oldmin) / (oldmax - oldmin)) * (newmax-newmin) + newmin;
    return value;
}


float Point::dot(const Point a, const Point b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Point Point::cross(const Point a, const Point b) {
    return Point(
            a.y*b.z - a.z*b.y,
            a.z*b.x - a.x*b.z,
            a.x*b.y - a.y*b.x);
}

/* operators */
bool Point::operator == (const Point & p) const {
    return ( (x==p.x) && (y==p.y) && (z==p.z) );
}

Point Point::operator + (const Point & p) const {
    Point a( x+p.x, y+p.y, z+p.z);
    return a;
}

Point Point::operator - (const Point & p) const {
    Point a( x-p.x, y-p.y, z-p.z);
    return a;
}


void Point::operator += (const Point & p) {
    x+= p.x;
    y+= p.y;
    z+= p.z;
}

void Point::operator -= (const Point & p) {
    x-= p.x;
    y-= p.y;
    z-= p.z;
}

Point Point::operator ^ (const Point & p) const {
    return Point(
            y*p.z - z*p.y,
            z*p.x - x*p.z,
            x*p.y - y*p.x);
}

float Point::operator * (const Point & p) const {
    return (x * p.x + y * p.y + z * p.z);
}

/* scalar ops */

Point Point::operator / (const float scalar) const {
    Point a(this->x, this->y, this->z);
    a /= scalar;
    return a;
}

Point Point::operator * (const float scalar) const {
    Point a(this->x, this->y, this->z);
    a *= scalar;
    return a;
}

void Point::operator /= (const float scalar) {
    if(!scalar)return;
    x /= scalar;
    y /= scalar;
    z /= scalar;
}

void Point::operator *= (const float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

/* matrices */

Point Point::operator * (const mat4 & M) const {
    vec4 a(this->x, this->y, this->z, 1);
    vec4 c = M * a;
    return Point(c.x, c.y, c.z);
}

Point Point::operator * (const mat3 & M) const {
    vec3 a(this->x, this->y, this->z);
    vec3 c = M * a;
    return Point(c.x, c.y, c.z);
}

void Point::print() { printf("point: x: %f, y: %f, z: %f\n", x, y, z); }

Point::~Point() {}
