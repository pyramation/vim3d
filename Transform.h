#ifndef TRANSFORM_H
#define TRANSFORM_H


#include <stdio.h>
#include <math.h>

#include "Point.h"

typedef struct {
Point u;
Point v;
Point w;
} coordinateFrame_t;

#include "Generic.h"

class Transform : public Generic{
public:

    mat4 M;
    
    bool locked;
    void axis();

    String matrixInfo();

    void rodriguez_rotate(float degrees, Point&eye, Point&up);
	
	
	/* Geo and Light Transformations */
	void move(float yDegrees, float xDegrees, Point camDir);
	void rotateObj(float degrees, Point axis);

    void loadIdentity();

    Point pos();

    static mat3 rotateMat(float theta, Point&axis);
	static mat4 rotateMat4(float theta, Point axis);
	static mat4 scaleNormalsMatrix(float x, float y, float z);
    static mat4 coordinateFrame(Point up, Point f);
    static mat4 identity();
    static mat4 noTrans(mat4 M);
    static mat4 invTranspose(mat4 M);
    static mat4 invTransposeForNormals(mat4 M);
    static coordinateFrame_t getCoordinateFrame(Point up, Point f);
	
    Transform();
	virtual ~Transform();

};


#endif
