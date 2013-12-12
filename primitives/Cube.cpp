#include "Cube.h"

Cube::Cube(){
    geoType = CUBE_GEO;
    p = new Poly();
    Point pt1(0,0,0);
    Point pt2(0,0,10);
    Point pt3(10,0,10);
    Point pt4(10,0,0);
    Point pt5(10,10,0);
    Point pt6(10,10,10);
    Point pt7(0,10,10);
    Point pt8(0,10,0);

    p->addFace(pt1, pt2, pt7, pt8); // bottom
    p->addFace(pt3, pt4, pt5, pt6); // right
    p->addFace(pt1, pt2, pt3, pt4); // front 
    p->addFace(pt5, pt6, pt7, pt8); // top
    p->addFace(pt1, pt4, pt5, pt8); // left
    p->addFace(pt2, pt3, pt6, pt7); // back 

    p->computeNormals();
    this->addPoly(p); // this adds it to scene
    p->bound();
    this->bound();
}

Cube::~Cube(){}
