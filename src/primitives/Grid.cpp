#include <math.h>
#include "Grid.h"

Grid::Grid(){
    geoType = GRID_GEO;
    segmentSize=1;
    dx=dz=1;
    tx=tz=0;
    p = new Poly();
    Point p1(-100.0f,-100.0f, 0.0f); 
    Point p2(-100.0f, 100.0f, 0.0f); 
    Point p3( 100.0f, 100.0f, 0.0f); 
    Point p4( 100.0f,-100.0f, 0.0f); 
    p->addFace(p1, p2, p3, p4);
    p->computeNormals();
    p->bound();
    this->bound();
    this->addPoly(p);
}

Grid::Grid(float w, float h) {
    segmentSize=1;
    dx=dz=1;
    tx=tz=0;
    geoType = GRID_GEO;
    float minX = -w/2;
    float minY = -h/2;
    float maxX =  w/2;
    float maxY =  h/2;
    p = new Poly();
    Point p1(minX,minY, 0.0f); 
    Point p2(minX,maxY, 0.0f); 
    Point p3(maxX,maxY, 0.0f); 
    Point p4(maxX,minY, 0.0f); 
    p->addFace(p1, p2, p3, p4);
    p->computeNormals();
    p->bound();
    this->bound();
    this->addPoly(p);
}

Grid::Grid(float minX, float minY, float maxX, float maxY) {
    segmentSize=1;
    dx=dz=1;
    tx=tz=0;
    geoType = GRID_GEO;
    p = new Poly();
    Point p1(minX,minY, 0.0f); 
    Point p2(minX,maxY, 0.0f); 
    Point p3(maxX,maxY, 0.0f); 
    Point p4(maxX,minY, 0.0f); 
    p->addFace(p1, p2, p3, p4);
    p->computeNormals();
    p->bound();
    this->bound();
    this->addPoly(p);
}

Grid::Grid(int divisions) {
    geoType = GRID_GEO;
    this->segmentSize = 10.0f;
    this->dx = divisions;
    this->dz = divisions;
    this->tx = 0.0f;
    this->tz = 0.0f;
    p = new Poly();
    for(int i=0; i<divisions;i++) {
        for(int j=0;j<divisions;j++) {
            p->addVertex(Vertex(i*segmentSize,0,j*segmentSize));
            p->addTexCoord(TexCoord(float(i)/float(divisions),float(j)/float(divisions)));
        }
    }
    for(int i=0; i<divisions*(divisions-1);i++) {
            if (!(i%divisions))continue;
            Face f = Face();
            f.vi.push_back(i);  
            f.vi.push_back(i+1);  
            f.vi.push_back(i+1+divisions);  
            f.vi.push_back(i+divisions);  
            f.ti.push_back(i);  
            f.ti.push_back(i+1);  
            f.ti.push_back(i+1+divisions);  
            f.ti.push_back(i+divisions);  
            p->addFace(f);
    }
    p->computeNormals();
    p->centroid();
    p->bound();
    this->bound();
    this->addPoly(p);
}

Grid::Grid(int divisionsX, int divisionsZ, float segmentSize, float tx, float tz){
    geoType = GRID_GEO;
    this->segmentSize = segmentSize;
    this->dx = divisionsX;
    this->dz = divisionsZ;
    this->tx = tx;
    this->tz=tz;
    p = new Poly();
    for(int i=0; i<divisionsX;i++) {
        for(int j=0;j<divisionsZ;j++) {
            p->addVertex(Vertex(i*segmentSize+tx,0,j*segmentSize+tz));
            p->addTexCoord(TexCoord(float(i*segmentSize+tx)/float(divisionsX),float(j*segmentSize+tz)/float(divisionsZ)));
        }
    }
    for(int i=0; i<divisionsZ*(divisionsX-1);i++) {
		if (!(i%divisionsZ))continue;
		Face f = Face();
		f.vi.push_back(i);  
		f.vi.push_back(i+1);  
		f.vi.push_back(i+1+divisionsZ);  
		f.vi.push_back(i+divisionsZ);  
		f.ti.push_back(i);  
		f.ti.push_back(i+1);  
		f.ti.push_back(i+1+divisionsZ);  
		f.ti.push_back(i+divisionsZ);  
		p->addFace(f);
    }
    p->computeNormals();
    //p->centroid();
    p->bound();
    this->bound();
    this->addPoly(p);
}

Grid::~Grid(){}
