#include <math.h>
#include "Texture.h"
#include "Scene.h"

Texture::Texture() {
    active = false;
    textureLoaded = false;
    filepath = String();
    name = Scene::name(String("Texture"));
    type = TEXTURE_TYPE;
    image=NULL;
}

Texture * Texture::addTexture(String filename) {
    Texture * tex = NULL;
    if (!(tex=T(Scene::getObjectByFile(filename)))) {
        tex = new Texture();
        tex->filepath = filename;
        Scene::addTexture(tex);
    }
    return tex;
}

void Texture::checkers() {
    if (!textureLoaded) {
        printf("making checkers\n");
        w = h = 100;
        texture = new GLubyte[4*w*h];
        printf("h: %d, w: %d\n", h, w);
        for (int i = 0 ; i < h*w ; i++) {
            texture[i*4 + 0] = (GLubyte) i*4 % 255;
            texture[i*4 + 1] = (GLubyte) i*3 % 255;
            texture[i*4 + 2] = (GLubyte) i % 255;
            texture[i*4 + 3] = 255;
        }
        textureLoaded = true;
    }
}


void Texture::readTexture() {
    if (!textureLoaded) {
        if (!filepath.length()) {
            printf("no filepath for texture\n");
            return;
        }
        printf("Reading texture...%s\n",filepath.c_str());
        FILE *fp ; int i;
        if((fp = fopen(filepath.c_str(),"rb"))) {
            //fscanf(fp,"%*s %*d %*d %*d%*c") ;
            char s[48];
            fgets(s,48, fp); // read P6
            fgets(s,48, fp); // read width and height
            sscanf(s,"%d %d", &w, &h);
            texture = new GLubyte[4*w*h];
            printf("h: %d, w: %d\n", h, w);
            fgets(s,48, fp);
            fgets(s,48, fp);
            for (i = 0 ; i < h*w ; i++) {
                fscanf(fp,"%c",&(texture[i*4 + 2]));
                fscanf(fp,"%c",&(texture[i*4 + 1]));
                fscanf(fp,"%c",&(texture[i*4 + 0]));
                texture[i*4 + 3] = (GLubyte) 255;
            }
            fclose(fp) ;
        } else {
            checkers();
        }
    }
}


void Texture::initializeTexture() {
    if (!textureLoaded) {
        readTexture();

        glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        /* Now, set up all the stuff for texturing, per page 399 */
        glGenTextures(1, &texName) ;
        glBindTexture(GL_TEXTURE_2D, texName) ;

        bool mipmaps = false;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) ;
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) ;
        if (mipmaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR) ;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, texture);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture) ;
        }
        /* don't this this anymore */
        delete texture;
        textureLoaded = true;
        printf("texture initialized...\n");
    }
}

void Texture::EnableBind() {
    glEnable(GL_TEXTURE_2D) ;
    // GL_REPLACE -> no lighting
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
    glBindTexture(GL_TEXTURE_2D, texName) ;
    //glColor3f(0.0,0.0,0.0) ;
    //err = glGetError() ; assert(err == GL_NO_ERROR) ;
}

void Texture::Disable() {
    //err = glGetError() ; assert(err == GL_NO_ERROR) ;
    glDisable(GL_TEXTURE_2D) ;
}

Texture::~Texture() { if (textureLoaded) glDeleteTextures(1, &texName); }
