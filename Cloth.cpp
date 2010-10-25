#include "Cloth.h"

Springs::Springs() {
    //vi = vector<int>();
    //ni = vector<int>();
    //ti = vector<int>();
    neighbors = vector <int>();
}

void Cloth::updateDistribution(int Time){
	//printf("time:%d\n", Time);
	Time = Time*timeScalar;
	vector <Vertex> tempV, firstV;
	//vertices.clear();
	//float integralT = 0.0f;
	float segment = .03f*timeScalar;
	float dist;
	//printf("nearest:%d\n", (int)nearestSprings.size());
	unsigned int pin1 = vertices.size()-1;
	unsigned int pin2 = vertices.size()-width;
	unsigned int pin3 = vertices.size()-width/2;	
	Point k1, k2, k3, k4;	
	for (float t = lastTime; t<Time; t+=segment) {
		
		firstV = vertices;
		tempV = vertices;
		vertices.clear();
		Point force;
		for (unsigned int i = 0; i<firstV.size(); i++){
			//printf("%d\n", i);
			if (i == pin1 or i == pin2 or i==pin3) {
				vertices.push_back(firstV[i]);
				continue;
			}		
			for (int degree = 0; degree<3; degree++) {
				if (degree == 0) {
					k1 = velocities[i];
					// use approximate midpoint to find k2
					tempV[i] = firstV[i] + k1*.5*segment;
				}
				
				force = Point(0.0f, -1.3f, 0.0f);
				
				// grid direction springs
				for (unsigned int j = 0; j<nearestSprings[i].neighbors.size(); j++) {
					//printf("j1:%d\n", j);				
					Point vec = tempV[i]-firstV[nearestSprings[i].neighbors[j]];
					dist = sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
					vec.normalize();
					//printf("force=%f,%f,%f, sep:%f\n", vec.x,vec.y,vec.z, nSep-dist);
					force = force + (vec*(nSep-dist)*kN);
				}
				
				for (unsigned int j = 0; j<skipSprings[i].neighbors.size(); j++) {
					//printf("j2:%d\n", j);				
					Point vec = tempV[i]-firstV[skipSprings[i].neighbors[j]];
					dist = sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
					vec.normalize();
					force = force + (vec*(sSep-dist)*kS);
				}
				for (unsigned int j = 0; j<diagonalSprings[i].neighbors.size(); j++) {
					//printf("j3:%d\n", j);				
					Point vec = tempV[i]-firstV[diagonalSprings[i].neighbors[j]];
					dist = sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
					vec.normalize();
					force = force + (vec*(dSep-dist)*kD);
				}
				float windScalar = -3.8f;
				force += Point(0.0f, 0.0f, windScalar)*(noiseFunc->perlinNoise2D(tempV[i].y+100, tempV[i].x+100) * noiseFunc2->perlinNoise2D(tempV[i].z+100, Time+1000));

				
			//vertices[i] = tempV[i] + velocities[i]*segment;
			// RK4 method
				if (degree==0) {
					k2 = velocities[i] + k1 + force * segment*.5f;
					// use k2 to find k3
					tempV[i] = firstV[i] + k2*.5*segment;					
				} else if (degree ==1) {
					k3 = velocities[i] + force * segment*.5f;
					// use k3 to find k4
					tempV[i] = firstV[i]  + k3*segment;					
				} else {
					k4 = velocities[i] + force*segment;
				}
			}
			vertices.push_back(firstV[i] + (k1+k4+((k2+k3)*2))*segment/6.0f);
			velocities[i]= k4*.55f;
		// for now small arbitrary dampening
		//velocities[i] += force*segment;
		//velocities[i] *= .8f;
		//if (velocities[i].x!=0.0f||velocities[i].z!=0.0f) {
		//  printf("velocity[i]%f,%f,%f\n", velocities[i].x, velocities[i].y, velocities[i].z);
		//}
		}
	}
	//for (unsigned int i = 0; i<vertices.size(); i+=10) {
		//printf("vert[%d].x, y, z= %f, %f, %f\n", i, vertices[i].x, vertices[i].y, vertices[i].z);
	//}
	lastTime = Time;
	computeSmoothNormals();
	freeList();
	bound();
}



