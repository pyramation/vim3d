#include <math.h>
#include <stdio.h>
#include "Channel.h"
#include "Scene.h"

float Channel::globalMaxY=0.0f;
float Channel::globalMinY=0.0f;

Channel::Channel() {
    start = scopeStart = 0;
    end = scopeEnd = 240;
    enabled = true;
    active = false;
    sampleRate = 30.0f;
    filepath = String();
    name = Scene::name(String("Chan"));
    points = vector<Point>();
    type = CHANNEL_TYPE;
}

Channel::Channel(String filepath) {
    this->filepath = filepath;
    start = scopeStart = 0;
    end = scopeEnd = 240;
    enabled = true;
    active = false;
    sampleRate = 30.0f;
    name = Scene::name(String("Chan"));
    points = vector<Point>();
    type = CHANNEL_TYPE;

    FILE * fd = fopen(filepath.c_str(), "r");
    if(!fd)return;
    char s[1024];
    float x,y;
    while((fgets(s, 1024, fd)) != NULL) {
        sscanf(s, "%f %f", &x,&y);
        points.push_back(Point(x,y));
    }
    fclose(fd);
    setBounds();
}

Channel::Channel(const Channel &ch) {
    filepath = ch.filepath;
    start = ch.start;
    scopeStart = ch.scopeStart;
    end = ch.end;
    scopeEnd = ch.scopeEnd;
    enabled = ch.enabled;
    active = ch.active;
    sampleRate = ch.sampleRate;
    name = Scene::name(String("Chan"));
    points = ch.points;
    type = CHANNEL_TYPE;
    maxX = ch.maxX;
    minX = ch.minX;
    maxY = ch.maxY;
    minY = ch.minY;
    maxZ = ch.maxZ;
    minZ = ch.minZ;
}

Channel Channel::convolve(const Channel * X, const Channel * Y) {
    const Channel & ch = *X;
    Channel combined(ch);
    combined.points.clear();
    for(unsigned int k=0; k<(CP(X).size()+CP(Y).size()-1); k++) {
        for(unsigned int n=0; n<CP(Y).size(); n++) {
            float kTerm, nkTerm;
            if (k>0&&k<CP(X).size()) {
                kTerm=CP(X)[k].y;
            } else kTerm=0.0;

            if (n-k>0&&n-k < CP(Y).size()) {
                nkTerm=CP(Y)[n-k].y;
            } else nkTerm=0.0;
            float add = (k>1) ? combined.points[k-1].y : 0.0f;
            combined.points.push_back(Point(float(k), add+kTerm*nkTerm));
        }
    }
    return combined;
}

Channel Channel::combine(const Channel * X, const Channel * Y) {
    const Channel & ch = *X;
    Channel combined(ch);
    combined.points.clear();
    for(unsigned int i=0; ( i<X->points.size() && i<Y->points.size()); i++) {
        combined.points.push_back(Point( X->points[i].y,Y->points[i].y));
    }
    return combined;
}

Channel Channel::combine(const Channel * X, const Channel * Y, const Channel * Z) {
    const Channel & ch = *X;
    Channel combined(ch);
    combined.points.clear();
    for(unsigned int i=0; ( i<X->points.size() && i<Y->points.size() && i<Z->points.size()); i++) {
        combined.points.push_back(Point( X->points[i].y,Y->points[i].y, Z->points[i].y));
    }
    return combined;
}

void Channel::Sin() {
    points.clear();
    enabled = true;
    for(float t=start; t<=start+2*3.14; t += 1.0f/sampleRate) {
        Point f(t, sin(t));
        points.push_back(f);
    }
    setBounds();
}

void Channel::Cos() {
    points.clear();
    enabled = true;
    for(float t=start; t<=start+2*3.14; t += 1.0f/sampleRate) {
        Point f(t, cos(t));
        points.push_back(f);
    }
    setBounds();
}

