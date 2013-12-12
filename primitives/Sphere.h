#ifndef SPHERE_H
#define SPHERE_H

#include "../Geometry.h"

class Geo;

class Sphere : public Geo {
public:

  float radius;
  Sphere(float width, float height, int levelOfDetail);
  ~Sphere();

};

#endif

