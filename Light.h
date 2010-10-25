#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

#define L_DIR 0
#define L_POINT 1
#define L_SPOT 2
#define L_AREA 3

class Light;
typedef vector <Light *> light_t;

class Light : public Object {
 public:
    GLfloat specular[4];
    GLfloat diffuse[4];
    GLfloat ambient[4];

    /* SPOT LIGHT */
    GLfloat spotDir[3]; /* spot light direction */
    GLfloat cutoff; /* cutoff angle */
    GLfloat spotExp;

    /* attenuation */
    GLfloat constant;
    GLfloat linear;
    GLfloat quadratic;
 
    int ltype;

    float w; float h; // only for area lights

    bool on;

    String propertyInfo();
    void display(int pass);
    void display();
	
	void move(float yDegrees, float xDegrees, Point camDir);
	void moveUp(float deg);
	void rotateLight(float degrees, Point axis);
	

    Light();
    virtual ~Light();

};


#endif
