#ifndef COLOR_H
#define COLOR_H

#include "Packages.h"
#include "Elements.h"

#include <stdio.h>
#include <math.h>

class Color {
    public:

        float r;
        float g;
        float b;

        void operator *= (const float scalar);
        void operator /= (const float scalar);
        void operator += (const Color colour);
        void operator *= (const Color colour);
        Color operator + (const Color c);
        Color operator + (const float s);
        Color operator * (const float s);
        Color operator / (const float s);

        void clamp();

        Color(float r, float g, float b);
        Color();
        virtual ~Color();

};

#endif

