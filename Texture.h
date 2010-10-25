#ifndef TEXTURE_H
#define TEXTURE_H

#include "OpenGLinc.h"
#include "Packages.h"
#include <stdio.h>
#include <assert.h>

#include "Generic.h"
#include "Image.h"

class Image;

class Texture;
typedef vector<Texture *> tex_t;

class Texture : public Generic {
public:
    GLubyte *texture;
    GLuint texName;
    GLenum err;

    Image * image;

    int h,w;


    static Texture * addTexture(String filename);

    void checkers();

    void readTexture();
    bool textureLoaded;

    void initializeTexture();
    void EnableBind();
    void Disable();
    

    Texture();
	virtual ~Texture();

};

#endif

