#include "BoundingBox.h"
#include "Scene.h"

BoundingBox::BoundingBox() { initialize(); }

void BoundingBox::initialize() {
    init = false;
    centroid.x = centroid.y = centroid.z = 0;
    centroid.x=centroid.y=centroid.z=max.x=max.y=max.z=min.x=min.y=min.z=0.0f;
    num_pts = 0;
}

void BoundingBox::Bound(BoundingBox * box) {

    if (!init) {
        min.x = min.y = min.z = FLT_MAX;
        max.x = max.y = max.z = FLT_MIN;
        init = true;
    }

    vector<Point>pts=vector<Point>();
    pts.push_back(Point(box->min.x, box->min.y, box->min.z));
    pts.push_back(Point(box->min.x, box->min.y, box->max.z));
    pts.push_back(Point(box->max.x, box->min.y, box->max.z));
    pts.push_back(Point(box->min.x, box->max.y, box->max.z));
    pts.push_back(Point(box->max.x, box->max.y, box->max.z));
    pts.push_back(Point(box->max.x, box->min.y, box->max.z));
    pts.push_back(Point(box->max.x, box->max.y, box->min.z));
    pts.push_back(Point(box->min.x, box->max.y, box->min.z));

    num_pts += box->num_pts;
    centroid.x = (centroid.x+box->centroid.x)/2.0;
    centroid.y = (centroid.y+box->centroid.y)/2.0;
    centroid.z = (centroid.z+box->centroid.z)/2.0;

    for(unsigned int i=0;i<pts.size();i++) {
        if (pts[i].x < min.x) min.x = pts[i].x;
        if (pts[i].x > max.x) max.x = pts[i].x;
        if (pts[i].y < min.y) min.y = pts[i].y;
        if (pts[i].y > max.y) max.y = pts[i].y;
        if (pts[i].z < min.z) min.z = pts[i].z;
        if (pts[i].z > max.z) max.z = pts[i].z;
    }

}

void BoundingBox::Bound(BoundingBox * box, mat4 M) {

    if (!init) {
        min.x = min.y = min.z = FLT_MAX;
        max.x = max.y = max.z = FLT_MIN;
        init = true;
    }

    vector<Point>pts=vector<Point>();
    pts.push_back(Point(box->min.x, box->min.y, box->min.z));
    pts.push_back(Point(box->min.x, box->min.y, box->max.z));
    pts.push_back(Point(box->max.x, box->min.y, box->max.z));
    pts.push_back(Point(box->min.x, box->max.y, box->max.z));
    pts.push_back(Point(box->max.x, box->max.y, box->max.z));
    pts.push_back(Point(box->max.x, box->min.y, box->max.z));
    pts.push_back(Point(box->max.x, box->max.y, box->min.z));
    pts.push_back(Point(box->min.x, box->max.y, box->min.z));

    num_pts += box->num_pts;
    centroid.x = (centroid.x+box->centroid.x)/2.0;
    centroid.y = (centroid.y+box->centroid.y)/2.0;
    centroid.z = (centroid.z+box->centroid.z)/2.0;

    for(unsigned int i=0;i<pts.size();i++) {
        pts[i] = pts[i]*M;
    }

    for(unsigned int i=0;i<pts.size();i++) {
        if (pts[i].x < min.x) min.x = pts[i].x;
        if (pts[i].x > max.x) max.x = pts[i].x;
        if (pts[i].y < min.y) min.y = pts[i].y;
        if (pts[i].y > max.y) max.y = pts[i].y;
        if (pts[i].z < min.z) min.z = pts[i].z;
        if (pts[i].z > max.z) max.z = pts[i].z;
    }

}

void BoundingBox::Bound(vector<Point>&pts) {

    if (!init) {
        min.x = min.y = min.z = FLT_MAX;
        max.x = max.y = max.z = FLT_MIN;
        init = true;
    }

    num_pts += pts.size();
    for(unsigned int i=0; i<pts.size(); i++) {
        centroid.x += pts[i].x/float(num_pts);
        centroid.y += pts[i].y/float(num_pts);
        centroid.z += pts[i].z/float(num_pts);

        if (pts[i].x < min.x) min.x = pts[i].x;
        if (pts[i].x > max.x) max.x = pts[i].x;
        if (pts[i].y < min.y) min.y = pts[i].y;
        if (pts[i].y > max.y) max.y = pts[i].y;
        if (pts[i].z < min.z) min.z = pts[i].z;
        if (pts[i].z > max.z) max.z = pts[i].z;
    }
}

void BoundingBox::display() {
    Scene::toggleLights(false);
    glBegin(GL_LINES);
    glVertex3f(min.x, min.y, min.z); glVertex3f(max.x, min.y, min.z);
    glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, max.y, min.z);
    glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, min.y, max.z);
    glVertex3f(min.x, min.y, max.z); glVertex3f(min.x, max.y, max.z);
    glVertex3f(min.x, min.y, max.z); glVertex3f(max.x, min.y, max.z);
    glVertex3f(max.x, min.y, max.z); glVertex3f(max.x, min.y, min.z);
    glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, max.y, max.z);
    glVertex3f(max.x, max.y, min.z); glVertex3f(min.x, max.y, min.z);
    glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, max.y, max.z);
    glVertex3f(min.x, max.y, max.z); glVertex3f(max.x, max.y, max.z);
    glVertex3f(max.x, max.y, max.z); glVertex3f(max.x, min.y, max.z);
    glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, min.y, min.z);
    glEnd();
    Scene::toggleLights(true);
}

BoundingBox::~BoundingBox() {}

