#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "../Geometry.h"

class Geo;

class Ellipse : public Geo {
public:

  float radius;
  Ellipse(float width, float height, int levelOfDetail, bool face);
  ~Ellipse();

};

#endif

