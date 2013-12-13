#include "Camera.h"
#include "Scene.h"
#include <math.h>

Camera::Camera(){
    type = CAMERA_TYPE;

    eye = Point(-16.0f, 10.0f, -16.0f);
    focus = Point(0,1,0);
    up = Point(0, 1, 0);

    fovy = 90;
    zNear = 0.1;
    zFar = 500;

    String c("Cam");
    name = Scene::name(c);
	zoomScale = .20f;
    distance = 16.0f;

}

void Camera::reset() {
    eye = Point(-16.0f, 10.0f, -16.0f);
    up = Point(0, 1, 0);
    focus = Point(0,1,0);
}

void Camera::top(){
    up.x = 0.0f;
    up.y = 0.0f;
    up.z = -1.0f;
    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = 0.0f;
    eye.x= 0.0f;
    eye.y = 40.0f;
    eye.z = 0.0f;
}
void Camera::front(){
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = 0.0f;
    eye.x= 0.0f;
    eye.y = 0.0f;
    eye.z = -40.0f;
}
void Camera::back(){
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = 0.0f;
    eye.x= 0.0f;
    eye.y = 0.0f;
    eye.z = -40.0f;
}
void Camera::left(){
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = 0.0f;
    eye.x= 40.0f;
    eye.y = 0.0f;
    eye.z = 0.0f;
}
void Camera::right(){
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    focus.x = 0.0f;
    focus.y = 0.0f;
    focus.z = 0.0f;
    eye.x= -40.0f;
    eye.y = 0.0f;
    eye.z = 0.0f;
}

String Camera::propertyInfo() {
    char s[256];
    sprintf(s, "position: %f %f %f\nlookat: %f %f %f\nup: %f %f %f\nzNear: %f\nzFar: %f\nfovy: %f\n",
            eye.x, eye.y, eye.z,
            focus.x, focus.y, focus.z,
            up.z, up.y, up.z,
            zNear, zFar, fovy);
    return String(s);
}


void Camera::display(int pass){}

void Camera::display() {
    /*
       Scene::toggleLights(false);
       if (active && this != Scene::camera) {
       glPushMatrix();
       glLoadIdentity();
       glTranslatef(eye.x, eye.y, eye.z);
       glutWireSphere(3.44, 4.55,4.55);
       glPopMatrix();
       }
       Scene::toggleLights(true);
       */
}

void Camera::axis(){
    glLineWidth(3.0) ;
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(eye.x,eye.y,eye.z);
    //glVertex3f(eye.x+u.x, eye.y+u.y, eye.z+u.z);
    glVertex3f(eye.x+1, eye.y, eye.z);

    glColor3f(0.0,1.0,0.0);
    glVertex3f(eye.x,eye.y,eye.z);
    //glVertex3f(eye.x+v.x, eye.y+v.y, eye.z+v.z);
    glVertex3f(eye.x, eye.y+1, eye.z);

    glColor3f(0.0,0.0,1.0);
    glVertex3f(eye.x,eye.y,eye.z);
    //glVertex3f(eye.x+w.x, eye.y+w.y, eye.z+w.z);
    glVertex3f(eye.x, eye.y, eye.z+1);
    glEnd();
}

// zoom
void Camera::dolly(float amount) {
    amount *= zoomScale/4.0f;
    Point w = eye - focus;
    w.normalize();
    eye = eye - Point::normalize(w)*amount;
    focus = focus - Point::normalize(w)*amount;
}

//roll
void Camera::roll(float degrees) {
    float scale = 5.0f;
    float the = degrees*3.1451/180*scale;
    Point pointTowards = Point::normalize(focus-eye);
    rodriguez_rotate(the, up, pointTowards);
}

// move up/down
void Camera::pedestal(float amount) {
    amount = -amount/20.0f*zoomScale;
    up.normalize();
    focus = focus + up*amount;
    eye = eye + up*amount;
}

// yaw
void Camera::pan(float degrees) {
    //rodriguez_rotate(degrees, eye, up);
    float scale = .30f;
    float the = degrees*3.1451/180*scale;
    Point pointTowards = Point::normalize(focus-eye);
    Point pointAway = pointTowards*-1;
    Point oldCenter = eye + pointTowards*distance; //possible to use focus as center?
    mat3 axisAngle = rotateMat(the, up);
    pointAway = Point::normalize(pointAway*axisAngle);
    eye = oldCenter + pointAway*distance;
    focus = oldCenter + pointAway*(distance/2.0f);
}

//pitch
void Camera::tilt(float degrees) {

    //    Point newAxis = Point::cross(up, eye);
    //   rodriguez_rotate(degrees, eye, newAxis);
    //  rodriguez_rotate(degrees, up, newAxis);

    float scale = .30f;
    float the = -degrees*3.1451/180*scale;
    Point pointTowards = Point::normalize(focus-eye);
    Point pointAway = pointTowards*-1;
    Point oldCenter = eye + pointTowards*distance; //possible to use focus as center?
    Point axis = Point::normalize(Point::cross(up, pointTowards));
    mat3 axisAngle = rotateMat(the, axis);
    pointAway = Point::normalize(pointAway*axisAngle);
    eye = oldCenter + pointAway*distance;
    focus = oldCenter + pointAway*(distance/2.0f);
    up = Point::normalize(up*axisAngle);



}

// follow left or right
void Camera::truck(float amount) {
    amount = -amount/15.0f*zoomScale;
    up.normalize();
    Point looK = focus - eye;
    Point truckV = Point::normalize(Point::cross(up, looK)); 
    focus = focus + truckV*amount;
    eye = eye + truckV*amount;
}

mat4 Camera::lookAt() {
    /* w = normalize(eye) */
    Point w = eye-focus;
    w.normalize();

    /* u = normalize(up x normalize(eye)) */
    Point u = Point::cross(up, w);
    u.normalize();

    /* v = w x u */
    Point v = Point::cross(w, u);

    /* construct transformation matrix */
    mat4 T; 
    T.a00 = 1;  T.a01 = 0; T.a02 = 0; T.a03 = -eye.x; 
    T.a10 = 0;  T.a11 = 1; T.a12 = 0; T.a13 = -eye.y; 
    T.a20 = 0;  T.a21 = 0; T.a22 = 1; T.a23 = -eye.z; 
    T.a30 = 0;  T.a31 = 0; T.a32 = 0; T.a33 = 1; 

    /* construct rotation matrix */
    mat4 R;
    R.a00 = u.x;  R.a01 = u.y; R.a02 = u.z; R.a03 = 0; 
    R.a10 = v.x;  R.a11 = v.y; R.a12 = v.z; R.a13 = 0; 
    R.a20 = w.x;  R.a21 = w.y; R.a22 = w.z; R.a23 = 0; 
    R.a30 = 0;    R.a31 = 0;   R.a32 = 0;   R.a33 = 1; 

    mat4 M = R * T;

    //glMatrixMode(GL_MODELVIEW);
    //glMultMatrixf(M.mat_array);

    return M;

}

//GLdouble Camera::fovx() { return 2.0f * atan(float(Scene::w)/(2.0f*zNear)); }
GLdouble Camera::fovx() { return 2.0f * atan(float(Scene::w)/(2.0f)); }

coordinateFrame_t Camera::coordinateFrame() { return Transform::getCoordinateFrame(up, focus-eye); }

void Camera::setDistance() {
  distance = eye.length();
  if (zoomScale < 1000) zoomScale=distance/10.0f;
  if (distance > 10 ) zFar = distance*distance;
}

Camera::~Camera(){}
