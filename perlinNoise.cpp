/*
 *  perlinNoise.cpp
 *  
 *
 *  Created by Barry Martin on 3/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "perlinNoise.h"

perlinNoise::perlinNoise(){
	persistence = 0.25f;
	numberOfOctaves = 4;
	frequency = 0.02f;
	amplitude = 1;
	freqOctDep = false;
	ampOctDep = false;
	useSum = false;
}

float perlinNoise::perlinNoise2D(float x, float y){
		
	float freq = frequency;
	float amp = amplitude;
	float total = 0.0f;
	int n = numberOfOctaves;
	
	for(int i=0;i<n;i++){
		if (freqOctDep) freq *=2;
		if(ampOctDep) amp*=persistence;
		total = total + interpolatedNoise2D(x * freq, y * freq) * amp;
	}
	
	return total;
}

float perlinNoise::interpolatedNoise2D(float x, float y){
	int intX = int(x);
	int intY = int(y);
	float fracX = x-intX;
	float fracY = y-intY;
	
	float n1 = smoothNoise2D(intX, intY);
	float n2 = smoothNoise2D(intX+1, intY);
	float n3 = smoothNoise2D(intX, intY+1);
	float n4 = smoothNoise2D(intX+1, intY+1);
	
	float i1 = interpolate(n1, n2, fracX);
	float i2 = interpolate(n3, n4, fracX);
	
	float i3 = interpolate(i1, i2, fracY);
	
	return i3;
}

float perlinNoise::interpolate(float a, float b, float x){
	float c1 = 3*pow(1-x, 2) - 2*pow(1-x,3);
	float c2 = 3*pow(x, 2) - 2*pow(x, 3);
	
	return a*c1 + b*c2;
}

float perlinNoise::smoothNoise2D(float x,float y){
	float corners = (noise2D(x-1, y-1) + noise2D(x+1, y-1) + noise2D(x-1, y+1) + noise2D(x+1, y+1)) / 16;
    float sides   = (noise2D(x-1, y)  + noise2D(x+1, y)  + noise2D(x, y-1)  + noise2D(x, y+1)) /  8;
    float center  =  noise2D(x, y) / 4;
    return corners + sides + center;	
}

//change all the primes in each differently
float perlinNoise::noise2D(float x, float y){
	float temp = 0;
	if (useSum) {
		temp = noise2D2(x, y) + noise2D3(x, y);
	}
	int n;
	n = int( x + y * 57.0f);
	n = (n<<13) ^ n;
	float res = (float)( 1.0 - ( (n * (n * n * 15731 + 789221)
								  + 1376312589) & 0x7fffffff ) / 1073741824.0);
	if (useSum) {
		res += temp;
		res /= 3;
	}
	return res;
}

float perlinNoise::noise2D2(float x, float y){
	int n;
	n = int( x + y * 57.0 );
	n = (n<<13) ^ n;
	float res = (float)( 1.0 - ( (n * (n * n * 15731 + 789221)
								  + 1500450271 ) & 0x7fffffff ) / 1073741824.0);
	return res;
}

float perlinNoise::noise2D3(float x, float y){
	int n;
	n = int(x + y * 57.0);
	n = (n<<13) ^ n;
	float res = (float)( 1.0 - ( (n * (n * n * 15731 + 789221)
								  + 3267000013) & 0x7fffffff ) / 2860486313.0);
	return res;
}

float perlinNoise::perlinNoise3D(float x, float y, float z){
	return 0.0f;
}


perlinNoise::~perlinNoise(){
}

