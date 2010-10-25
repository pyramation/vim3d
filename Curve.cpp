#include "Curve.h"
#include "stdio.h"


/* return a curve that is the effect of the tranformation of M */
Curve* Curve::curveM(mat4 matM){
    Curve * rstCurve = new Curve();

    point_t pv;
    point_t::iterator cursor;
    for (cursor = points.begin(); cursor != points.end(); cursor++) {
        Point p = *cursor;
        p = p*matM;
        rstCurve->points.push_back(p);
    }
    return rstCurve;
}


point_t Curve::rt(float theta, float x, float y, float z, float tx, float ty, float tz, float cenX, float cenY, float cenZ) {
    Point v = Point(x,y,z);
    v.normalize();
    Point q(x, y, z);
    mat3 R = Transform::rotateMat(theta, q);
    point_t pv;
    point_t::iterator cursor;
    if (points.size() > 1) {
        for (cursor = points.begin(); cursor < points.end(); cursor++) {
            Point p = *cursor;
            p.x = p.x - cenX;
            p.y = p.y - cenY;
            p.z = p.z - cenZ;			

            p = p*R;

            p.x = p.x + cenX + tx;
            p.y = p.y + cenY + ty;
            p.z = p.z + cenZ + tz;			
            pv.push_back(p);
        }
    }
    return pv;
}

void Curve::rtMod(float theta, float x, float y, float z, float tx, float ty, float tz, float cenX, float cenY, float cenZ) {
    Point q(x, y, z);
    mat3 rot = Transform::rotateMat(theta, q);
    for(unsigned int i=0;i<points.size();i++) {

        points[i].x -= cenX;
        points[i].y -= cenY;
        points[i].z -= cenZ;

        points[i] = points[i]*rot;

        points[i].x += (tx+cenX);
        points[i].y += (ty+cenY);
        points[i].z += (tz+cenZ);
    }
}


point_t Curve::rotate(float degrees, float x, float y, float z) {
    degrees = 3.14 / 180 * degrees;
    Point v = Point(x,y,z);
    v.normalize();
    point_t pv;
    point_t::iterator cursor;
    if (points.size() > 1) {
        for (cursor = points.begin(); cursor != points.end(); cursor++) {
            Point p = *cursor;
            Point v0(p.x, p.y, p.z);
            Point v1(p.x*cos(degrees), p.y*cos(degrees), p.z*cos(degrees));
            Point v2 = Point::cross(v, v0);
            v2.x*=sin(degrees);v2.y*=sin(degrees);v2.z*=sin(degrees);
            float t = Point::dot(v,v0);
            Point v3 = v;
            v3.x *= t;v3.y*=t;v3.z*=t;
            v3.x *= (1-cos(degrees));v3.y*=(1-cos(degrees));v3.z*=(1-cos(degrees));

            Point pvv = v1 + v2 + v3;

            p.x = pvv.x;
            p.y = pvv.y;
            p.z = pvv.z;
            pv.push_back(p);
        }
    }
    return pv;
}
point_t Curve::translate(float x, float y, float z) {
    point_t pv;
    point_t::iterator cursor;
    if (points.size() > 1) {
        for (cursor = points.begin(); cursor < points.end(); cursor++) {
            Point p = *cursor;
            p.x += x;
            p.y += y;
            p.z += z;
            pv.push_back(p);
        }
    }
    return pv;
}

void Curve::addPoint( Point a ) {
    if (!isactive) points.push_back(Point(a.x, a.y, a.z));
}

void Curve::deleteActivePoint() {
    if (isactive) {
        points.erase(activePoint);
        isactive = false ;
    }
}

void Curve::moveActivePoint(float dx, float dy, float dz) {
    if (isactive) {
        activePoint->x += dx;
        activePoint->y += dy;
        activePoint->z += dz;
    }
}

void Curve::moveActivePointTo(Point a) {
    if (isactive) {
        activePoint->x = a.x;
        activePoint->y = a.y;
        activePoint->z = a.z;
    }
}

