#ifndef RAY_H
#define RAY_H

#include "Packages.h"
#include "Elements.h"
#include "Object.h"

#include <stdio.h>
#include <math.h>

#include "nv_math.h"
#include "nv_algebra.h"



class Ray {
    public:

        Point pos;
        Point dir;

        
        Ray(int x, int y);
        Ray(Object * from, Object * to);
        Ray();
		static int intersectSphere(Ray ray, float sphereScale, Point spherePosition, Point * I);	
        static int intersectBox(Ray ray, Point min, Point max, Point * hit);
		static int intersectTriangle(Ray ray, vector <Vertex> triangle, Point * I, Point * coords);	
		static int intersectRectangle(Ray ray, vector <Vertex> rectangle, Point * I, Point * coords);
        static void intersectPlaneNoBounds(Point a, Point b, Point planeNormal, Point planePosition, Point * hit);

	
        virtual ~Ray();

};


#endif

