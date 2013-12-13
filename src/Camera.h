#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

class Camera;
typedef vector <Camera *> cam_t;

class Camera : public Object {
 public:

    GLdouble fovy;
    GLdouble fovx();
    GLdouble zNear;
    GLdouble zFar;
    Point eye;
    Point focus;
    Point up;

    coordinateFrame_t coordinateFrame();

	float zoomScale;
    float distance;
    void setDistance();
	
    void reset();
    String propertyInfo();
    void display(int pass);
    void display();
    void axis();

    /* viewing */
    void top();
    void front();
    void back();
    void left();
    void right();


    void roll(float degrees);
    void dolly(float amount);
    void pedestal(float amount);
    void truck(float amount);
    void tilt(float degrees);
    void pan(float degrees);

    mat4 lookAt();

    Camera();
    virtual ~Camera();

};


#endif
