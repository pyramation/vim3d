#ifndef IMAGE_H
#define IMAGE_H

#include "Packages.h"
#include "Elements.h"

#include <stdio.h>
#include <math.h>

#include "Generic.h"

class Image : public Generic {
    public:
        
        GLubyte * pixels;
        GLsizei w, h;
        GLenum format, imgtype;

        void WriteImage(String filename);
        static void WriteImage(String filename, int w, int h, GLubyte * pixels);

        static void SobelEdgeDetect(int w, int h, GLubyte * pixels);


        Image(GLsizei w, GLsizei h, GLenum format, GLenum imgtype);
        

        Image(String filename);
        Image();
        virtual ~Image();

};

#endif