Cloth::Cloth(){
	width = 14;
	height = 14;
	lastTime = 0;
	timeScalar = 7;
	nSep = 7.0f;
	dSep = sqrt(nSep*nSep+nSep*nSep);
	sSep = 2*nSep;
	kN = 10.05f;
	kD = 8.04f;
	kS = 2.01f;
	noiseFunc = new perlinNoise();
	noiseFunc->freqOctDep = false;
	noiseFunc->ampOctDep = false;
	noiseFunc->useSum = false;
	noiseFunc->frequency = 0.5f;
	noiseFunc->persistence = 1.0f/64.0f;
	
	noiseFunc2 = new perlinNoise();
	noiseFunc2->freqOctDep = false;
	noiseFunc2->ampOctDep = false;
	noiseFunc2->useSum = true;
	noiseFunc2->frequency = .05f;
	noiseFunc2->persistence = 1.0f/128.0f;
	
	int j = 0; 
	int h = -1;
	for (int i = 0; i<width*height; i++, j++) {
		if (j%width==0) {
			h++;
		}
		j = j%width;
		
		vertices.push_back(Point((j-(width/2))*nSep, h*nSep+height*nSep/2, 0.0));
		velocities.push_back(Point(0.0f, 0.0f, 0.0f));
	}
	// Make a link group for every vertex for use in getForces
	j = 0; 
	h = -1;
	for (unsigned int i = 0; i<vertices.size(); i++, j++){
		Springs closeSprings = Springs();
		Springs diagSprings = Springs();
		Springs skippingSprings = Springs();		
		if (j%width==0) {
			h++;
		}
		j = j%width;
		
		// verticle and horizontal springs, skip springs for anti-folding
		if (j>0) {
			closeSprings.neighbors.push_back((j-1)+(h*width));
			if (j>1) {
				skippingSprings.neighbors.push_back((j-2)+(h*width));
			}
		}
		if (j<width-1) {
			closeSprings.neighbors.push_back((j+1)+(h*width));
			if (j<width-2) {
				skippingSprings.neighbors.push_back((j+2)+(h*width));
			}
		}		
		if (h>0) {
			closeSprings.neighbors.push_back(j+((h-1)*width));
			if (h>1) {
				skippingSprings.neighbors.push_back(j+((h-2)*width));
			}
		}
		if (h<width-1) {
			closeSprings.neighbors.push_back(j+((h+1)*width));
			if (h>width-2) {
				skippingSprings.neighbors.push_back(j+((h+2)*width));
			}
		}	
		
		//diagonal springs
		if (j>0&&h>0) {
			diagSprings.neighbors.push_back((j-1)+((h-1)*width));
		}
		if (j>0&&h<width-1) {
			diagSprings.neighbors.push_back((j-1)+((h+1)*width));
		}		
		if (j<width-1&&h>0) {
			diagSprings.neighbors.push_back((j+1)+((h-1)*width));
		}
		if (j<width-1&&h<width-1) {
			diagSprings.neighbors.push_back((j+1)+((h+1)*width));
		}		
		nearestSprings.push_back(closeSprings);
		skipSprings.push_back(skippingSprings);
		diagonalSprings.push_back(diagSprings);		
		
		if(j<width-1&&h<height-1){
			Face f = Face();
			f.vi.push_back(j+(h*width)+1);
			f.vi.push_back(j+h*width+width+1);	
			f.vi.push_back(j+h*width+width+2);
			f.vi.push_back(j+(h*width)+2);
			faces.push_back(f);
		}
	}
}
Cloth::~Cloth(){
	
}
/*
#include "Poly.h"
#include "Term.h"
#include "Primitives.h"
#include "vectorUtils.h"
#include "Scene.h"

Face::Face() {
    vi = vector<int>();
    ni = vector<int>();
    ti = vector<int>();
    neighbors = vector <int>();
}

void Poly::bound() {
    bool init = false;
    cx = cy = cz = 0.0f;
    point_t::iterator csr=vertices.begin();
    for(;csr!=vertices.end();csr++) {
        Point p = *csr;
        cx += p.x/float(vertices.size());
        cy += p.y/float(vertices.size());
        cz += p.z/float(vertices.size());

        if (init) {
            if (p.x < minX) minX = p.x;
            if (p.x > maxX) maxX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.y > maxY) maxY = p.y;
            if (p.z < minZ) minZ = p.z;
            if (p.z > maxZ) maxZ = p.z;
        } else {
            maxX = minX = p.x;
            maxY = minY = p.y;
            maxZ = minZ = p.z;
            init = true;
        }    
    }
}

void Poly::invertNormals(){
	for (unsigned int i = 0; i<normals.size();i++){
		normals[i] = normals[i]*-1;
	}
}

void Poly::addNormal(Normal n) { normals.push_back( n ); }
void Poly::addVertex(Vertex v) { vertices.push_back( v ); }
void Poly::addTexCoord(TexCoord t) { textures.push_back( t ); }

void Poly::addFace(Vertex a, Vertex b, Vertex c, Vertex d) {
    Face f = Face();
    addVertex(a); f.vi.push_back(vertices.size());  
    addVertex(b); f.vi.push_back(vertices.size());  
    addVertex(c); f.vi.push_back(vertices.size());  
    addVertex(d); f.vi.push_back(vertices.size());  
    TexCoord t1(0,0);addTexCoord(t1); f.ti.push_back(textures.size());
    TexCoord t2(0,1);addTexCoord(t2); f.ti.push_back(textures.size());
    TexCoord t3(1,1);addTexCoord(t3); f.ti.push_back(textures.size());
    TexCoord t4(1,0);addTexCoord(t4); f.ti.push_back(textures.size());
    addFace(f);
}

void Poly::addFace(Vertex a, Vertex b, Vertex c) {
    Face f = Face();
    addVertex(a);f.vi.push_back(vertices.size());
    addVertex(b);f.vi.push_back(vertices.size());
    addVertex(c);f.vi.push_back(vertices.size());
    TexCoord t1(0,0); TexCoord t2(0,1); TexCoord t3(1,1);
    addTexCoord(t1);f.ti.push_back(textures.size());
    addTexCoord(t2);f.ti.push_back(textures.size());
    addTexCoord(t3);f.ti.push_back(textures.size());
    addFace(f);
}

void Poly::addFace(vector<int>&indices) {
    Face f = Face();
    for(unsigned int i=0;i<indices.size();i++) {
        f.vi.push_back(indices[i]);
    }
    addFace(f);
}

void Poly::addFace(Face  f) { faces.push_back(f); }

// way to show face numbers?
// booleans to keep original face and create new end cap?
// this can be the generalization for sweep!
void Poly::ExtrudeFace(unsigned int n, Point dir) {
    if (faces.size() > n) {

        vector<int>indices = vector<int>();
        for(unsigned int i=0;i<faces[n].vi.size();i++) {
            addVertex(dir+vertices[faces[n].vi[i]-1]);
            indices.push_back(vertices.size());
        }
        addFace(indices); // this face should have same normals and all
        vector<int>begin = faces[n].vi;
        ExtrudeFace(begin, indices);
        freeList();
        bound();
    }
}
 
 idea: use the s and t coords from the extrude vertices 

void Poly::ExtrudeFace(vector<int>&start, vector<int>&end) {
   if (start.size() != end.size()) return;
   vector<int>newIndices = vector<int>();
   for(unsigned int i=0;i<start.size();i++) {
            newIndices.push_back(start[i]);
            newIndices.push_back(end[i]);
            newIndices.push_back(end[(i+1) % end.size()]);
            newIndices.push_back(start[(i+1) % start.size()]);
            addFace(newIndices);
            newIndices.clear();
   }
}

 
This function should be passed in the starting indices, an empty container, and three points
it will then add the appropriate transformed vertices indices into container


void Poly::transferCopy(vector<int>&indices, vector<int>&container, Point start, Point mid, Point end) {
    Point a = mid - start; a.normalize();
    Point b = end - mid; b.normalize();
    Point c = a + b;     
    c.normalize();
    mat4 M = Transform::coordinateFrame(Point(0,1,0), c);
    float cx, cy, cz;
    cx = cy = cz = 0;
    for(unsigned int i=0;i<indices.size();i++) {
        cx += vertices[indices[i]-1].x/indices.size();
        cy += vertices[indices[i]-1].y/indices.size();
        cz += vertices[indices[i]-1].z/indices.size();
    }
    M.a03 = mid.x-cx;
    M.a13 = mid.y-cy;
    M.a23 = mid.z-cz;
    for(unsigned int i=0;i<indices.size();i++) {
        addVertex(vertices[indices[i]-1] * M);
        container.push_back(vertices.size());
    }
}

void Poly::curveMe(Geo * crossSec) {
    vector<Vertex>points = vertices;
    vertices.clear();
    vector<int>indices = vector<int>(); 
    for(unsigned int i=0; i<P(crossSec)->vertices.size(); i++) {
        addVertex( P(crossSec)->vertices[i] );
        indices.push_back( vertices.size() );
    }
    vector<int>container = vector<int>();
    for(unsigned int i=0; i<points.size()-2; i++) {
        transferCopy(indices, container, points[i], points[i+1], points[i+2]);
        ExtrudeFace(indices, container);
        indices = container;
        container.clear();
    }
}

void Poly::readFile(String filename) { readOBJ(filename); }

void Poly::visualizePoints() {
    glColor3f(0.0f,1.0f,1.0f);
    for(unsigned int i=0; i<faces.size();i++) {
        if (faces[i].ni.size() > 0) {
            Scene::toggleLights(false);
            for(unsigned int j=0; j<faces[i].vi.size();j++) {
                if (curSTATE & POINT_NORMALS) {
                    glBegin(GL_LINES);
                    if (faces[i].ni.size() == faces[i].vi.size()) {
                        glVertex3f( vertices[faces[i].vi[j]-1].x, vertices[faces[i].vi[j]-1].y, vertices[faces[i].vi[j]-1].z);     
                        glVertex3f( normals[faces[i].ni[j]-1].x + vertices[faces[i].vi[j]-1].x, normals[faces[i].ni[j]-1].y + vertices[faces[i].vi[j]-1].y, normals[faces[i].ni[j]-1].z + vertices[faces[i].vi[j]-1].z);
                    } else {
                        glVertex3f( vertices[faces[i].vi[j]-1].x, vertices[faces[i].vi[j]-1].y, vertices[faces[i].vi[j]-1].z);     
                        glVertex3f( normals[faces[i].ni[0]-1].x + vertices[faces[i].vi[j]-1].x, normals[faces[i].ni[0]-1].y + vertices[faces[i].vi[j]-1].y, normals[faces[i].ni[0]-1].z + vertices[faces[i].vi[j]-1].z);
                    }
                    glEnd();
                }
                if (curSTATE & POINT_NUMS) {
                    glRasterPos3f( vertices[faces[i].vi[j]-1].x, vertices[faces[i].vi[j]-1].y, vertices[faces[i].vi[j]-1].z);
                    char s[16];
                    const char * t = s;
                    sprintf(s, "%d", faces[i].vi[j]-1);
                    for(;*t!='\0';t++)
                        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s);

                }
            }
            Scene::toggleLights(true);
        }
    }
    glColor3f(1.0f,1.0f,1.0f);
}

// these normals will be transormed by the model matrix in a way that is no good.
// to remedey this you should do this in the geo->display() and invert themodelview transform and apply a different transform
void Poly::visualizeFaces() {

    for(unsigned int i=0;i<faces.size();i++) {
        float cx, cy, cz;
        cx = cy = cz = 0.0f;
        for(unsigned int j=0;j<faces[i].vi.size();j++) {
            cx += vertices[faces[i].vi[j]-1].x/float(faces[i].vi.size());
            cy += vertices[faces[i].vi[j]-1].y/float(faces[i].vi.size());
            cz += vertices[faces[i].vi[j]-1].z/float(faces[i].vi.size());
        }
        float nx, ny, nz;
        nx = ny = nz = 0.0f;
        for(unsigned int k=0;k<faces[i].ni.size();k++) {
            nx += normals[faces[i].ni[k]-1].x/float(faces[i].ni.size());
            ny += normals[faces[i].ni[k]-1].y/float(faces[i].ni.size());
            nz += normals[faces[i].ni[k]-1].z/float(faces[i].ni.size());
        }
        Normal norm(cx+nx, cy+ny, cz+nz);
        Scene::toggleLights(false);
        glColor3f(1.0f,0.0f,1.0f);

        if (curSTATE & FACE_NORMALS) { 
            glBegin(GL_LINES);
            glVertex3f(cx, cy, cz);
            glVertex3f(norm.x, norm.y, norm.z);
            glEnd(); 
        }

        if (curSTATE & FACE_NUMS) {
            // print face number
            glRasterPos3f(norm.x, norm.y, norm.z);
            char s[16];
            const char * t = s;
            sprintf(s, "%d", i);
            for(;*t!='\0';t++)
                glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s);
        }

        Scene::toggleLights(true);
        glColor3f(1.0f,1.0f,1.0f);
    }

}

void Poly::bounds() {
    Scene::toggleLights(false);
    glBegin(GL_LINES);
    glVertex3f(minX, minY, minZ); glVertex3f(maxX, minY, minZ);
    glVertex3f(minX, minY, minZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, minY, minZ); glVertex3f(minX, minY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(minX, maxY, maxZ);
    glVertex3f(minX, minY, maxZ); glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, minY, maxZ); glVertex3f(maxX, minY, minZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(minX, maxY, minZ);
    glVertex3f(minX, maxY, minZ); glVertex3f(minX, maxY, maxZ);
    glVertex3f(minX, maxY, maxZ); glVertex3f(maxX, maxY, maxZ);
    glVertex3f(maxX, maxY, maxZ); glVertex3f(maxX, minY, maxZ);
    glVertex3f(maxX, maxY, minZ); glVertex3f(maxX, minY, minZ);
    glEnd();
    Scene::toggleLights(true);
}

void Poly::displayGeometry() {

    if ((curSTATE & POINT_NORMALS) || (curSTATE & POINT_NUMS)) visualizePoints();
    if ((curSTATE & FACE_NORMALS) || (curSTATE & FACE_NUMS)) visualizeFaces();

    if (showBounds) bounds();

    if (faces.size() == 0 && curSTATE==DISPLAY_STATE) return;

    if ((curSTATE & DISPLAY_LIST) && displayListLoaded) {
        glCallList(listIndex);

    } else {

        if (curSTATE & DISPLAY_LIST) {
            listIndex = glGenLists(1);
            glNewList(listIndex, GL_COMPILE);
        }

        if (faces.size() > 0) {

            for(unsigned int f=0; f<faces.size();f++) {

                if (faces[f].vi.size() == 3) glBegin(GL_TRIANGLES);
                else if (faces[f].vi.size() == 4) glBegin(GL_QUADS);
                else if (faces[f].vi.size() > 4) glBegin(GL_POLYGON);
                else continue;

                for(unsigned int v=0;v<faces[f].vi.size();v++) {


                    if (faces[f].ti.size() > v) {

                        if (textures.size() > (unsigned int) faces[f].ti[v]-1)
                            glTexCoord3f(textures[faces[f].ti[v]-1].x, textures[faces[f].ti[v]-1].y, textures[faces[f].ti[v]-1].z);
                    }


                    if (faces[f].ni.size() > v) {
                        if (normals.size() > (unsigned int) faces[f].ni[v]-1) 
                            glNormal3f(normals[faces[f].ni[v]-1].x, normals[faces[f].ni[v]-1].y, normals[faces[f].ni[v]-1].z);
                        
                    }


                    if (vertices.size() > (unsigned int) faces[f].vi[v]-1)
                        glVertex3f(vertices[faces[f].vi[v]-1].x, vertices[faces[f].vi[v]-1].y, vertices[faces[f].vi[v]-1].z);
                }

                glEnd();
            }

        } else {
            if (vertices.size() > 1) {

                Scene::toggleLights(false);
                glBegin(GL_LINES);
                for(unsigned int p=0;p<vertices.size()-1;p++) {
                    glVertex3f(vertices[p].x, vertices[p].y, vertices[p].z);
                    glVertex3f(vertices[p+1].x, vertices[p+1].y, vertices[p+1].z);
                }
                glEnd();
                Scene::toggleLights(true);
            }
        }
        

        if (curSTATE & DISPLAY_LIST) {
            glEndList();
            displayListLoaded = true;
            glCallList(listIndex);
        }
    }

}

void Poly::freeList() {
    if (displayListLoaded) {
        displayListLoaded = false;
        glDeleteLists(listIndex, 1);
    }
}

void Poly::clearAll() {
    normals.clear();
    textures.clear();
    faces.clear();
    vertices.clear();
    freeList();
}

void Poly::Bezier(int levelOfDetail) {
    Curve c(vertices);
    clearAll();
    vertices = c.Bezier(levelOfDetail);
    bound();
}


void Poly::Bezier2(int levelOfDetail) {
    Curve c(vertices);
    clearAll();
    vertices = c.Bezier2(levelOfDetail);
    bound();
}

void Poly::Bspline(int levelOfDetail) {
    Curve c(vertices);
    clearAll();
    vertices = c.Bspline(levelOfDetail);
    bound();
}

void Poly::Extrude(float x, float y, float z) {
    int numCurves = 2;
    int numPoints = vertices.size();

    Curve c = Curve(vertices);

    clearAll();

    point_t p1 = c.translate(0.0,0.0,0.0);
    point_t p2 = c.translate(x,y,z);

    point_t::iterator curve1, curve2;
    curve1 = p1.begin();
    curve2 = p2.begin();
    int index=0;
    int pointCount, curveCount;
    pointCount=curveCount=0;
    for(; curve1!=p1.end()-1 && curve2 != p2.end()-1; curve1++, curve2++) {
        Vertex p1 = *curve1;
        TexCoord t1(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));
        Vertex p4 = *(curve1+1);
        TexCoord t4(float(pointCount+1)/float(numPoints),float(curveCount)/float(numCurves));
        Vertex p2 = *curve2;
        TexCoord t2(float(pointCount)/float(numPoints),float(curveCount+1)/float(numCurves));
        Vertex p3 = *(curve2+1);
        TexCoord t3(float(pointCount+1)/float(numPoints),float(curveCount+1)/float(numCurves));

        Face f = Face();
        vertices.push_back(p1);
        textures.push_back(t1);
        index++;
        f.vi.push_back(index);
        f.ti.push_back(index);
        vertices.push_back(p2);
        textures.push_back(t2);
        index++;
        f.vi.push_back(index);
        f.ti.push_back(index);
        vertices.push_back(p3);
        textures.push_back(t3);
        index++;
        f.vi.push_back(index);
        f.ti.push_back(index);
        vertices.push_back(p4);
        textures.push_back(t4);
        index++;
        f.vi.push_back(index);
        f.ti.push_back(index);
        faces.push_back(f);
        pointCount++;
    }
    bound();
}

void Poly::Revolve(float degs, float x, float y, float z) {
    int numCurves = int(degs)/10;// this should be modular (10.0f)
    int numPoints = vertices.size();

    Curve c = Curve(vertices);

    clearAll();

    int index=0;
     iterations over new curves 
    int curveCount = 0;
    for(float degrees = 0; degrees<degs; degrees+=10) {
         point_t is a vector of points, the curve class is returning a curve so to speak 
        point_t p1 = c.rotate(degrees, x,y,z);
        point_t p2 = c.rotate(degrees+10, x,y,z);

        point_t::iterator curve1, curve2;
        curve1 = p1.begin();
        curve2 = p2.begin();
        int pointCount = 0;
         iteration over the curve points 
        for(; curve1!=p1.end()-1 && curve2 != p2.end()-1; curve1++, curve2++) {
            Vertex p1 = *curve1;
            TexCoord t1(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));
            Vertex p4 = *(curve1+1);
            TexCoord t4(float(pointCount+1)/float(numPoints),float(curveCount)/float(numCurves));
            Vertex p2 = *curve2;
            TexCoord t2(float(pointCount)/float(numPoints),float(curveCount+1)/float(numCurves));
            Vertex p3 = *(curve2+1);
            TexCoord t3(float(pointCount+1)/float(numPoints),float(curveCount+1)/float(numCurves));

            Face f = Face();
            vertices.push_back(p1);
            textures.push_back(t1);
            index++;
            f.vi.push_back(index);
            f.ti.push_back(index);
            vertices.push_back(p2);
            textures.push_back(t2);
            index++;
            f.vi.push_back(index);
            f.ti.push_back(index);
            vertices.push_back(p3);
            textures.push_back(t3);
            index++;
            f.vi.push_back(index);
            f.ti.push_back(index);
            vertices.push_back(p4);
            textures.push_back(t4);
            index++;
            f.vi.push_back(index);
            f.ti.push_back(index);
            faces.push_back(f);
            pointCount++;
        }
        curveCount++;
    }
    bound();
}

void Poly::textureMap(){
	//map vertices with M?
	//this is for a nearly flat surface
	textures.clear();
	int index = 0;
	for (unsigned int i = 0; i<faces.size(); i++) {
		Face f = faces[i];
		Face temp = Face();
		for (unsigned int j = 0; j<faces[i].vi.size(); j++) {
			Point v(vertices[f.vi[j]].x, vertices[f.vi[j]].y, vertices[f.vi[j]].z);
			TexCoord t(v.x+.7*v.y, v.z+.7*v.y);
			textures.push_back(t);
			temp.vi.push_back(f.vi[j]);
			temp.ti.push_back(index);
			printf("v.x+.7*v.y, v.z+.7*v.y:%f, %f\n", v.x+.7*v.y, v.z+.7*v.y);
		}
	faces[i] = temp;
	}
}

void Poly::Sweep(Geo * backBone, Geo * crossSec, bool onPath, bool isWall) {
    // Set up the curve to translate along
    // along is the path of the cross-section
    Curve tempC = Curve(vertices);
    clearAll();

    Curve c = *(tempC.curveM(backBone->M)); //adjust points to M(atrix)
	point_t along = c.translate(0.0f, 0.0f, 0.0f);
    point_t::iterator curveC = along.begin();
	
	Vertex point1 = *curveC;
    Curve tmp = Curve(P(crossSec)->vertices);
    Curve fromCurve = *(tmp.curveM(crossSec->M));//adjust points to M(atrix)
	if (onPath) {
		for (unsigned int vertInd = 0; vertInd<fromCurve.points.size(); vertInd++) {
			fromCurve.points[vertInd].x+=point1.x;
			fromCurve.points[vertInd].y+=point1.y;
			fromCurve.points[vertInd].z+=point1.z;
		}
	}
    //stuff for wall
    float cenX, cenY, cenZ, minX, maxX, minZ, maxZ, width;//, dx, dy, dz; //centroid	
    cenX = 0; cenY = 0; cenZ = 0;
    minX = 9999999; maxX = -9999999; minZ = 9999999; maxZ = -9999999;
    //point_t onePoints, twoPoints, tempPoints; //these are for wall-side division	
	
    for(point_t::iterator crossSection = fromCurve.points.begin(); crossSection<fromCurve.points.end(); crossSection++){
        cenX = cenX + crossSection->x;
        cenY = cenY + crossSection->y;
        cenZ = cenZ + crossSection->z;
        if(crossSection->x<minX) minX = crossSection->x;
        if(crossSection->x>maxX) maxX = crossSection->x;
        if(crossSection->z<minZ) minZ = crossSection->z;
        if(crossSection->z>maxZ) maxZ = crossSection->z;
    }
    cenX = cenX / fromCurve.points.size(); 
    cenY = cenY / fromCurve.points.size();
    cenZ = cenZ / fromCurve.points.size();	
    width = .5*sqrtf((maxX-minX)*(maxX-minX)+(maxZ-minZ)*(maxZ-minZ));
    float sumTheta = 0.0f; //for debugging
	
    point_t::iterator curve1, curve2, curve3;
    point_t first, last, previous;
    bool side[fromCurve.points.size()]; 
	
    // Set up the first curve to extrude across
    Point delta, thisDelta, axis, lastDelta, hyperDelta;
	
	//stuff for texture mapping
	int numCurves = fromCurve.points.size();
    int numPoints = vertices.size();	
    int curveCount = 0;		
	int pointCount = 0;
	
    for (;curveC<along.end()-1; curveC++) {
        ************ Wall 2-curve setup ************
        if(isWall){
			if(curveC==along.begin()){ 
				first = fromCurve.translate(0.0,0.0,0.0);
				// hyperdelta is a vector to find the insideWall 
				hyperDelta = Point::normalize(Point((curveC+2)->x-(curveC+1)->x, 0.0, (curveC+2)->z-(curveC+1)->z));
				float avTheta = 0.0;
				for(unsigned int i=0;i<fromCurve.points.size();i++){
					Point tempP = Point(fromCurve.points[i].x-cenX, 0.0, fromCurve.points[i].z - cenZ);
					tempP.normalize();
					float angle = atan2(tempP.x,tempP.z) - atan2(hyperDelta.x, hyperDelta.z);
					if (angle>PI_) {
						angle = angle - 2*PI_;
					}
					if (angle< -PI_) {
						angle = angle + 2*PI_;								
					}
					avTheta = avTheta + (angle*angle);
				}
				avTheta = avTheta / fromCurve.points.size();			
				//split points to sides of the wall
				for(unsigned int i=0;i<fromCurve.points.size();i++){
					Point tempP = Point(fromCurve.points[i].x-cenX, 0.0, fromCurve.points[i].z - cenZ);					
					tempP.normalize();
					float angle = atan2(tempP.x, tempP.z) - atan2(hyperDelta.x, hyperDelta.z);
					// shift closest to 
					if (angle>PI_) {
						angle = angle - 2*PI_;	
					}
					if (angle< -PI_) {
						angle = angle + 2*PI_;							
					}
					
					if(angle*angle<avTheta){
						side[i] = true;
					} else {				
						// farthest side
						side[i] = false;
					}
				}				
				
				// not the first segment of the wall
			} else {
				if(curveC-1>along.begin()) previous = first;
				first = last;
			}
			
			
			// this occurs for every wall segment
			thisDelta = Point((curveC+1)->x - curveC->x, (curveC+1)->y - curveC->y, (curveC+1)->z - curveC->z);
			Point normDelta = Point::normalize(thisDelta);
			
			//check if we need to switch wall-inner sides
			Point oneD, twoD;
			for (unsigned int i= 0; i<fromCurve.points.size(); i++) {
				if(side[i]){
					oneD = Point(fromCurve.points[i].x-cenX, 0.0, fromCurve.points[i].z-cenZ);
				} else {
					twoD = Point(fromCurve.points[i].x-cenX, 0.0, fromCurve.points[i].z-cenZ);
				}
			}
			
			if ((curveC+2)<along.end()) {
				hyperDelta = Point((curveC+2)->x-(curveC+1)->x, (curveC+2)->y-(curveC+1)->y, (curveC+2)->z-(curveC+1)->z);
				float angleD = atan2(hyperDelta.x, hyperDelta.z);
				float angleOne = atan2(oneD.x, oneD.z);
				float angleTwo = atan2(twoD.x, twoD.z);
				angleOne = angleOne - angleD;
				angleTwo = angleTwo - angleD;
				// shift closest to 0 rads
				if (angleOne>PI_) {
					angleOne = angleOne - 2*PI_;	
				}
				if (angleOne< -PI_) {
					angleOne = angleOne + 2*PI_;							
				}
				if (angleTwo>PI_) {
					angleTwo = angleTwo - 2*PI_;	
				}
				if (angleTwo< -PI_) {
					angleTwo = angleTwo + 2*PI_;							
				}
				if(angleOne*angleOne>angleTwo*angleTwo){
					for(unsigned int i= 0; i<fromCurve.points.size(); i++) {
						side[i] = !side[i];
					}
				}
				
			} else {
				// final segment is straight across perp to final delta
				
			}
			
			cenX = cenX + thisDelta.x;
			cenY = cenY + thisDelta.y; 
			cenZ = cenZ + thisDelta.z;
			
			for(unsigned int i=0;i<fromCurve.points.size();i++) {
				
				//find the equations for the line perp line through the new center and solve
				float m1 = (thisDelta.x/thisDelta.z);	
				float c1 = fromCurve.points[i].x - m1 * fromCurve.points[i].z;
				float m2 = -(thisDelta.z/thisDelta.x);	
				float c2 = cenX - m2 * cenZ;
				float solnZ = (c2-c1)/(m1 - m2);
				float solnX = m1 * solnZ + c1;
				Point del = Point::normalize(Point(solnX-cenX, 0.0, solnZ-cenZ));
				if (curveC+2==along.end()) {
					fromCurve.points[i].x = cenX + del.x*width;
					fromCurve.points[i].z = cenZ + del.z*width;
					continue;
				}
				
				// normDelta is thisDelta normalized
				if(side[i]){
					fromCurve.points[i].x = cenX + del.x*width - normDelta.x*width;
					fromCurve.points[i].z = cenZ + del.z*width  - normDelta.z*width;
					
				} else {
					fromCurve.points[i].x = cenX + del.x*width  + normDelta.x*width;
					fromCurve.points[i].z = cenZ + del.z*width  + normDelta.z*width;
					
				}
			}
			last = fromCurve.translate(0.0,0.0,0.0);
			
            ******************* non-wall Sweep 2-curve Setup *********************	
		} else{
            // this case is the final section so curve is rotated to normal the final line-seg
            if(curveC+2==along.end()){
                thisDelta = Point::normalize(Point((curveC+1)->x - curveC->x, (curveC+1)->y - curveC->y, (curveC+1)->z - curveC->z));
				
                // this is the average of two sections delta for a middle 'face'
            } else {
                thisDelta = Point::normalize(Point((curveC+2)->x - curveC->x, (curveC+2)->y - curveC->y, (curveC+2)->z - curveC->z));
            }
			
            // first face - setup delta and first curve
            if(curveC==along.begin()){
                delta = Point::normalize(Point((curveC+1)->x - along.begin()->x, (curveC+1)->y - along.begin()->y, 
											   (curveC+1)->z - along.begin()->z));
                first = fromCurve.translate(0.0,0.0,0.0);
				
                // not first face - lastDelta & last are first/delta	
            } else {
				if(curveC-1>along.begin()) previous = first;
                delta = lastDelta;
                first = last;
				
            }
            lastDelta = thisDelta;
            //delta and thisDelta are normalized so asin of cross = angle
			
            axis = Point::cross(delta, thisDelta);
            float theta = asin(axis.length());
            sumTheta = sumTheta + theta;
            axis.normalize();		
            fromCurve.rtMod(theta, axis.x, axis.y, axis.z,
							(curveC+1)->x - curveC->x, (curveC+1)->y - curveC->y, (curveC+1)->z - curveC->z, cenX, cenY, cenZ);
            cenX = cenX + (curveC+1)->x - curveC->x;
            cenY = cenY + (curveC+1)->y - curveC->y; 
            cenZ = cenZ + (curveC+1)->z - curveC->z;
			
            last = fromCurve.translate(0.0,0.0,0.0);
        }
		
		pointCount = 0;
        ************* start segment sweep*************
        curve1 = first.begin();
        curve2 = fromCurve.points.begin();
		
		if(curveC-1>along.begin())	curve3 = previous.begin();
		
		Vertex p1, p2, p3, p4;
		if(curveC==along.begin()){
			p1 = *curve1;
			vertices.push_back(p1);		
			TexCoord t(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));	
			textures.push_back(t);
			pointCount++;
			for(; curve1!= first.end()-1; curve1++) {					
				p4 = *(curve1+1);
				vertices.push_back(p4);
				TexCoord t(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));	
				textures.push_back(t);				
				pointCount++;
			}
			curveCount++;
		}
		pointCount = 0;
		
		p2 = *curve2;
		vertices.push_back(p2);		
		TexCoord t(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));	
		textures.push_back(t);		
		pointCount++;
        for(;curve2 != fromCurve.points.end()-1 ;curve2++) {							
			p3 = *(curve2+1);
			vertices.push_back(p3);
			TexCoord t(float(pointCount)/float(numPoints),float(curveCount)/float(numCurves));	
			textures.push_back(t);			
			pointCount++;
		}
		curveCount++;

    }
	
	unsigned int j = 0;
	for(unsigned int i = 0; i<fromCurve.points.size()*(along.size()-1); i++, j++){
		if (j==fromCurve.points.size()) {
			j=0;
			continue;
		}
		Face f = Face();
		f.vi.push_back(i);  
		f.vi.push_back(i+1);  
		f.vi.push_back(i+1+fromCurve.points.size());  
		f.vi.push_back(i+fromCurve.points.size()); 

		f.ti.push_back(i);		
		f.ti.push_back(i+1);
		f.ti.push_back(i+1+fromCurve.points.size());  
		f.ti.push_back(i+fromCurve.points.size());  

		faces.push_back(f);
	}
    bound();
	computeNormals();
}

void Poly::rotate(float degrees, float x, float y, float z) {
    float theta = degrees*PI_/180.0f;
    mat4 R = Transform::rotateMat4(theta, Point(x,y,z));
    for(unsigned int i=0; i<vertices.size();i++) {
        Point temp = Point(vertices[i].x, vertices[i].y, vertices[i].z)*R;
        vertices[i] = temp;
    }
    for(unsigned int i=0; i<normals.size();i++) {
        Point temp = Point::normalize((Point(normals[i].x, normals[i].y, normals[i].z)*R));
        normals[i] = temp;
    }
    freeList();
    bound();
    
}

void Poly::scale(float x, float y, float z) {
    for(unsigned int i=0; i<vertices.size();i++) {
        vertices[i].x *= x;
        vertices[i].y *= y;
        vertices[i].z *= z;
    }
    scaleNormals(x,y,z);
    freeList();
    bound();
}

void Poly::scaleNormals(float x, float y, float z){
    mat4 sInv = Transform::scaleNormalsMatrix(x,y,z);
    for(unsigned int i=0; i< normals.size(); i++) {
        //Point temp = Point::normalize((Point(normals[i].x, normals[i].y, normals[i].z)*sInv));
        //normals[i].x = temp.x;
        //normals[i].y = temp.y; 
        //normals[i].z = temp.z;
		normals[i]*sInv;
    }
    freeList();
}

void Poly::computeNormals() {
	
	
	 freeList();
	 normals.clear();
	 
	 if (faces.size() == 0) return;
	 
	 int index=0;
	 for(unsigned int i=0; i<faces.size();i++) { 
		 faces[i].ni.clear();
		 Vertex p1 = vertices[faces[i].vi[0]-1];
		 Vertex p2 = vertices[faces[i].vi[1]-1];
		 Vertex p3 = vertices[faces[i].vi[2]-1];
		 Vertex v1, v2, v;
		 if (faces[i].vi.size() == 3) {
			 v1 = Vertex( p2.x-p1.x, p2.y-p1.y, p2.z-p1.z); 
			 v2 = Vertex( p3.x-p1.x, p3.y-p1.y, p3.z-p1.z); 
			 v = Point::cross(v1, v2);
		 } else {
			 Vertex p4 = vertices[faces[i].vi[3]-1];
			 v1 = Vertex( p2.x-p1.x, p2.y-p1.y, p2.z-p1.z); 
			 v2 = Vertex( p4.x-p1.x, p4.y-p1.y, p4.z-p1.z); 
			 v = Point::cross(v1, v2);
		 }
		 
		 // ADD FACE NORMALS (not vertex yet... )
		 v.normalize();
		 Normal n(v.x, v.y, v.z);
		 normals.push_back(n);
		 index++;
		 faces[i].ni.push_back(index);
	 }
}

void Poly::computeSmoothNormals() {
    freeList();
    normals.clear();

    if (faces.size() == 0) return;

	for(unsigned int i=0; i<vertices.size();i++) { 
		normals.push_back(Point(0.0,0.0,0.0));
	}
    for(unsigned int i=0; i<faces.size();i++) { 
        faces[i].ni.clear();
        Vertex p1 = vertices[faces[i].vi[0]-1];
        Vertex p2 = vertices[faces[i].vi[1]-1];
        Vertex p3 = vertices[faces[i].vi[2]-1];
        Vertex v1, v2, v;
        if (faces[i].vi.size() == 3) {
            v1 = Vertex( p2.x-p1.x, p2.y-p1.y, p2.z-p1.z); 
            v2 = Vertex( p3.x-p1.x, p3.y-p1.y, p3.z-p1.z); 
            v = Point::cross(v1, v2);
        } else {
            Vertex p4 = vertices[faces[i].vi[3]-1];
            v1 = Vertex( p2.x-p1.x, p2.y-p1.y, p2.z-p1.z); 
            v2 = Vertex( p4.x-p1.x, p4.y-p1.y, p4.z-p1.z); 
            v = Point::cross(v1, v2);
        }

        // ADD FACE NORMALS (not vertex yet... )
        v.normalize();
        Normal n(v.x, v.y, v.z);
        normals[faces[i].vi[0]-1]+=n;
        normals[faces[i].vi[1]-1]+=n;
        normals[faces[i].vi[2]-1]+=n;
        faces[i].ni.push_back(faces[i].vi[0]-1);
        faces[i].ni.push_back(faces[i].vi[1]-1);
        faces[i].ni.push_back(faces[i].vi[2]-1);		
		if (faces[i].vi.size() > 3){
			normals[faces[i].vi[3]-1]+=n;
			faces[i].ni.push_back(faces[i].vi[3]-1);
		}

    }
	for(unsigned int i=0; i<normals.size();i++) {
		normals[i] = Point::normalize(normals[i]);
	}
}

void Poly::indexMeld(){
	freeList();
	point_t temp = vertices;
	vertices.clear();
	int index = 0;
	for(unsigned int i = 0; i<faces.size(); i++){
		Face f = faces[i]; 
		for (unsigned int j = 0; j<f.vi.size(); j++) {
			int found = -1;
			Point p = Point(temp[f.vi[j]-1].x,temp[f.vi[j]-1].y,temp[f.vi[j]-1].z);
			for (unsigned int k = 0; k<vertices.size(); k++) {
				if (fabs(p.x-vertices[k].x)<.001&&fabs(p.y-vertices[k].y)<.001&&fabs(p.z-vertices[k].z)<.001) {
					found = k+1;
					break;
				}
			}
			if(found!=-1){
				f.vi[j] = found;
				continue;
			}
			
			vertices.push_back(p);
			index++;
			f.vi[j] = index;
		}
		faces[i] = f;
	}
}

void Poly::readOBJ(String obj) {
    filepath = obj;

    FILE *objDesc = fopen(obj.c_str(), "r");
    if (!objDesc) {
        printf("! bad file descriptor for %s\n", obj.c_str());
        return;
    }


    char s[512];

    while((fgets(s, 512, objDesc)) != NULL) {
         tokenize string
        String line = String(s);
        vector<String>tokens;
        Term::Tokenize(line, tokens, " \n");
        if (!tokens.size()) continue;
        String command = *(tokens.begin());
         parse! 
        if ((command.size() == 1) && !command.compare(String("v"))) {
            tokens.erase(tokens.begin());
            if (tokens.size() != 3) continue;
            String arg1 = *(tokens.begin());
            String arg2 = *(tokens.begin()+1);
            String arg3 = *(tokens.begin()+2);
            float x, y, z;
            x = atof(arg1.c_str());
            y = atof(arg2.c_str());
            z = atof(arg3.c_str());
            Vertex v(x, y, z);
            addVertex(v);
        } else if ((command.size() == 2) && !command.compare(String("vt"))) {
            tokens.erase(tokens.begin());
            if (tokens.size() != 2) continue;
            String arg1 = *(tokens.begin());
            String arg2 = *(tokens.begin()+1);
            float x, y, z;
            x = atof(arg1.c_str());
            y = atof(arg2.c_str());
            if (tokens.size() == 3) {
                String arg3 = *(tokens.begin()+2);
                z = atof(arg3.c_str());
            } else z = 0.0f;
            TexCoord t(x, y, z);
            addTexCoord(t);
        } else if ((command.size() == 2) && !command.compare(String("vn"))) {
            tokens.erase(tokens.begin());
            if (tokens.size() != 3) continue;
            String arg1 = *(tokens.begin());
            String arg2 = *(tokens.begin()+1);
            String arg3 = *(tokens.begin()+2);
            float x, y, z;
            x = atof(arg1.c_str());
            y = atof(arg2.c_str());
            z = atof(arg3.c_str());
            Normal n(x, y, z);
            addNormal(n);
        } else if ((command.size() == 1) && !command.compare(String("f"))) {
            Face f = Face();
            tokens.erase(tokens.begin());
            if (tokens.size() < 3) continue;

             iterate over tokens in the face arguments 
            vector <String>::iterator cursor = tokens.begin();
            for(; cursor!=tokens.end(); cursor++) {
                String face = *cursor;
                const char * string = face.c_str();
                int v, t, n;
                 collect indices 
                if (sscanf(string, "%d/%d/%d", &v,&t,&n) == 3) {
                    f.vi.push_back(v);
                    f.ti.push_back(t);
                    f.ni.push_back(n);
                } else if (sscanf(string, "%d/%d", &v,&t) == 2) {
                    f.vi.push_back(v);
                    f.ti.push_back(t);
                } else if (sscanf(string, "%d//%d", &v,&n) == 2) {
                    f.vi.push_back(v);
                    f.ni.push_back(n);
                } else if (sscanf(string, "%d", &v) == 1) {
                    f.vi.push_back(v);
                } else {
                }
            }
            addFace(f); 
        }
    }
    base = vertices;
    bound();
}


void Poly::writeOBJ(String filename) {

    FILE * fd = fopen(filename.c_str(), "w");
    if (!fd) {
        printf("! cannot write %s\n", filename.c_str());
        return;
    }
    fprintf(fd, "# generated by vim3D (Dan Lynch and Barry Martin)\n");
    if (vertices.size() > 0) {
        vector <Vertex>::iterator cursorV;
        for(cursorV = vertices.begin(); cursorV != vertices.end(); cursorV++) {
            Vertex p = *cursorV;
            fprintf(fd, "v %f %f %f\n", p.x, p.y, p.z);
        }
    }
    if (normals.size() > 0) {
        vector <Normal>::iterator cursorN;
        for(cursorN = normals.begin(); cursorN != normals.end(); cursorN++) {
            Normal n = *cursorN;
            fprintf(fd, "vn %f %f %f\n", n.x, n.y, n.z);
        }
    }
    if (textures.size() > 0) {
        vector <TexCoord>::iterator cursorT;
        for(cursorT = textures.begin(); cursorT != textures.end(); cursorT++) {
            TexCoord t = *cursorT;
            fprintf(fd, "vt %f %f %f\n", t.x, t.y, t.y);
        }
    }
    if (faces.size() > 0) {
        vector <Face>::iterator cursorF;
        for(cursorF = faces.begin(); cursorF != faces.end(); cursorF++) {
            Face f = *cursorF;

            if (f.vi.size() > 0) {
                 write out a face 
                fprintf(fd, "f ");
                vector<int>::iterator skee;
                unsigned int z=0;

                 iterate over a polygonal face 
                for(skee=f.vi.begin(); skee!=f.vi.end(); skee++,z++) {

                    unsigned int t,n;
                    t=n=0;
                     get textures coordinates and normals if any (indices) 
                    n = (f.ni.size() > z) ? f.ni[z] : 0;
                    t = (f.ti.size() > z) ? f.ti[z] : 0;


                    if (f.vi[z] == 0) {
                        fprintf(fd, "%d", f.vi[z]);
                    } else if ((t == 0) && (n == 0)) {
                        fprintf(fd, "%d", f.vi[z]);
                    } else if ((f.ti.size() > z ) && (f.ti[z] == 0)) {
                        fprintf(fd, "%d//%d", f.vi[z], n);
                    } else if ((f.ni.size() > z) && (f.ni[z] == 0)) {
                        fprintf(fd, "%d/%d", f.vi[z], t);
                    } else {
                        fprintf(fd, "%d/%d/%d", f.vi[z], t, n);
                    }
                    fprintf(fd, " ");
                }
                fprintf(fd, "\n");
            }
        }
    }
    fclose(fd);

}
float basicnoise(float v) {

    float left=floor(v);
    float right=ceil(v);

    //percentage of the way through the chunk

    float perc = v - floor(v);
    if (left == 0) perc = v - int(v);

    int rnd1 = rand() % 10 + 1;
    int rnd2 = rand() % 10 + 1;
    float r1 = float(rnd1)/10 * left;
    float r2 = float(rnd2)/10 * right;
    perc = 3 * perc * perc - 2 * perc * perc * perc;
    return r1 * perc + (1-perc) * r2;
}

void Poly::noise(float freqMult, float scale, float detailScale, float pers *min .25*) {

    freeList();
	noiseFunc = new perlinNoise();
	
	float firstFreq = noiseFunc->frequency*freqMult;
	float secFreq = firstFreq*freqMult*.1;
	float firstPers = (1.0/8.0<1.0/64.0/pers)?1.0:1.0/64.0/pers;
					   ///(1.0/64.0<1.0/512.0/pers)?1.0/16.0:1.0/512.0/pers;
	//float firstPers = (1.0/64.0<1.0/512.0/pers)?1.0/16.0:1.0/512.0/pers;
	float secPers = firstPers * 8;
	//printf("pers %f\n", firstPers);
    for(unsigned int i=0; i<vertices.size();i++) {
		//printf("noiseFunc=%f\n", noiseFunc->perlinNoise2D(vertices[i].x, vertices[i].z));
		noiseFunc->freqOctDep = false;
		noiseFunc->ampOctDep = false;
		noiseFunc->useSum = false;
		noiseFunc->frequency = firstFreq;
		noiseFunc->persistence = firstPers;
        vertices[i].y += 50*scale*Terrain::getElevation(noiseFunc, vertices[i].x, vertices[i].z);
		noiseFunc->useSum = true;
		noiseFunc->freqOctDep = true;
		noiseFunc->ampOctDep = true;		
		noiseFunc->frequency = secFreq;
		noiseFunc->persistence = secPers;
		vertices[i].y +=20*scale*detailScale*Terrain::getElevation(noiseFunc, vertices[i].x, vertices[i].z);
		//basicnoise(15*vertices[i].y + 10);
    }
	//computeNormals();
	//textureMap();

}

void Poly::cut(unsigned int n, float size) {
  if (faces.size() > n) {
    vector<int>indices = faces[n].vi;
    faces.erase( faces.begin() + n );
  }
}

void Poly::close() {
    point_t points = vertices;
    clearAll();
    vertices = points;
    Face f = Face();
    for(unsigned int i=1; i<=vertices.size();i++) {
       f.vi.push_back(i);
    }
    addFace(f);
}

void Poly::transform(float x, float y, float z) {
    freeList();
    for(unsigned int i=0;i<vertices.size();i++) {
        vertices[i].x -= x;
        vertices[i].y -= y;
        vertices[i].z -= z;
    } 

}
void Poly::centroid() { bound(); transform(cx,cy,cz); }
void Poly::xmin() { bound(); transform(minX, 0, 0); }
void Poly::xmax() { bound(); transform(maxX, 0, 0); }
void Poly::ymin() { bound(); transform(0, minY, 0); }
void Poly::ymax() { bound(); transform(0, maxY, 0); }
void Poly::zmin() { bound(); transform(0, 0, minZ); }
void Poly::zmax() { bound(); transform(0, 0, maxZ); }


void Poly::multiplyTextureCoords(float s, float t) {
    for(unsigned int i=0; i<textures.size(); i++) {
        textures[i].x*= s;
        textures[i].y*= t;
    }
    freeList();
}

void Poly::basicTextureCoords(float s, float t) {
    freeList();

    for(unsigned int i=0;i<faces.size();i++) {
        if (faces[i].vi.size() == 4) {
             if there are texture coordinates, clear them out 
               if (faces[i].ti.size() > 0) {
               vector<int>::iterator ind=faces[i].ti.begin();
               for(;ind!=faces[i].ti.end();ind++) { 
               if (textures.size() > *ind) textures.erase(textures.begin() + *ind);
               }
               }
               ahh! dont do it, will screw up later indices 
            faces[i].ti.clear();

            TexCoord t1(0.0f,0.0f); addTexCoord(t1); faces[i].ti.push_back(textures.size());
            TexCoord t2(0.0f,t); addTexCoord(t2); faces[i].ti.push_back(textures.size());
            TexCoord t3(s,t); addTexCoord(t3); faces[i].ti.push_back(textures.size());
            TexCoord t4(s,0.0f); addTexCoord(t4); faces[i].ti.push_back(textures.size());


        } else {
            
            float maxX, maxY, maxZ, minX, minY, minZ;
            maxX=maxY=maxZ=minX=minY=minZ=0.0f;
            bool init = false;
            for(unsigned int j=0;j<faces[i].vi.size();j++) {
                if (!init) {
                    init = true;
                    minX = vertices[faces[i].vi[j]-1].x;
                    maxX = vertices[faces[i].vi[j]-1].x;
                    minY = vertices[faces[i].vi[j]-1].y;
                    maxY = vertices[faces[i].vi[j]-1].y;
                    minZ = vertices[faces[i].vi[j]-1].z;
                    maxZ = vertices[faces[i].vi[j]-1].z;
                } else {
                    if (vertices[faces[i].vi[j]-1].x < minX) minX = vertices[faces[i].vi[j]-1].x;
                    if (vertices[faces[i].vi[j]-1].x < maxX) maxX = vertices[faces[i].vi[j]-1].x;
                    if (vertices[faces[i].vi[j]-1].y < minY) minY = vertices[faces[i].vi[j]-1].y;
                    if (vertices[faces[i].vi[j]-1].y < maxY) maxY = vertices[faces[i].vi[j]-1].y;
                    if (vertices[faces[i].vi[j]-1].z < minZ) minZ = vertices[faces[i].vi[j]-1].z;
                    if (vertices[faces[i].vi[j]-1].z < maxZ) maxZ = vertices[faces[i].vi[j]-1].z;
                }
            }
            
            for(unsigned int j=0;j<faces[i].vi.size();j++) {
                //TexCoord t(float(j)/float(faces[i].vi.size()), float(j)/float(faces[i].vi.size()));
                TexCoord t(0,0);
                addTexCoord(t); faces[i].ti.push_back(textures.size());
            }
        }
    }
}

Poly::Poly(String filename) { 
    filepath = filename;
    showBounds=false;
    displayListLoaded=false;
    name = Scene::name(String("Poly"));
    readOBJ(filename);
    type = POLYGON_TYPE;
}

Poly::Poly() {
    showBounds=false;
    displayListLoaded=false;
    name = Scene::name(String("Poly"));
    type = POLYGON_TYPE;
}

Poly::~Poly() {

    printf("destroying Poly()\n");

    if (normals.size() > 0) {
        printf("destroying Poly.normals size: %d\n", (int) normals.size());
        normals.clear();
    }
    if (vertices.size() > 0) {
        printf("destroying Poly.vertices size: %d\n", (int) vertices.size());
        vertices.clear();
    }
    if (textures.size() > 0) {
        printf("destroying Poly.textures size: %d\n", (int) textures.size());
        textures.clear();
    }
    if (faces.size() > 0) {
        printf("destroying Poly.faces size: %d\n", (int) faces.size());
        faces.clear();
    }

}

*/