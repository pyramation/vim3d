#ifndef GRID_H
#define GRID_H

#include "../Geometry.h"

class Geo;

class Grid : public Geo {
public:

  int dx,dz;
  float segmentSize, tx, tz;
    
  Grid();
  Grid(float minX, float minY, float maxX, float maxY);
  Grid(float w, float h);
  Grid(int divisions);
  Grid(int divisionsX, int DivisionsZ, float segmentSize, float tx, float tz);	
  ~Grid();

};

#endif

