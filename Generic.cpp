#include "Generic.h"
#include "Scene.h"

Generic::Generic() {
  type = GENERIC_TYPE;
  active = false;
  filepath = String();
  name = String();
  id = Scene::newID();
}

String Generic::printInfo() {
    char s[256];
    sprintf(s, "id[%d]:%s %s%s\n", id, name.c_str(), filepath.c_str(), (active)?"(selected)":"");
    return String(s);
}


bool Generic::isObject(Generic *o) {return( isGeo(o) | isCamera(o) | isLight(o)  );}
bool Generic::isGeo(Generic *o) {return(o!=NULL && o->type==GEOMETRY_TYPE);}
bool Generic::isCamera(Generic *o) {return(o!=NULL && o->type==CAMERA_TYPE);}
bool Generic::isLight(Generic *o) {return(o!=NULL && o->type==LIGHT_TYPE);}
bool Generic::isPoly(Generic *o) {return(o!=NULL && o->type==POLYGON_TYPE);}
bool Generic::isChannel(Generic *o) {return(o!=NULL && o->type==CHANNEL_TYPE);}
bool Generic::isTexture(Generic *o) {return(o!=NULL && o->type==TEXTURE_TYPE);}
bool Generic::isImage(Generic *o) {return(o!=NULL && o->type==IMAGE_TYPE);}


Generic::~Generic(){}
