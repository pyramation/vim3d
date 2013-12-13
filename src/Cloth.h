#ifndef CLOTH_H
#define CLOTH_H

#include "Poly.h"

class Springs {
public:
	//vector <int> vi;
	//vector <int> ni;
	//vector <int> ti;
	vector <int> neighbors;
	Springs();
};

class Cloth;
//typedef vector <Cloth *> cloth_t;
//class Poly;

class Cloth : public Poly {
	public:
	// last(current) time, dimensions and spring rest distance
	float lastTime, nSep, dSep, sSep;
	int width, height;
	// spring constants
	int kN, kD, kS;
	int timeScalar;
	
	vector <Springs> nearestSprings;
	vector <Springs> skipSprings;
	vector <Springs> diagonalSprings;
	vector <Point> velocities;
	
	perlinNoise * noiseFunc2;
	
	void updateDistribution(int Time);
	
	Cloth();
	virtual ~Cloth();
};

#endif

/*
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "Packages.h"
#include "Elements.h"

#include "Generic.h"

#include "perlinNoise.h"
#include "Terrain.h"
*/
/* this struct for mapping faces in obj files */
/*
class Face {
    public:
        vector <int> vi;
        vector <int> ni;
        vector <int> ti;
        vector <int> neighbors;
        Face();
};

class Poly;
typedef vector <Poly *> poly_t;
class Geo;

class Poly : public Generic {
    public:

        vector <Normal> normals;
        vector <Vertex> vertices;
        vector <Vertex> base;
        vector <TexCoord> textures;
        vector <Face> faces;
		
		perlinNoise * noiseFunc;

        void bound();
        void bounds();
        bool showBounds;
		void invertNormals();

        void freeList();
        GLuint listIndex;
        bool displayListLoaded;

        void addVertex(Vertex v);
        void addNormal(Normal n);
        void addTexCoord(TexCoord t);

        void addFace(Face f);
        void addFace(Vertex a, Vertex b, Vertex c, Vertex d);
		void addFace(Vertex a, Vertex b, Vertex c);
        void addFace(vector<int>&indices);

        void noise(float freqMult, float scale, float detailScale, float pers);
        void close();
        void cut(unsigned int n, float size);
        void ExtrudeFace(unsigned int n, Point dir);
        void ExtrudeFace(vector<int>&start, vector<int>&end);
        void transferCopy(vector<int>&indices, vector<int>&container, Point start, Point mid, Point end);
        void curveMe(Geo * crossSec);

        void readOBJ(String obj);
        void writeOBJ(String obj);
        void getOBJFace(Face * f, char * string, int index);
        void readFile(String obj);

        void visualizePoints();
        void visualizeFaces();


        float maxX; float minX; float maxY; float minY; float maxZ; float minZ;
        float cx, cy, cz;
        void centroid();
        void ymax(); void ymin(); void xmax(); void xmin(); void zmax(); void zmin();
        void transform(float x, float y, float z);
        void scale(float x, float y, float z);
        void scaleNormals(float x, float y, float z);
        void rotate(float degress, float x, float y, float z);

        void multiplyTextureCoords(float s, float t);
        void basicTextureCoords(float s, float t); 

        void clearAll();
        void Bezier(int levelOfDetail);
        void Bezier2(int levelOfDetail);
        void Bspline(int levelOfDetail);
        void Revolve(float degs, float x, float y, float z);
        void Extrude(float x, float y, float z);
		void Sweep(class Geo * curveGeo, class Geo * crossSec, bool keepPath, bool isWall);
        void computeNormals();
		void computeSmoothNormals();
		void indexMeld();
		void textureMap();


        void cookGeo();
        void displayGeometry();

        Poly(String filename);
        Poly();
        virtual ~Poly();

 
 */