void Channel::setBounds() {
    if (points.size() <= 0) return;
    bool init=false;
    for(vector<Point>::iterator cursor=points.begin();
            cursor!=points.end(); cursor++) {
        Point p = *cursor;
        if (init) {
            if (p.x < minX) minX = p.x;
            if (p.x > maxX) maxX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.y > maxY) maxY = p.y;
            if (p.z < minZ) minZ = p.z;
            if (p.z > maxZ) maxZ = p.z;

            if (p.y < globalMinY) globalMinY = p.y;
            if (p.y > globalMaxY) globalMaxY = p.y;

        } else {
            maxX = minX = p.x;
            maxY = minY = p.y;
            maxZ = minZ = p.z;
            init = true;

            if (p.y < globalMinY) globalMinY = p.y;
            if (p.y > globalMaxY) globalMaxY = p.y;
        }
    }
}
float min(float a, float b) { return (a<b)?a:b; }
float max(float a, float b) { return (a>b)?a:b; }
void Channel::display(int x, int y, int w, int h) {
    if (points.size() > 0) {
        glBegin(GL_LINES);
        for(unsigned int i=0; i<points.size()-1; i++) {
            Point p = *(points.begin() + i);
            Point p2 = *(points.begin() + i + 1);
            float nx = Point::fit(p.x, minX, maxX, float(x), float(x+w));
            float ny = Point::fit(p.y, minY,maxY,float(y),float(y+h));
            float nx2 = Point::fit(p2.x, minX, maxX, float(x), float(x+w));
            float ny2 = Point::fit(p2.y, minY,maxY,float(y),float(y+h));

            /* break out scope */
            if (nx > maxX) break;

            glVertex2f(nx, ny);
            glVertex2f(nx2, ny2);
        }
        glEnd();
    }
}


/*
   void Channel::display(int x, int y, int w, int h, int begin, float amount) {
   if (points.size() > 0) {
   glBegin(GL_LINES);

   bool useGlobal = false;

   for(unsigned int i=begin; i<points.size()-1; i++) {
   float nx = Point::fit(points[i].x, minX,max(0,min(maxX,amount*maxX)),float(x),float(x+w));
   float ny = Point::fit(points[i].y, minY,maxY,float(y),float(y+h));
   float nx2 = Point::fit(points[i+1].x, minX,max(0,min(maxX,amount*maxX)),float(x),float(x+w));
   float ny2 = Point::fit(points[i+1].y, minY,maxY,float(y),float(y+h));
   if (nx > amount*maxX) break;

   glVertex2f(nx, ny);
   glVertex2f(nx2, ny2);
   }
   glEnd();
   }
   }
   */
void Channel::display(int x, int y, int w, int h, int begin, float amountx, float amounty) {
    if (points.size() > 0) {
        glBegin(GL_LINES);

        for(unsigned int i=begin; i<points.size()-1; i++) {
            //float nx = Point::fit(points[i].x, minX,max(0,min(maxX,amount*maxX)),float(x),float(x+w));
            float nx = Point::fit(points[i].x, minX,w*amountx,float(x),float(x+w));
            float ny = Point::fit(points[i].y, minY,maxY/amounty,float(y),float(y+h));
            //float nx2 = Point::fit(points[i+1].x, minX,max(0,min(maxX,amount*maxX)),float(x),float(x+w));
            float nx2 = Point::fit(points[i+1].x, minX,w*amountx,float(x),float(x+w));
            float ny2 = Point::fit(points[i+1].y, minY,maxY/amounty,float(y),float(y+h));

            /* break out scope */
            //    if (nx > amount*maxX) break;

            glVertex2f(nx, ny);
            glVertex2f(nx2, ny2);
        }
        glEnd();
    }
}
void Channel::WriteCHAN(String filename) {
    FILE * fd = fopen(filename.c_str(), "w");
    if (!fd) return;
    for(unsigned int i=0;i<points.size();i++) {
        fprintf(fd, "%f %f\n", points[i].x, points[i].y);
    }    
    fclose(fd);
}

