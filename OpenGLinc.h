#ifndef OPENGLINC_H
#define OPENGLINC_H

#if defined(_WIN32)
#include <gl/glut.h>
#elif defined(macintosh) || defined(__MACH__) && defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h> 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#endif

