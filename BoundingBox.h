#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Packages.h"
#include "Elements.h"

#include <stdio.h>
#include <math.h>

class BoundingBox {
    public:

        Point min;
        Point max;
        Point centroid;

        int num_pts;
        bool init;

        void initialize();
        void display();
        void Bound(vector<Point>&pts);
        void Bound(BoundingBox * box);
        void Bound(BoundingBox * box, mat4 M);

        BoundingBox();
        virtual ~BoundingBox();

};

#endif

