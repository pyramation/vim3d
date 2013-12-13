#include "Transform.h"


void Transform::rotateObj(float degrees, Point axis) {
    float theta = degrees*3.1451f/180.0f;
    mat4 rot4 = rotateMat4(theta, axis);
	Point tempT = Point(M.a03, M.a13, M.a23);
	M.a03=0; M.a13=0; M.a23=0;
    M = rot4*M;
	M.a03 = tempT.x; M.a13=tempT.y; M.a23 = tempT.z;
}

mat3 Transform::rotateMat(float theta, Point&axis) {
    mat3 R;
    R.a00 = cos(theta) + axis.x*axis.x*(1-cos(theta)); R.a01 = axis.x*axis.y*(1-cos(theta)) - axis.z*sin(theta); R.a02 = (1-cos(theta))*axis.x*axis.z + axis.y*sin(theta); 
    R.a10 = (1-cos(theta))*axis.x*axis.y + axis.z*sin(theta); R.a11 = cos(theta) + (1-cos(theta))*axis.y*axis.y; R.a12 = (1-cos(theta))*axis.y*axis.z - axis.x*sin(theta); 
    R.a20 = (1-cos(theta))*axis.x*axis.z -axis.y*sin(theta); R.a21 = (1-cos(theta))*axis.y*axis.z + axis.x*sin(theta); R.a22 = cos(theta) + (1-cos(theta))*axis.z*axis.z;
    return R;
}

mat4 Transform::rotateMat4(float theta, Point axis) {
    mat4 R;
    R.a00 = cos(theta) + axis.x*axis.x*(1-cos(theta)); R.a01 = axis.x*axis.y*(1-cos(theta)) - axis.z*sin(theta); R.a02 = (1-cos(theta))*axis.x*axis.z + axis.y*sin(theta); R.a03 = 0;
    R.a10 = (1-cos(theta))*axis.x*axis.y + axis.z*sin(theta); R.a11 = cos(theta) + (1-cos(theta))*axis.y*axis.y; R.a12 = (1-cos(theta))*axis.y*axis.z - axis.x*sin(theta); R.a13 = 0;
    R.a20 = (1-cos(theta))*axis.x*axis.z -axis.y*sin(theta); R.a21 = (1-cos(theta))*axis.y*axis.z + axis.x*sin(theta); R.a22 = cos(theta) + (1-cos(theta))*axis.z*axis.z;  R.a23 = 0;
    R.a30 = R.a31 = R.a32 = 0; R.a33 = 1;
    return R;
}

void Transform::rodriguez_rotate(float degrees, Point&eye, Point&up) {
    degrees = 3.14159 / 180.0f * degrees; 
    up.normalize();


    Point v1 = eye * cos(degrees);
    Point v2 = Point::cross(up, eye) * sin(degrees);
    Point v3 = up * Point::dot(up, eye);
    v3 = v3 * (1-cos(degrees));
    eye = v1 + v2 + v3;
}

void drawArrow(Point v) {
    glColor3f(1.0,1.0,1.0);
    glRasterPos3f(v.x,v.y,v.z);

    char sx[5];
    sprintf(sx, "%G", v.x);
    char sy[5];
    sprintf(sy, "%G", v.y);
    char sz[5];
    sprintf(sz, "%G", v.z);

    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'(');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sx[0] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sx[1] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sx[2] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sx[3] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sx[4] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,',');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sy[0] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sy[1] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sy[2] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sy[3] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sy[4] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,',');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sz[0] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sz[1] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sz[2] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sz[3] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, sz[4] );
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,')');

    glBegin(GL_TRIANGLES);
    glVertex3f(v.x, v.y, v.z);
    glVertex3f(v.x+0.1, v.y-0.1, v.z);
    glVertex3f(v.x-0.1, v.y-0.1, v.z);
    glEnd();
}


void printVector(Point v) {
    printf("\nVECTOR: \n<%f %f %f>\n", v.x, v.y, v.z);  
}

void printMatrix(mat4 * M) {

    printf("\nMATRIX: \n%f %f %f %f\n", M->a00, M->a01, M->a02, M->a03);  
    printf("%f %f %f %f\n", M->a10, M->a11, M->a12, M->a13); 
    printf("%f %f %f %f\n", M->a20, M->a21, M->a22, M->a23); 
    printf("%f %f %f %f\n\n", M->a30, M->a31, M->a32, M->a33); 
}


Transform::Transform() {

    M.a00 = 1;  M.a01 = 0; M.a02 = 0; M.a03 = 0; 
    M.a10 = 0;  M.a11 = 1; M.a12 = 0; M.a13 = 0; 
    M.a20 = 0;  M.a21 = 0; M.a22 = 1; M.a23 = 0; 
    M.a30 = 0;  M.a31 = 0; M.a32 = 0; M.a33 = 1; 

    //Point u = Point(1,0,0);
    //Point v = Point(0,1,0);
    //Point w = Point(0,0,1);

    locked = false;
}

