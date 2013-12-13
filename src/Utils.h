#ifndef UTILS_H
#define UTILS_H

#include "Packages.h"
#include "Elements.h"

#include "Scene.h"

#include <stdio.h>
#include <math.h>

class Utils {
    public:

        static String GeoProperties(Geo * g);
        static String CamProperties(Camera * c);
        static String LightProperties(Light * l);
        static String matrix(Object * o);
        static String Param(int param);
        static int Param(String param);
    
        static void writeMTL(FILE * fd, Material &mat);

        Utils();
        virtual ~Utils();
};

#endif