void Curve::updateActivePoint(float x, float y, float z) {
    float dx, dy, dz, dist2;
    float radius = Point::ACTIVE_RADIUS*Point::ACTIVE_RADIUS;
    dist2 = Point::ACTIVE_RADIUS+1;
    if (isactive) {
        dx = activePoint->x - x;
        dy = activePoint->y - y;
        dz = activePoint->z - z;
        dist2 = dx*dx + dy*dy + dz*dz;
    }
    if (dist2 > radius) {
        if (isactive) activePoint->active = false;
        isactive = false ; 
        for (point_t::iterator it = points.begin(); it != points.end(); it++) {
            dx = it->x - x;
            dy = it->y - y;
            dz = it->z - z;
            dist2 = dx*dx + dy*dy + dz*dz;
            if (dist2 < radius) {
                activePoint = it;
                it->active = true;
                isactive = true ; 
                lastActive = *it;
                break;
            }
        }
    }
}

Curve::Curve()
{
    isactive = false ;
}

Curve::Curve( vector <Point> pts ) {
    points = pts;
    /*
       vector <Point>::iterator cursor;
       for(cursor = pts.begin(); cursor!=pts.end(); cursor++) {
       Point p = *cursor;
       points.push_back(p);
       }
     */
}

void Curve::axis(){
    float scale = 10.0;
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
    glColor3f(1.0,1.0,1.0);
}

void Curve::EditHandle() {
    if (points.size() > 0) {
//        if (points.begin() <= activePoint && activePoint <= points.end()) {
            if (isactive) {
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glTranslatef(activePoint->x, activePoint->y, activePoint->z);
                axis();
                glPopMatrix();
            }
 //       } else printf("activePoint is no good\n");
    }
}

void Curve::printCurve() {
    if (points.size() > 0) {
        point_t::iterator a, b;
        glBegin( GL_LINES );
        for(a = points.begin(); a != points.end()-1; a++) {
            b = a + 1;
            glVertex3f( a->x, a->y, a->z );
            glVertex3f( b->x, b->y, b->z );
        }
        glEnd();
    }
}

void Curve::printPoints() {
    if (points.size() > 0) {
        point_t::iterator a;
/*
           for(a = points.begin(); a != points.end(); a++) {
           glPushMatrix();
           glTranslatef( a->x, a->y, a->z );
           glutWireSphere(1,10,10);
           glPopMatrix();
           }
*/
        glPointSize(9.0f);
        glBegin( GL_POINTS );
        for(a = points.begin(); a != points.end(); a++) {
            glVertex3f( a->x, a->y, a->z );
        }
        glEnd();
        glPointSize(1.0f);
    }
}

void Curve::printCurve(point_t p) {
    if (p.size() > 0) {
        point_t::iterator a, b, activeP;
        glBegin( GL_LINES );
        for(a = p.begin(); a != p.end()-1; a++) {
            b = a + 1;
            glVertex3f( a->x, a->y, a->z );
            glVertex3f( b->x, b->y, b->z );
        }
        glEnd();
    }
}
Point lerp(Point p, Point n, float t) {
    Point a = Point(0,0,0);
    a.x = p.x + (n.x - p.x)*t;
    a.y = p.y + (n.y - p.y)*t;
    a.z = p.z + (n.z - p.z)*t;
    return a;
}

Point deCasteljau(point_t input, float u) {
    point_t pv = point_t();
    if (!input.size()) return Point(0.0, 0.0);
    if (input.size() == 1) return *(input.begin()); 
    for(point_t::iterator cursor = input.begin(); cursor < input.end()-1; cursor++) {
        Point pt1 = *cursor; 
        Point pt2 = *(cursor+1);
        pv.push_back(lerp(pt1, pt2, u));
    }
    return deCasteljau(pv, u);
}


