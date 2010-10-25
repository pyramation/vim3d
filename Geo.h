#ifndef GEO_H
#define GEO_H


#include <stdio.h>
#include <math.h>
#include "Poly.h"
#include "Channel.h"
#include "Curve.h"

#include "Ray.h"
#include "Render.h"

#include "Object.h"
#include "BoundingBox.h"

#define BASIC_GEO   0
#define INSTANCE_GEO    1
#define SPHERE_GEO  2
#define CUBE_GEO    3
#define GRID_GEO    4
#define ELIPSE_GEO    5

class Geo;
class Intersection;

typedef vector <Geo *> geo_t;

class Geo : public Object {
    public:

        int geoType;


        void recook();
        bool showBounds;

        BoundingBox * boundingBox;
        BoundingBox * getBounds();
        void bound();  // determines bounds
        void bounds(); // displays bounds

        float maxX; float minX; float maxY; float minY; float maxZ; float minZ;
        float cx, cy, cz;

        /* display */
	    Texture * tex;
        Image * normalMap;

        void copyMaterial(Material & mat);

        Poly * base;
        Poly * p; // present or current state
        int State;
        Curve * curve;

        unsigned int presentState;
        unsigned int previewState; // these will reference history_index
        
        bool group; // container
        bool copy; // container
        bool inst; // in the container
        bool disp; // display or not, i.e. used as a template
        Generic * Target;
        Generic * Template;

        Intersection getIntersection(Ray ray, float &closestD, mat4 M);

        void addPoly(Poly * poly);
        void addGeo(Geo * geo);

        GLfloat specular[4];
        GLfloat diffuse[4];
        GLfloat ambient[4];
        GLfloat emission[4];
        GLfloat shininess[1];
        float reflection[3];
        float refractiveIndex;

        Point pos();

        /* light passes, object passes */
        void display(int pass);
        
        void display();
        void copyPoints(vector <Point> * points);
        void scale(float xScale, float yScale, float zScale);
        void move(float yDegrees, float xDegrees, Point camDir);
        void moveUp(float deg);

        void defaults();

        void writeGeometry(String filename);
        void writeMTL(String matLibName);
        Poly * getPoly();

        String propertyInfo();

        static bool isSphere(Geo *geo);
        static bool isCube(Geo *geo);
        static bool isGrid(Geo *geo);

        Geo(bool instance);
        Geo();
        virtual ~Geo();

};


#endif

