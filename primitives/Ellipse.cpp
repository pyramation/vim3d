#include <math.h>
#include "Ellipse.h"

Ellipse::Ellipse(float width, float height, int levelOfDetail, bool face){
    geoType = ELIPSE_GEO;
    p = new Poly();
	float theta, piece;
	//width = 10.0f; height = 10.0f;
	theta = 0.0f;
	//levelOfDetail = 64;
	piece = 2.0f*3.14159/levelOfDetail;
	for (int i = 0; i<levelOfDetail; i++) {
		Point p1(width*cos(theta), height*sin(theta), 0.0f);
		p->addVertex(p1);
		theta = theta + piece;
	}
	Point p1(width, 0.0, 0.0);
	p->addVertex(p1);
	//Add the first vertex again(in case this is a curve)
	

	if (face) {
		Poly * newP = new Poly();
		point_t::iterator cursor, cursor2, cursor3;
		cursor = p->vertices.begin();
		cursor2 = p->vertices.end()-1;
		Point p1,  p2, p3, p4;
		while(true){
			if(cursor+2>=cursor2) break; //less than 3 vertices on this side
			p1 = *(cursor);
			p2 = *(cursor2);
			cursor2--;
			p3 = *(cursor2);
			cursor++;
			if (cursor2==cursor) {
				newP->addFace(p1, p2, p3);
				break;
			}
			p4 = *(cursor);
			newP->addFace(p1, p2, p3, p4);
		}
		p = newP;
		p->computeNormals();
	}    
	
    this->addPoly(p); 
    p->bound();
    this->bound();
}


Ellipse::~Ellipse(){}
