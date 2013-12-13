#include "Light.h"
#include "Scene.h"
#include "OpenGLinc.h"

Light::Light() {
    type = LIGHT_TYPE;
    specular[0] = 1.0f;
    specular[1] = 1.0f;
    specular[2] = 1.0f;
    specular[3] = 1.0f;
    ambient[0] = 0.1f;
    ambient[1] = 0.1f;
    ambient[2] = 0.1f;
    ambient[3] = 0.1f;
    diffuse[0] = 1.0f;
    diffuse[1] = 1.0f;
    diffuse[2] = 1.0f;
    diffuse[3] = 1.0f;

    /* spot light stuff */
    spotDir[0] = spotDir[1] = 0.0f;
    spotDir[2] = -1.0f; // openGL default is 0,0,-1
    cutoff = 60.0f;
    spotExp = 2.0;
    /* attenuation */
    constant = 1.0f;
    linear = 0.1f;
    quadratic = 0.00f; // small values like 0.04 if you use this at all


    ltype = L_POINT;

    on = true;

    String l = String("Light");
    this->name = Scene::name(l);
    id = 0;
    for(unsigned int i=0;i<Scene::allOBJS.size();i++) {
        if (!isLight(Scene::allOBJS[i])) continue;
        if (Scene::allOBJS[i]->id == id) {
            if (Scene::allOBJS[i] == this) continue;
            id++;//i=-1;
        }
    }

}

String Light::propertyInfo() {
    char s[1024];
    sprintf(s, "name:%s\nltype:  %d\nattenuations: constant: %f, linear: %f, quadratic: %f\ncutoff: %f\nspotDir:  %f %f %f spotExp: %f\ndiffuse:  %f %f %f %f\nspecular: %f %f %f %f\nambient:  %f %f %f %f\n",
            name.c_str(), ltype, constant, linear, quadratic, 
            cutoff, spotDir[0],spotDir[1], spotDir[2], spotExp,
            diffuse[0], diffuse[1], diffuse[2], diffuse[3],
            specular[0], specular[1], specular[2], specular[3],
            ambient[0], ambient[1], ambient[2], ambient[3]);
    return String(s);
}

void Light::display(int pass) {
    displayPass = pass;
    display();
}

void Light::display() {
    GLint polygon_mode;
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(M.mat_array);

    /* display purposes */
    if (((curSTATE & LIGHTS_DISP) || active) && (curSTATE & SHOW_SELECT)) {
        glPushMatrix();
        glColor3f(diffuse[0],diffuse[1],diffuse[2]);
        //glMultMatrixf(M.mat_array);
        
        glRotatef(90,0,1,0);
        if(ltype != L_POINT) glutSolidCone(4.0f, 6.0f, 15, 15);
        glPushAttrib(GL_LIGHTING_BIT);
        Scene::toggleLights(false);
        if (active&&ltype!=L_POINT) glutWireCone(4.0f, 6.0f, 15, 15);
        glColor3f(specular[0], specular[1], specular[2]);
        glutSolidSphere(3.0f, 15,15);
        Scene::toggleLights(true);
        glPopAttrib();
        glPopMatrix();
    }


    /* display all child objects */
    obj_t::iterator csr=objs.begin();
    for(;csr!=objs.end();csr++){
        Object * o = *csr;
        o->display(displayPass);
    }
    // PROBLEM??? maybe because we should calculate a new position based on combined
    // heirarchy of positions and transslations...
    if (polygon_mode == GL_FILL) {
        /* xyzw - homogeneous coordinates. if w=0, then directional light */
        //GLdouble mat[16];
        //glGetDoublev(GL_MODELVIEW_MATRIX, mat);
        //GLfloat light_position[] = {M.a03+mat[3], M.a13+mat[7], M.a23+mat[11], ltype};
        GLfloat lpos[4];
        if (ltype == L_DIR) {
            lpos[0] = M.a03;
            lpos[1] = M.a13;
            lpos[2] = M.a23;
        } else if (ltype == L_POINT) {
            lpos[0] = 0.0f;
            lpos[1] = 0.0f;
            lpos[2] = -1.0f;
        } else {
            lpos[0] = 0.0f;
            lpos[1] = 0.0f;
            lpos[2] = -1.0f;
        }
        if (ltype == L_DIR) lpos[3] = 0.0f;
        else lpos[3] = 1.0f;
        //GLfloat light_position[] = {M.a03, M.a13, M.a23, ltype};
        //GLfloat light_position[] = {0.0f, 0.0f, 0.0f, ltype};

        GLint li = Scene::light_lookup[this->id % GL_MAX_LIGHTS];
        glLightfv(li, GL_AMBIENT, ambient);
        glLightfv(li, GL_SPECULAR, specular);
        glLightfv(li, GL_DIFFUSE, diffuse);
        glLightfv(li, GL_POSITION, lpos);
        if (ltype == L_SPOT) { // directional (infinite) is 0
            glLightfv(li, GL_SPOT_DIRECTION, spotDir);
            glLightf(li, GL_SPOT_CUTOFF, cutoff);
            glLightf(li, GL_SPOT_EXPONENT, spotExp);
        }
        glLightf(li, GL_CONSTANT_ATTENUATION, constant);
        glLightf(li, GL_LINEAR_ATTENUATION, linear);
        glLightf(li, GL_QUADRATIC_ATTENUATION, quadratic);

        if (on) glEnable(li);
        if (!(curSTATE & LIGHTS_ON) || !on) glDisable(li);




    }
    glPopMatrix();

}
void Light::move(float yDegrees, float xDegrees, Point camDir){
    Point perpDir = Point::cross(Point(0, 1, 0), camDir);
    camDir.normalize();
    perpDir.normalize();
    camDir = camDir*yDegrees;
    perpDir = perpDir*xDegrees;
    float scale = 0.2f; //relate to distance from camera
    M.a23 += scale*(camDir.z+perpDir.z);
    M.a03 += scale*(camDir.x+perpDir.x);

}
void Light::moveUp(float deg){
    float scale = .3f;
    M.a13 += scale*deg;
}


void Light::rotateLight(float degrees, Point axis){
    float scale = 1.0f;
    float the = degrees*3.1451f/180.0f*scale;
    mat4 rot4 = rotateMat4(the, axis);

    M = M*rot4;
}

Light::~Light(){}
