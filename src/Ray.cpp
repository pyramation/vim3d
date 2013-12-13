#include "Ray.h"
#include "Scene.h"

Ray::Ray(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = float(x);
    winY = float(viewport[3]) - float(y);
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    Point hit = Point();
    Point end = Point(posX, posY, posZ);
    intersectPlaneNoBounds( end, SceneCAM->eye, Point::normalize(end-SceneCAM->eye), end, &hit);
    
    pos = SceneCAM->eye;
    dir = hit - pos;
    dir.normalize();
}

Ray::Ray(Object * from, Object * to) {

    if (isCamera(from)) pos = C(from)->eye;
    else pos = to->pos();

    dir = to->pos() - pos;

}

int Ray::intersectSphere( Ray ray, float sphereScale, Point spherePosition, Point * I) {
    Point viewPlanePoint = ray.pos+ray.dir;
    Point P1 = ray.dir; // line direction, cam a -> viewPlane pt. b (m)
    Point Po = ray.pos; // line (c)
    float c = Point::dot(Po-spherePosition, Po-spherePosition) - sphereScale*sphereScale;
    float b = 2 * Point::dot(P1, Po - spherePosition);
    float a = Point::dot(P1, P1);
    float discriminant = b*b - 4*a*c;
    if (discriminant<0) {
        return 0;
    }
    discriminant = sqrt(discriminant);
    float t1 = (-b + discriminant)/(2*a);
    float t2 = (-b - discriminant)/(2*a);

    if (t2<0) {
        if(t1<0){
            return 0;
        } else {
            *I = P1*t1 + Po;
            return 1;
        }
    }
    if (t1<t2&&t1>0) {
        *I = P1*t1 + Po;
        return 1;
    } else{
        *I = P1*t2 + Po;
        return 1;
    }
}

int Ray::intersectTriangle(Ray ray, vector <Vertex> triangle, Point * I, Point * coords){

    Point u, v, n, w, i;
	
    //get triangle 'basis'
    if (triangle.size()!=3) {
        return 0;
    }
    u = triangle[1] - triangle[0];
    v = triangle[2] - triangle[0];
    n = Point::cross(u, v);
	
    if (n.length()==0) {
        return 0;
    }
    float a, b, c;
    a = -Point::dot(n, ray.pos-triangle[0]); 
    b = Point::dot(n, ray.dir);
    if (b==0) {
        return 0;
    }
    c = a/b;
	
    if( c<0.0 ) return 0;
    // I = intersection point of line and plane of triangle
    *I  = ray.pos + (ray.dir * c);
	
    //distance to intersection point from triangle /origin/ => to
    Point to = *I - triangle[0]; 
    float duu, duv, dvv, dtu, dtv;
    duu = Point::dot(u,u);
    duv = Point::dot(u,v);
    dvv = Point::dot(v,v);
    dtu = Point::dot(to,u);
    dtv = Point::dot(to, v);
    float d = duv*duv - duu * dvv;
	
    float s = (duv*dtv - dvv*dtu)/d;
    if(s<0.0||s>1.0) return 0;
	
    float t = (duv*dtu - duu * dtv)/d;
    if(t<0.0||(s+t)>1.0) return 0;
	
	coords->x = t;
	coords->y = s;
	coords->z = 1.0-s-t;
	
    return 1;
}

int Ray::intersectRectangle(Ray ray, vector <Vertex> rectangle, Point * I, Point * coords){
    vector <Vertex> testTri1;
    testTri1.push_back(rectangle[0]);
    testTri1.push_back(rectangle[1]);
    testTri1.push_back(rectangle[2]);
	
    if (intersectTriangle(ray, testTri1, I, coords)) { return 2; }
	
    vector <Vertex> testTri2;
    testTri2.push_back(rectangle[2]);
    testTri2.push_back(rectangle[3]);
    testTri2.push_back(rectangle[0]);	
	
    if (intersectTriangle(ray, testTri2, I, coords)) { return 3; }
    return 0;
}


// min and max represent the minimum and maximum bounds
int Ray::intersectBox(Ray ray, Point min, Point max, Point * hit) {
    float t1, t2;
    float tFar = FLT_MAX;
    float tNear = FLT_MIN;

    // calculate intersection distances of planes
    if (ray.dir.x == 0) {
        // ray is parallel to planes
        if (ray.pos.x < min.x || ray.pos.x > max.x) return 0;
    } else {
        t1 = (min.x - ray.pos.x)/ray.dir.x;
        t2 = (max.x - ray.pos.x)/ray.dir.x;
        if (t1 > t2) {
            float tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        if (t1 > tNear) tNear = t1;
        if (t2 < tFar) tFar = t2;
        if (tNear > tFar) return 0; // box is missed
        if (tFar < 0) return 0; // box is behind ray

    }
    // calculate intersection distances of planes
    if (ray.dir.y == 0) {
        // ray is parallel to planes
        if (ray.pos.y < min.y || ray.pos.y > max.y) return 0;
    } else {
        t1 = (min.y - ray.pos.y)/ray.dir.y;
        t2 = (max.y - ray.pos.y)/ray.dir.y;
        if (t1 > t2) {
            float tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        if (t1 > tNear) tNear = t1;
        if (t2 < tFar) tFar = t2;
        if (tNear > tFar) return 0; // box is missed
        if (tFar < 0) return 0; // box is behind ray

    }
    // calculate intersection distances of planes
    if (ray.dir.z == 0) {
        // ray is parallel to planes
        if (ray.pos.z < min.z || ray.pos.z > max.z) return 0;
    } else {
        t1 = (min.z - ray.pos.z)/ray.dir.z;
        t2 = (max.z - ray.pos.z)/ray.dir.z;
        if (t1 > t2) {
            float tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        if (t1 > tNear) tNear = t1;
        if (t2 < tFar) tFar = t2;
        if (tNear > tFar) return 0; // box is missed
        if (tFar < 0) return 0; // box is behind ray
    }
    ray.dir.normalize();
    *hit = ray.pos + ray.dir * tNear;
    return 1;
}

void Ray::intersectPlaneNoBounds( Point a, Point b, Point planeNormal, Point planePosition, Point * hit) {
    a = a-b;
    float p1 = Point::dot(a, planeNormal);
    float p0 = Point::dot(b, planeNormal);
    float pA = Point::dot(planePosition, planeNormal);
    float t = (-p0 + pA)/p1;
    *hit = Point(b.x + a.x*t,b.y + a.y*t,b.z + a.z*t);
}

Ray::Ray() {}

Ray::~Ray() {}

