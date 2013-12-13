#if !defined(CURVE_H)
#define CURVE_H

#include "OpenGLinc.h"
#include <vector>
#define vector std::vector
#include <math.h>
#include "Point.h"
#include "Transform.h"

typedef vector <Point> point_t;


class Curve  
{
    protected:
        point_t::iterator activePoint;
        bool isactive;

    public:
        point_t points;
        Point lastActive;
        Curve();
        Curve( vector <Point> pts );
        virtual ~Curve();

        void EditHandle();

        void addPoint(Point a);
        void deleteActivePoint();
        void moveActivePoint(float dx, float dy, float dz);
        void moveActivePointTo(Point a);
        void updateActivePoint(float x, float y, float z);
        static void printCurve(point_t p);
        void printPoints();
        void printCurve();
        void axis();
        point_t Bezier(int levelOfDetail);
        point_t Bspline(int levelOfDetail);
        point_t Bezier2(int levelOfDetail);
        void noInterpolation();

        point_t translate(float x, float y, float z);
        point_t rotate(float degrees,float x, float y, float z);
        point_t rt(float theta, float x, float y, float z, float tx, float ty, float tz, float cenX, float cenY, float cenZ);
		void rtMod(float theta, float x, float y, float z, float tx, float ty, float tz, float cenX, float cenY, float cenZ);
        Curve * curveM(mat4 matM);
};

#endif