point_t Curve::Bezier(int levelOfDetail) {
    point_t pv;
    int i=0; 
    for(float u=0; u<=1; u += 1.0 / float(levelOfDetail), i++) {
        Point a = deCasteljau(points,u);
        pv.push_back(a);
    }
    printCurve(pv);
    return pv;
}

float matrixBS(float p0, float p1, float p2, float p3, float u) {
    float u2 = u*u;
    float u3 = u*u*u;
    return 1.0/6.0 * (p0*(-u3+3*u2-3*u+1)+
            p1*(3*u3-6*u2+4)+
            p2*(-3*u3+3*u2+3*u+1)+
            p3*u3);
}

void drawSeg(point_t::iterator p1, point_t::iterator p2, point_t::iterator p3, point_t::iterator p4, int levelOfDetail, point_t &pv) {

    float x, y, z;

    //draw segment
    for(float u=0.0; u<=1.0; u+= 1.0/float(levelOfDetail)) {
        x = matrixBS( p1->x, p2->x, p3->x, p4->x, u);
        y = matrixBS( p1->y, p2->y, p3->y, p4->y, u);
        z = matrixBS( p1->z, p2->z, p3->z, p4->z, u);
        pv.push_back( Point(x,y,z) );
    }
}

point_t Curve::Bspline(int levelOfDetail) {

    point_t pv;
    if (points.size() < 4) return pv;
    for(point_t::iterator cursor = points.begin(); cursor < points.end()-3; cursor++) {
        drawSeg(cursor, cursor+1, cursor+2, cursor+3, levelOfDetail, pv);
    }
    printCurve(pv);
    return pv;
}

void Curve::noInterpolation() {printCurve();}

void accumulateNextLevel(point_t* accum, point_t pts) {
    if (pts.empty()) return; 
    accum->push_back(*(pts.begin()));
    if (pts.size() == 1) return;
    point_t newv;
    for (point_t::iterator it = pts.begin(); it != pts.end() - 1; it++) {
        Point pt1 = *it; 
        Point pt2 = *(it+1);
        newv.push_back(lerp(pt1, pt2, 0.5));
    }
    //save the last point
    Point last = *(pts.end()-1);
    pts.pop_back();
    //recursive call
    accumulateNextLevel(accum, newv);
    accum->push_back(last);
}


// The basic draw function for Bezier2.  Note that as opposed to Bezier, 
// this draws the curve by recursive subdivision.  So, levelofdetail 
// corresponds to how many times to recurse.  
point_t Curve::Bezier2(int levelOfDetail) {
    //This is just a trick to find out if this is the top level call
    //All recursive calls will be given a negative integer, to be flipped here
    point_t newP;
    if (levelOfDetail <= 0) {
        levelOfDetail = -levelOfDetail;
    }

    //Base case.  No more recursive calls.
    if (levelOfDetail <= 1) {
        if (points.size() >= 2) {
            for (point_t::iterator it = points.begin(); it != points.end(); it++) {
                newP.push_back(*it);
            }
        }
    } else {
        point_t* accum = new point_t();
        Curve left, right;

        accumulateNextLevel(accum, points);

        int count = 0;
        int goal = accum->size() / 2;
        for (point_t::iterator a = accum->begin(); a != accum->end(); a++) {
            if (count < goal) {
                left.points.push_back( *a );	
            } else if (count > goal) {
                right.points.push_back( *a );
            } else {
                left.points.push_back( *a );	
                right.points.push_back( *a );		
            }
            count++;
        }

        //accumulateNextLevel(accum, points);
        //Point a = deCasteljau(pts,0.5);

        point_t pta = left.Bezier2(1-levelOfDetail);
        point_t ptb = right.Bezier2(1-levelOfDetail);
        point_t::iterator cursor;

        for (cursor = pta.begin(); cursor!= pta.end(); cursor++) 
            newP.push_back(*cursor);
        for (cursor = ptb.begin(); cursor!= ptb.end(); cursor++) 
            newP.push_back(*cursor);

        delete accum;
    }
    return newP;
}

Curve::~Curve(){}
