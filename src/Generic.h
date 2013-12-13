#ifndef GENERIC_H
#define GENERIC_H

#include "Packages.h"

#define GENERIC_TYPE    0
#define OBJECT_TYPE     1
#define CAMERA_TYPE     2
#define LIGHT_TYPE      3
#define GEOMETRY_TYPE   4
#define POLYGON_TYPE    5
#define TEXTURE_TYPE    6
#define CHANNEL_TYPE    7
#define IMAGE_TYPE      8

class Generic;
typedef vector <Generic *> gen_t;

class Generic {
 public:

    String printInfo();
    //virtual String printInfo();
    bool active;
    int type,id;

    String filepath;
    String name;

    static bool isObject(Generic *o);
    static bool isGeo(Generic *o);
    static bool isCamera(Generic *o);
    static bool isLight(Generic *o);
    static bool isChannel(Generic *o);
    static bool isTexture(Generic *o);
    static bool isPoly(Generic *o);
    static bool isImage(Generic *o);
    
    Generic();
    virtual ~Generic();

};


#endif
