/*
 *  Terrain.cpp
 *  
 *
 *  Created by Barry Martin on 3/31/10.
 *
 */

#include "Terrain.h"

Terrain::Terrain(){
}

float Terrain::getElevation(perlinNoise * p, float x, float y){
	return getEl(p, x, y);
}
float Terrain::getElevation(perlinNoise *  p, float x, float y, float persistence, float frequency){
	return getEl(p, x, y);		
}
float Terrain::getEl(perlinNoise * p, float x, float y){
	return p->perlinNoise2D(x, y);
}

Terrain::~Terrain(){

}