Channel * Channel::differentiate() {
    Channel * chan = new Channel();
    float small = 0.000000001;
    for(unsigned int i=0;i<points.size()-1;i++) {
        if (points[i+1].x - points[i].x != 0) chan->points.push_back(Point(points[i].x, (points[i+1].y - points[i].y)/(points[i+1].x - points[i].x)));
        else chan->points.push_back(Point(points[i].x, (points[i+1].y - points[i].y)/small));

        // steal last point
        if (i == points.size()-2) {
            if (points[i+1].x - points[i].x != 0) chan->points.push_back(Point(points[i+1].x, (points[i+1].y - points[i].y)/(points[i+1].x - points[i].x)));
            else chan->points.push_back(Point(points[i+1].x, (points[i+1].y - points[i].y)/small));
        }
    }    
    chan->setBounds();
    return chan;
}

Channel Channel::project(Channel * channel) {
    Channel ch(*this);
    ch.name = Scene::name("op");
    for(unsigned int i=0; (i<ch.points.size() && i<channel->points.size());i++) {
        ch.points[i] = points[i].project(channel->points[i]);
    } 
    ch.setBounds();
    return ch;
}

Channel Channel::operator - (const Channel & C) const {
    Channel ch(C);
    ch.name = Scene::name("op");
    for(unsigned int i=0; (i<ch.points.size() && i<points.size());i++) {
        ch.points[i].y -= points[i].y;
    } 
    ch.setBounds();
    return ch;
}

Channel Channel::operator + (const Channel & C) const {
    Channel ch(C);
    ch.name = Scene::name("op");
    for(unsigned int i=0; (i<ch.points.size() && i<points.size());i++) {
        ch.points[i].y += points[i].y;
    } 
    ch.setBounds();
    return ch;
}

Channel Channel::operator / (const Channel & C) const {
    Channel ch(C);
    ch.name = Scene::name("op");
    for(unsigned int i=0; (i<ch.points.size() && i<points.size());i++) {
        ch.points[i].y /= points[i].y;
    } 
    ch.setBounds();
    return ch;
}

Channel Channel::operator * (const Channel & C) const {
    Channel ch(C);
    ch.name = Scene::name("op");
    for(unsigned int i=0; (i<ch.points.size() && i<points.size());i++) {
        ch.points[i].y *= points[i].y;
    } 
    ch.setBounds();
    return ch;
}

Channel Channel::operator * (const float &f) const {
    Channel ch(*this);
    ch.name = this->name;
    for(unsigned int i=0; i<ch.points.size();i++) {
        ch.points[i].y *= f;
    } 
    ch.setBounds();
    return ch;
}
Channel Channel::operator / (const float &f) const {
    if (f == 0) return Channel(*this);
    Channel ch(*this);
    ch.name = this->name;
    for(unsigned int i=0; i<ch.points.size();i++) {
        ch.points[i].y /= f;
    } 
    ch.setBounds();
    return ch;
}
Channel Channel::operator + (const float &f) const {
    Channel ch(*this);
    ch.name = this->name;
    for(unsigned int i=0; i<ch.points.size();i++) {
        ch.points[i].y += f;
    } 
    ch.setBounds();
    return ch;
}
Channel Channel::operator - (const float &f) const {
    Channel ch(*this);
    ch.name = this->name;
    for(unsigned int i=0; i<ch.points.size();i++) {
        ch.points[i].y -= f;
    } 
    ch.setBounds();
    return ch;
}

String Channel::propertyInfo() {
    char s[1024];
    sprintf(s, "name: %s\nstart: %d end: %d\nscopeStart: %d scopeEnd: %d\nsampleRate: %f\nmin(%f,%f,%f) max(%f,%f,%f)\n",
            name.c_str(), start, end, scopeStart, scopeEnd, sampleRate, 
            minX,minY,minZ, maxX,maxY,maxZ);
    String n(s);
    return n;
}


Channel::~Channel() { printf("deleting channel %s\n", name.c_str());}