Transform::~Transform() {}

void Transform::axis(){
    float scale = 10.0;
    float ascale = 2.0;
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(scale,0.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,scale,0.0);

    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,scale);
    glEnd();

    glPushMatrix();
    glColor3f(1.0,0.0,0.0);
    glTranslatef(scale,0.0,0.0);
    glRotatef(90,0.0,1.0,0.0);

    glutSolidCone(ascale/2.0f,ascale,10,10);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.0,1.0,0.0);
    glTranslatef(0.0,scale,0.0);
    glRotatef(-90,1.0,0.0,0.0);
    glutSolidCone(ascale/2.0f,ascale,10,10);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0,0.0,1.0);
    glTranslatef(0.0,0.0,scale);
    glutSolidCone(ascale/2.0f,ascale,10,10);
    glPopMatrix();
    glColor3i(1,1,1);

}

Point operator * (const mat4 &M, Point&eye) {
    vec4 v(eye.x, eye.y, eye.z, 1);
    vec4 b = M * v;
    return Point(b.x, b.y, b.z);
}

String Transform::matrixInfo() {
    char s[256];
    sprintf(s, "matrix:\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
            M.a00,  M.a01, M.a02, M.a03,
            M.a10,  M.a11, M.a12, M.a13,
            M.a20,  M.a21, M.a22, M.a23,
            M.a30,  M.a31, M.a32, M.a33);
    return String(s);
}

coordinateFrame_t Transform::getCoordinateFrame(Point up, Point f) {
    up.normalize();
    f.normalize();
    Point w = up^f;
    w.normalize();
    Point u = f^w;
    u.normalize();
    coordinateFrame_t c = coordinateFrame_t();
    c.u = u;
    c.v = f;
    c.w = w;
    return c;
}

mat4 Transform::coordinateFrame(Point up, Point f) {
    coordinateFrame_t c = getCoordinateFrame(up,f);
    mat4 M;    
    M.a00=c.u.x;  M.a01=c.v.x; M.a02=c.w.x; M.a03=0.0f;
    M.a10=c.u.y;  M.a11=c.v.y; M.a12=c.w.y; M.a13=0.0f;
    M.a20=c.u.z;  M.a21=c.v.z; M.a22=c.w.z; M.a23=0.0f;
    M.a30=0.0f;  M.a31=0.0f; M.a32=0.0f; M.a33=1.0f;
    return M;
}

mat4 Transform::scaleNormalsMatrix(float x, float y, float z) {
        mat4 sInv;
        sInv.a00=1.0f/x; sInv.a01=0; sInv.a02=0; sInv.a03=0;
        sInv.a10=0; sInv.a11=1.0f/y; sInv.a12=0; sInv.a13=0;
        sInv.a20=0; sInv.a21=0; sInv.a22=1.0f/z; sInv.a23=0;
        sInv.a30=0; sInv.a31=0; sInv.a32=0; sInv.a33 = 1;
        return sInv;
}

void Transform::loadIdentity() {
    M.a00=1.0f;  M.a01=0.0f; M.a02=0.0f; M.a03=0.0f;
    M.a10=0.0f;  M.a11=1.0f; M.a12=0.0f; M.a13=0.0f;
    M.a20=0.0f;  M.a21=0.0f; M.a22=1.0f; M.a23=0.0f;
    M.a30=0.0f;  M.a31=0.0f; M.a32=0.0f; M.a33=1.0f;
}

mat4 Transform::identity() {
    mat4 M;
    M.a00=1.0f;  M.a01=0.0f; M.a02=0.0f; M.a03=0.0f;
    M.a10=0.0f;  M.a11=1.0f; M.a12=0.0f; M.a13=0.0f;
    M.a20=0.0f;  M.a21=0.0f; M.a22=1.0f; M.a23=0.0f;
    M.a30=0.0f;  M.a31=0.0f; M.a32=0.0f; M.a33=1.0f;
    return M;
}

mat4 Transform::noTrans(mat4 M) {
    mat4 nT = M;
    nT.a03=0.0f;
    nT.a13=0.0f;
    nT.a23=0.0f;
    return M;
}

mat4 Transform::invTranspose(mat4 M) {
    mat4 inv = invert(inv, M);
    mat4 invT=transpose(invT, inv);
    return invT;
}

mat4 Transform::invTransposeForNormals(mat4 M) {
    mat4 inv = invert(inv, noTrans(M));
    mat4 invT=transpose(invT, inv);
    return invT;
}

Point Transform::pos() { return Point(M.a03, M.a13, M.a23); }

