#ifndef RENDER_H
#define RENDER_H

#include "Packages.h"
#include "Ray.h"
#include <stdio.h>
#include <math.h>
#include "Color.h"
#include "Geometry.h"

class Light;
class Intersection {
   public:
   Point hit;
   Normal n;
   Geo * geo;
   Point eye;

   Face f; // if not Sphere, or special geo, then has a face
   TexCoord t;
   Material mat;

   Intersection();  
   virtual ~Intersection();

};

typedef struct {
   Color color;
   float depth;
} Pixel;

class Render {
    public:

        /* static vars */

        // number of anti-aliasing samples
        static int samples;
        static int bounces;
        static int w;
        static int h;
        static unsigned int start;
        static unsigned int end;
        static bool jitter;
        static bool cartoon;
        static bool depth_map;

        static void RayTrace(String filename);
        static void CurrentScreen(String filename, GLint x, GLint y, GLsizei w, GLsizei h);
        static void WriteImageToScreen(GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, GLubyte * pixels);

        static Intersection getIntersection(Ray ray);
        static Color shadowRays(Intersection intersection);
		static Pixel getColor(Ray ray, int bounces);
        static Color lightingModel(Light * light, Material * mat, Point eye, Point hit, Point n);
		static Color getRefraction(Intersection intersect, Ray ray, int bounces);
		static Color getReflection(Intersection intersect, Ray ray, int bounces);

        Render();
        virtual ~Render();
};

#endif

