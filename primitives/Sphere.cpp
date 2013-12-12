#include <math.h>
#include "Sphere.h"
#include "Ellipse.h"

Sphere::Sphere(float width, float height, int levelOfDetail){
    geoType = SPHERE_GEO;
    //radius = width/2.0;
    radius = width;
	Geo * circle = (Geo *) new Ellipse(width, height, levelOfDetail, false);
	p = circle->p;
	p->Revolve(180, 0, 1, 0);

    // calculate normals
    int index=0;
    for(unsigned int i=0;i<p->faces.size();i++) {
        for(unsigned int j=0; j<p->faces[i].vi.size(); j++) {
            Normal n(p->vertices[p->faces[i].vi[j]-1].x, p->vertices[p->faces[i].vi[j]-1].y, p->vertices[p->faces[i].vi[j]-1].z);
            n.normalize();
            p->normals.push_back(n);
            index++;
            p->faces[i].ni.push_back(index);
        }
    }
    delete circle;

    this->bound();

    // we don't need to add any poly since ellipse did that
}


Sphere::~Sphere(){}
