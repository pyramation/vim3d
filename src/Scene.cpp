#include "Scene.h"

#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <algorithm>

#include "Utils.h"
#include "Term.h"
#include "Func.h"
#include "vectorUtils.h"
#include "Directory.h"
#include "Operations.h"
#include "Render.h"
#include "Ray.h"
#include "Cloth.h"


Scene::Scene() {}

Scene::~Scene() {}

Server * Scene::server = NULL;
Client * Scene::client = NULL;

WindowManager * Scene::window;
Camera * Scene::camera;

unsigned long int Scene::start=0;
unsigned long int Scene::end=81000;

int Scene::gridSize = 200;
int Scene::gridStep = 20;

float Scene::bgcolor[4] = {0,0,0,0};
float Scene::gridcolor[3] = {1,1,0};
float Scene::textcolor[3] = {1,1,1};

int Scene::amount = 0;
int Scene::x_ = 0;
int Scene::y_ = 0;
int Scene::btn_ = 0;
int Scene::state_ = 0;
int Scene::w = 480;
int Scene::h = 640;
unsigned int Scene::id=0;

int Scene::displayState = DEFAULT_STATE;
int Scene::interactiveState = NO_CURR_STATE;
int Scene::drawingState = NO_INTERP;

GLint Scene::light_lookup[] = {
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
    GL_LIGHT7
};

String outbuf_ = String();

long unsigned int Scene::time=0;


bool Scene::toggleRMouse = false;
bool Scene::toggleMMouse = false;

Generic * selOBJ = NULL;

vector<animation_t>Scene::animations=vector<animation_t>();
vector<reference_t>Scene::references=vector<reference_t>();

Curve *Scene::curve=NULL;
gen_t Scene::allObjects = gen_t();

unsigned int Scene::newID() { id++; return id; }

String Scene::name(String name) {
    int i=0;
    String n=String();
    for(;;) {
        char num[40];
        sprintf(num, "%d", i++);
        
        n = String(name + String(num));
        if(!Scene::nameExists(allOBJS,n)) {
            break;
        }
    }
    return n;
}

void Scene::passiveMotion(int x, int y){
    if (ivMODE & EDIT_MODE) {
        Point a = window2scene(x, y);
        if (curve) curve->updateActivePoint(a.x,a.y,a.z); 
    }
}

void Scene::timerFunc(int value) {

    //if (server) server->check_fds();

    if (curSTATE & PLAY) {
        if (curSTATE & RAYTRACING) {
            char s[512];
            sprintf(s, "Out/render%05lu.ppm", time);
            Render::RayTrace(String(s));
            if (time >= Render::end) curSTATE&=~RAYTRACING;
        }
        if (curSTATE & RENDERING) {
            char s[512];
            sprintf(s, "Out/screen%05lu.ppm", time);
            Render::CurrentScreen(String(s), 0,0,w,h);
            if (time >= Render::end) curSTATE&=~RENDERING;
        }
        time++;
        animCallbackFunc();
        refCallbackFunc();
        glutPostRedisplay();
    }
    glutTimerFunc(1000/SCENE_FPS, timerFunc, 1);
}

float getAnim(vector <animation_t> &animations, int i, long unsigned int time) {
    return animations[i].scalar*animations[i].chan->points[(int (time*animations[i].freqscalar + animations[i].timeShift)) % animations[i].chan->points.size()].y + animations[i].offset; }
#define diff(i) getAnim(animations, i, time-1)-getAnim(animations, i, time)
#define anim(i) getAnim(animations, i, time)
#define animOBJ animations[i].object
    float clamp01(float a) { if (a < 0.0f) return 0.0f; if (a > 1.0f) return 1.0f; return a;}

    void Scene::animCallbackFunc() {
        float degs = 0.0f;
        if (animations.size() <= 0) return;
        for(unsigned int i=0;i<animations.size();i++) {
            if (!(animations[i].start <= time && time <= animations[i].end)) continue;
            switch(animations[i].param) {
                case TX: if (isCamera(animOBJ)) C(animOBJ)->eye.x = anim(i); else animOBJ->M.a03 = anim(i); break;
                case TY: if (isCamera(animOBJ)) C(animOBJ)->eye.y = anim(i); else animOBJ->M.a13 = anim(i); break;
                case TZ: if (isCamera(animOBJ)) C(animOBJ)->eye.z = anim(i); else animOBJ->M.a23 = anim(i); break;
                case RX: degs = diff(i); if (isCamera(animOBJ)) { C(animOBJ)->tilt(degs);} else if (isObject(animOBJ)) { Point a (1,0,0); O(animOBJ)->rotateObj(degs, a);} break;
                case RY: degs = diff(i); if (isCamera(animOBJ)) { C(animOBJ)->pan(degs);} else if (isObject(animOBJ)) { Point a (0,1,0); O(animOBJ)->rotateObj(degs, a);} break;
                case RZ: degs = diff(i); if (isCamera(animOBJ)) { C(animOBJ)->roll(degs);} else if (isObject(animOBJ)) { Point a (0,0,1); O(animOBJ)->rotateObj(degs, a);} break;
                case SX: break;
                case SY: break;
                case SZ: break;
                case LOOKAT: break;
                             /* diffuse */
                case KD_R: if (isGeo(animOBJ)) G(animOBJ)->diffuse[0] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->diffuse[0] = clamp01(anim(i)); break;
                case KD_G: if (isGeo(animOBJ)) G(animOBJ)->diffuse[1] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->diffuse[1] = clamp01(anim(i)); break;
                case KD_B: if (isGeo(animOBJ)) G(animOBJ)->diffuse[2] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->diffuse[2] = clamp01(anim(i)); break;
                case KD_A: if (isGeo(animOBJ)) G(animOBJ)->diffuse[3] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->diffuse[3] = clamp01(anim(i)); break;
                               /* specular */
                case KS_R: if (isGeo(animOBJ)) G(animOBJ)->specular[0] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->specular[0] = clamp01(anim(i)); break;
                case KS_G: if (isGeo(animOBJ)) G(animOBJ)->specular[1] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->specular[1] = clamp01(anim(i)); break;
                case KS_B: if (isGeo(animOBJ)) G(animOBJ)->specular[2] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->specular[0] = clamp01(anim(i)); break;
                case KS_A: if (isGeo(animOBJ)) G(animOBJ)->specular[3] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->specular[3] = clamp01(anim(i)); break;

                               /* ambient */
                case KA_R: if (isGeo(animOBJ)) G(animOBJ)->ambient[0] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->ambient[0] = clamp01(anim(i)); break;
                case KA_G: if (isGeo(animOBJ)) G(animOBJ)->ambient[1] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->ambient[1] = clamp01(anim(i)); break;
                case KA_B: if (isGeo(animOBJ)) G(animOBJ)->ambient[2] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->ambient[2] = clamp01(anim(i)); break;
                case KA_A: if (isGeo(animOBJ)) G(animOBJ)->ambient[3] = clamp01(anim(i)); else if (isLight(animOBJ)) L(animOBJ)->ambient[3] = clamp01(anim(i)); break;

                               /* shininess */
                case KSHINE: if (isGeo(animOBJ)) G(animOBJ)->shininess[0] = anim(i); break;
				
					/*cloth*/
				case UPDATE: ((Cloth*)G(animations[i].object)->p)->updateDistribution(time); break;
            }
        }
    }

#define refOBJ references[i].refObject
float getRefValue(vector <reference_t> &references, int i, int param, long unsigned int time) {
    float val = 0.0f;
    switch(param) {
        case TX: if (isCamera(refOBJ)) val = C(refOBJ)->eye.x; else if (isObject(refOBJ)) val = O(refOBJ)->M.a03; else if (isPoly(refOBJ)) val = PL(refOBJ)->vertices[time % PL(refOBJ)->vertices.size()].x; break;
        case TY: if (isCamera(refOBJ)) val = C(refOBJ)->eye.y; else if (isObject(refOBJ)) val = O(refOBJ)->M.a13; else if (isPoly(refOBJ)) val = PL(refOBJ)->vertices[time % PL(refOBJ)->vertices.size()].y; break;
        case TZ: if (isCamera(refOBJ)) val = C(refOBJ)->eye.z; else if (isObject(refOBJ)) val = O(refOBJ)->M.a23; else if (isPoly(refOBJ)) val = PL(refOBJ)->vertices[time % PL(refOBJ)->vertices.size()].z; break;
    }
    return references[i].scalar * val + references[i].offset;
}
#define ref(i) getRefValue(references, i, references[i].paramRef, ((references[i].freqscalar*time)+references[i].timeShift))
#define tOBJ references[i].targObject
void Scene::refCallbackFunc(){
    if (references.size()<=0)return; 

    for(unsigned int i=0; i<references.size();i++) {
        if (!(references[i].start <= time && time <= references[i].end)) continue;
        switch(references[i].paramTarget) {
            case TX: if (isCamera(tOBJ)) C(tOBJ)->eye.x = ref(i); else if (isObject(tOBJ)) O(tOBJ)->M.a03 = ref(i); break;
            case TY: if (isCamera(tOBJ)) C(tOBJ)->eye.y = ref(i); else if (isObject(tOBJ)) O(tOBJ)->M.a13 = ref(i); break;
            case TZ: if (isCamera(tOBJ)) C(tOBJ)->eye.z = ref(i); else if (isObject(tOBJ)) O(tOBJ)->M.a23 = ref(i); break;

            case KD_R: if (isGeo(tOBJ)) G(tOBJ)->diffuse[0] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->diffuse[0] = ref(i); break;
            case KD_G: if (isGeo(tOBJ)) G(tOBJ)->diffuse[1] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->diffuse[1] = ref(i); break;
            case KD_B: if (isGeo(tOBJ)) G(tOBJ)->diffuse[2] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->diffuse[2] = ref(i); break;
            case KD_A: if (isGeo(tOBJ)) G(tOBJ)->diffuse[3] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->diffuse[3] = ref(i); break;

            case KS_R: if (isGeo(tOBJ)) G(tOBJ)->specular[0] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->specular[0] = ref(i); break;
            case KS_G: if (isGeo(tOBJ)) G(tOBJ)->specular[1] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->specular[1] = ref(i); break;
            case KS_B: if (isGeo(tOBJ)) G(tOBJ)->specular[2] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->specular[2] = ref(i); break;
            case KS_A: if (isGeo(tOBJ)) G(tOBJ)->specular[3] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->specular[3] = ref(i); break;

            case KA_R: if (isGeo(tOBJ)) G(tOBJ)->ambient[0] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->ambient[0] = ref(i); break;
            case KA_G: if (isGeo(tOBJ)) G(tOBJ)->ambient[1] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->ambient[1] = ref(i); break;
            case KA_B: if (isGeo(tOBJ)) G(tOBJ)->ambient[2] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->ambient[2] = ref(i); break;
            case KA_A: if (isGeo(tOBJ)) G(tOBJ)->ambient[3] = ref(i); else if (isLight(tOBJ)) L(tOBJ)->ambient[3] = ref(i); break;

            case POS: if (isCamera(tOBJ) && isCamera(refOBJ)) {
                C(tOBJ)->eye = C(refOBJ)->eye;
                C(tOBJ)->up = C(refOBJ)->up;
                C(tOBJ)->focus = C(refOBJ)->focus;
            }

            case LOOKAT: if (isLight(tOBJ) && isObject(refOBJ)) {
                             Point a(O(refOBJ)->M.a03, O(refOBJ)->M.a13, O(refOBJ)->M.a23); 
                             Point b(L(tOBJ)->M.a03, L(tOBJ)->M.a13, L(tOBJ)->M.a23); 
                             Point c  = a - b; c.normalize();
                             L(tOBJ)->spotDir[0] = c.x;
                             L(tOBJ)->spotDir[1] = c.y;
                             L(tOBJ)->spotDir[2] = c.z;
                         } else if (isCamera(tOBJ) & isObject(refOBJ)) {
                             Point a(O(refOBJ)->M.a03, O(refOBJ)->M.a13, O(refOBJ)->M.a23); 
                             C(tOBJ)->focus = a;
                         } else if (isObject(tOBJ) & isObject(refOBJ)) {
                             Point a(O(refOBJ)->M.a03, O(refOBJ)->M.a13, O(refOBJ)->M.a23); 
                             Point b(O(tOBJ)->M.a03, O(tOBJ)->M.a13, O(tOBJ)->M.a23); 
                             Point c  = a - b; c.normalize();
                             O(tOBJ)->M = Transform::coordinateFrame(Point(0,1,0), c);
                             O(tOBJ)->M.a03 = b.x;
                             O(tOBJ)->M.a13 = b.y;
                             O(tOBJ)->M.a23 = b.z;
                         }
        }
    }
}


/* maybe use variable args (vargs) and also Generic so that you can do crazy refs between different types of objects */
void Scene::register_reference(Generic * ref, int refParam, Generic * targ, int targParam, float freqscalar, float scalar, float offset, int timeShift, unsigned long int start, unsigned long int end) {
    reference_t r = reference_t();
    r.refObject = ref;
    r.paramRef = refParam;
    r.targObject = targ;
    r.paramTarget=targParam;
    r.freqscalar = freqscalar;
    r.scalar = scalar;
    r.offset = offset;
    r.timeShift = timeShift;
    r.start = start;
    r.end = end;
    references.push_back(r);
}

void Scene::register_animation(Object * obj, int param, Channel * chan, float freqscalar, float scalar, float offset, int timeShift, unsigned long int start, unsigned long int end) {
    animation_t a = animation_t();
    a.object = obj;
    a.param = param;
    a.chan = chan;
    a.freqscalar = freqscalar;
    a.scalar = scalar;
    a.offset = offset;
    a.timeShift = timeShift;
    a.start = start;
    a.end = end;
    animations.push_back(a);
}

/* display functions */
void showLight(Generic * o) {if (isObject(o) && !O(o)->parent)O(o)->display(LIGHT_TYPE);}
void showGeometry(Generic * o) {if(isObject(o) && !O(o)->parent)O(o)->display(GEOMETRY_TYPE);}
void Scene::displayObjects() {for_each (allOBJS.begin(), allOBJS.end(), showGeometry);}
void Scene::displayLights() {for_each (allOBJS.begin(), allOBJS.end(), showLight);}


Generic * Scene::getObjectByName(gen_t &list, String sname) {
    gen_t::iterator cursor; 
    for(cursor = list.begin(); cursor != list.end(); cursor++) {
        Generic * o = *cursor;
        String cmp = o->name;
        if (!cmp.compare(sname)) return o;
        //if (o->objs.size() > 0) {
        //    Object * ob = getObjectByName(o->objs, sname);
        //    if (ob) return ob;
        // }
    }
    return NULL;
}

/* if you keep the list normalized, this need not be recursive, nor the above function */
bool Scene::nameExists(gen_t &list, String sname) {
    gen_t::iterator cursor; 
    for(cursor = list.begin(); cursor != list.end(); cursor++) {
        Generic * o = *cursor;
        String cmp = o->name;
        if (!cmp.compare(sname)) return true;
        //if (o->objs.size() > 0) {
        //   bool test = nameExists(o->objs, sname);
        //    if (test) return true;
        // }
    }
    return false;
}

Generic * Scene::getObjectByFile(String filepath) {
    gen_t::iterator csr=allOBJS.begin();
    for(;csr!=allOBJS.end();csr++) {
        if (!(*csr)->filepath.compare(filepath)) {
            return *csr;
        }
    }
    return NULL;
}

inline int screenWidth() {return .3*glutGet(GLUT_SCREEN_WIDTH);}
inline int screenHeight() {return .5*glutGet(GLUT_SCREEN_HEIGHT);}
inline int windowWidth() {return glutGet(GLUT_WINDOW_WIDTH);}
inline int windowHeight() {return glutGet(GLUT_WINDOW_HEIGHT);}

Point Scene::window2scene(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    //Camera * cam = WindowManager::getCamera(x, y);
    //WindowManager::getViewport(viewport, x, y);

    winX = float(x);
    winY = float(viewport[3]) - float(y);
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    Point planeNormal = Point(0,1,0);
    Point planePos = Point(0,0,0);

    if (ivMODE&EDIT_MODE) {
        if (ivMODE & YAXIS_MODE) planeNormal = Point(1,0,0);
        if(curve) planePos = curve->lastActive;
    }

    Point hit = Point();
    Ray::intersectPlaneNoBounds( Point(posX, posY, posZ), camera->eye, planeNormal, planePos, &hit);
    return hit;
}

void Scene::motion(int x, int y) {

    if (window->current->view == CHANNEL_VIEW) {
        window->current->scopeAmountx -= 10*(x-x_)/float(screenWidth());
        window->current->scopeAmounty -= 10*(y-y_)/float(screenHeight());
        
        //printf("scope amountx %f\n", window->current->scopeAmountx);
        //printf("scope amounty %f\n", window->current->scopeAmountx);
        x_ = x;
        y_ = y;
        glutPostRedisplay();
        return;
    }

    if(btn_==GLUT_LEFT_BUTTON && !toggleRMouse && !toggleMMouse) {
        glutSetCursor(GLUT_CURSOR_CYCLE);

        // DRAGGING POINTS IN EDIT MODE
        if (ivMODE & EDIT_MODE) {
            if (curve) {
                Point a = window2scene(x,y);
                if (ivMODE & ORTHO_MODE) Func::orthoPoint(a);
                curve->moveActivePointTo(a);}
                //if (curve) curve->moveActivePoint(x_-x, 0.0, y_-y);
        }

        if((ivMODE & TRANSLATE_MODE) && camera){
            if (isGeo(selected)) {
                Point camDir = Point(camera->eye.x - camera->focus.x, 0 , camera->eye.z - camera->focus.z);
                G(selected)->move(y-y_, x-x_, camDir);
            } else if (isLight(selected)) {
                Point camDir = Point(camera->eye.x - camera->focus.x, 0 , camera->eye.z - camera->focus.z);
                L(selected)->move(y-y_, x-x_, camDir);
            }
        } else if (ivMODE & ROTATE_MODE) {
            if (isGeo(selected)) {
                G(selected)->rotateObj(y-y_+x-x_, Point(1, 0, 0));
            } else if (isLight(selected)) {
                L(selected)->rotateLight(y-y_+x-x_, Point(1, 0, 0));
            }
        } else if (camera && !camera->locked) {
            camera->tilt( (y_-y)/2.0f );
            camera->pan(x_-x);
        }
    }
    if(btn_==GLUT_MIDDLE_BUTTON || toggleMMouse) {
        glutSetCursor(GLUT_CURSOR_UP_DOWN);
        if (ivMODE & ROTATE_MODE) {
            if (isGeo(selected)) {
                G(selected)->rotateObj(y-y_+x-x_, Point(0, 1, 0));
            } else if (isLight(selected)) {
                L(selected)->rotateLight(y-y_+x-x_, Point(0, 1, 0));
            }
        } else if (camera && !camera->locked) {
            // zoom
            camera->dolly( y_-y );
            camera->dolly(x-x_);

        }
    }
    if(btn_==GLUT_RIGHT_BUTTON || toggleRMouse) {
        glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
        if(ivMODE & TRANSLATE_MODE){
            if (isGeo(selected)) {
                G(selected)->moveUp(y_-y);
            } else if (isLight(selected)) {
                L(selected)->moveUp(y_-y);
            }
        } else if(ivMODE & ROTATE_MODE) {
            if (isGeo(selected)) {
                G(selected)->rotateObj(y-y_+x-x_, Point(0, 0, 1));
            } else if (isLight(selected)) {
                L(selected)->rotateLight(y-y_+x-x_, Point(0, 0, 1));
            }
        } else if (camera && !camera->locked) {
            camera->pedestal( y_-y );
            camera->truck(x_-x);
        }
    }
    x_ = x;
    y_ = y;
    glutPostRedisplay();
}

void Scene::mouse(int btn, int state, int x, int y) {

    if (state == GLUT_DOWN) {
        camera = WindowManager::getCamera(x,y);

        // CLICKING -- INSERT MODE
        if (ivMODE&INSERT_MODE) {
            if (btn == GLUT_RIGHT_BUTTON) {
                Func::End_INSERT_MODE();
            } else if (btn == GLUT_LEFT_BUTTON) {
                Point a = window2scene(x,y);
                if (ivMODE & ORTHO_MODE) Func::orthoPoint(a);
                if (!curve) addCurve();
                curve->addPoint(a);
            }
            glutPostRedisplay();

            // CLICKING -- EDIT_MODE
        } else if ((ivMODE & EDIT_MODE) && curve) {
            if (btn == GLUT_RIGHT_BUTTON) { 
                Func::End_EDIT_MODE();
            } else if (btn == GLUT_MIDDLE_BUTTON) {
                curve->deleteActivePoint();
            }
            glutPostRedisplay();
        } else if (ivMODE & SELECT_MODE){

            /* select objects */
            Ray ray(x,y);
            Intersection i = Render::getIntersection(ray);
            if (i.geo) setActive(GEN(i.geo));

        }

        if (state_ == M_NONE) {
            state_ = M_DRAG;
            btn_ = btn;
            x_ = x;
            y_ = y;
        }

    } else if (state == GLUT_UP)  {	
        if(SceneCAM) SceneCAM->setDistance();
        state_ = M_NONE;
        if (ivMODE & INSERT_MODE) glutSetCursor(GLUT_CURSOR_INFO);
        else if (ivMODE & EDIT_MODE) glutSetCursor(GLUT_CURSOR_CROSSHAIR);
        else glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    }
}

void Scene::addCurve() {
    Geo * g = new Geo();
    g->name=name(String("Curve"));
    CV(g) = new Curve();
    Scene::curve = CV(g);
    Scene::add(GEN(g));
}

void Scene::keyboard(unsigned char key, int x, int y) {

    /* command line */
    if (key != 0x1B && key != '\r' && (ivMODE&COMMAND_MODE)) {
        /* delete key or backspace */
        if (key == 0x08 || key == 0x7F) {
            if (Term::buffer.size() > 0) Term::buffer.erase(Term::buffer.end()-1);
            glutPostRedisplay();
            return;
        }
        // append char to the buffer!
        Term::buffer.push_back(key);
        glutPostRedisplay();
        return;
    }

    // INSERT MODE
    if (key == 'i' || key == '1') {
        Func::Begin_INSERT_MODE();
        glutPostRedisplay();
        return;
    } else  if (key == '\r') { /* RETURN_KEY */
        // ENTER RETURN 
        if (ivMODE&COMMAND_MODE) Func::End_COMMAND_MODE();
        else if (ivMODE&EDIT_MODE) Func::End_EDIT_MODE();
        else if (ivMODE&INSERT_MODE) Func::End_INSERT_MODE();
        else Func::Begin_EDIT_MODE();
    }

    String s;
    String text;
    vector<String>emptyArgs=vector<String>();
    switch(key) {
        case 'q': Scene::Exit(); break;
        case ':': Func::Begin_COMMAND_MODE(); break;
        case 'h':   if (camera) {
                        camera->reset();
                        camera->zoomScale = 0.20f;  
                        camera->distance = 16.0f;
                    }
                    window->current->scopeAmountx=1.0f;
                    window->current->scopeAmounty=1.0f;break;
        case 'j': selected = PrevElement( allObjects, selected ); Operations::List(emptyArgs); break;
        case 'k': selected = NextElement( allObjects, selected ); Operations::List(emptyArgs); break;
        case 'l':
                  /* cycle through draing modes */
                  if (drawMODE&NO_INTERP){drawMODE&=~NO_INTERP;drawMODE|=BEZIER_INTERP;}
                  else if (drawMODE&BEZIER_INTERP){drawMODE&=~BEZIER_INTERP;drawMODE|=BSPLINE_INTERP;}
                  else if (drawMODE&BSPLINE_INTERP){drawMODE&=~BSPLINE_INTERP;drawMODE|=NO_INTERP;}
                  break;
        case 0x1B: //escape
                  ivMODE&=~COMMAND_MODE;
                  Term::buffer.clear();
                  break;
        case 'd': if (curve) curve->points.clear(); break;
        case 'o': curSTATE=(curSTATE&ORTHO_GRID)?(curSTATE&~ORTHO_GRID):(curSTATE|ORTHO_GRID); break;
        case 'e': break;        
        case 'c': camera->locked = !camera->locked; break;        
        case '*': Func::Toggle_SELECT_MODE(); break;        
        case 'a': Func::Toggle_LIGHTS_ON(); break;
        case ' ': Func::Toggle_PLAY(); break;
        case 'n': Func::Toggle_LIGHTS_DISP(); break;
        case '+': Func::addPoint();break;
        case '0': curSTATE=DISPLAY_STATE; break;
        case '9': char ss[512]; sprintf(ss, "Capture%05lu.ppm", time); Render::CurrentScreen(String(ss), 0,0,w,h); break;
        case '8': Func::Toggle_ORTHO_MODE(); break;
        case '7': Func::Toggle_SHOW_AXIS(); break;
        case '6': Func::Toggle_SHOW_TERM(); break;
        case '5': Func::Toggle_DISPLAY_LIST(); break;
        case '4': Func::Toggle_SHOW_SELECT(); break;
        case '2': if (SceneCAM) SceneCAM->top();break;
        case 't': Func::Toggle_TEXTURE_ON(); break;
        case 's': Func::Toggle_SMOOTH_SD(); break;
        case 'w': Func::Toggle_WIREFRAME(); break;
        case 'p': Func::Toggle_POINT_DISP(); break;
        case 'm': Func::Toggle_TRANSLATE_MODE(); break;
        case 'r': Func::Toggle_ROTATE_MODE();break;
        case 'y': Func::Toggle_YAXIS_MODE();break;
        case 'u': Func::Undo(); break; 
        case 'v': Func::Redo(); break;
		case '=': Func::Zoomfactor(1); break;
		case '-': Func::Zoomfactor(-1); break;			
        case '`': camera->up = Point(0, 1, 0); break;
        case 'z': toggleRMouse = !toggleRMouse; if(toggleRMouse) toggleMMouse = false; break;
        case 'x':
                  toggleMMouse = !toggleMMouse; if(toggleMMouse) toggleRMouse = false; break; }
                  glutPostRedisplay();
}

void Scene::specialKey(int key, int x, int y) {
    switch(key) {
        case 100: if (isObject(selOBJ)) O(selOBJ)->M.a03 += 1.0; break; // left
        case 101: if (ivMODE&COMMAND_MODE) { Term::goBack(); glutPostRedisplay(); } else { if (isObject(selOBJ)) O(selOBJ)->M.a13 += 1.0; } break; //up
        case 102: if (isObject(selOBJ)) O(selOBJ)->M.a03 -= 1.0; break; //right
        case 103: if (ivMODE&COMMAND_MODE) { Term::goForward(); glutPostRedisplay(); } else { if (isObject(selOBJ)) O(selOBJ)->M.a13 -= 1.0; } break; //down
    }
    glutPostRedisplay();
}

void Scene::reshape(int width, int height){
    w = width;
    h = height;
    Scene::window->reshape(w, h); 
}

void Scene::init() {

    /* initialize cameras and window */
    Scene::camera = new Camera();
    Scene::window = new WindowManager();
    Scene::addCamera(camera);
    Scene::window->setSize(0,0, w, h);
    Scene::window->setCamera(Scene::camera);

    amount = 5;

    //set up a point light
    glEnable(GL_LIGHTING);

    /* enable hidden-surface removal */
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
}

void Scene::toggleLights(bool onOff) {
    if (!onOff) {
        glDisable(GL_LIGHTING);
        return;
    }
    if ( !(curSTATE & POINT_DISP) && !(curSTATE & WIREFRAME) ) {
        glEnable(GL_LIGHTING);
    }
}

void Scene::set2DMode(float x, float y, float w, float h) {
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
    glPushMatrix();                  // Store The Projection Matrix
    glLoadIdentity();               // Reset The Projection Matrix
    gluOrtho2D(x,w,y,h);
    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glPushMatrix();     // Store The Modelview Matrix
    glLoadIdentity();      // Reset The Modelview Matrix
    Scene::toggleLights(false);
}

void Scene::set3DMode() {
    glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix
    glPopMatrix();                 // Restore The Old Projection Matrix
    glMatrixMode(GL_MODELVIEW);    // Select The Modelview Matrix
    glPopMatrix();               // Restore The Old Projection Matrix
    glEnable(GL_DEPTH_TEST);
    Scene::toggleLights(true);
}


void Scene::showScene() {
    if (curSTATE & ORTHO_GRID) Scene::constructionPlane();
    if (curSTATE & SHOW_AXIS) Scene::axis();
    Scene::printCurves();
    Scene::displayObjects();
    glColor3f(1.0,1.0,1.0);
}

//Display
void Scene::display() {
    // set the background color
    glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);
    /* we must clear the buffer*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Scene::window->display();


    /* display text and terminal */
    if (curSTATE & SHOW_TERM) {
        glViewport(0, 0, w, h);
        set2DMode(0, 0, w, h);
        {
            Scene::printTerminal();
            Scene::printOutput(outbuf_, 0, h-15);
            if (Scene::camera) {
                String c = String("Cam: " + Scene::camera->name);
                Scene::printOutput(c, w-c.length()-100, h-15);
            }
            if (selOBJ) {
                String c = String("select: " + selOBJ->name);
                Scene::printOutput(c, w-c.length()-100, h-45);
            }
            Scene::printChannel();
        }
        set3DMode();
    }
    glutSwapBuffers();
    glFlush();
}

void Scene::printChannel() {}

void Scene::printOutput(String &buffer, float w, float h) {

    int newLine=0;
    glRasterPos2f(w,h);
    const char *s = buffer.c_str();
    for(unsigned int j=0;j<buffer.size(); j++,s++) {
        if (*s == '\n') {
            newLine+=15;
            glRasterPos2f(w,h-newLine);                 
        } else {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*s);
        }
    }


}

void Scene::printTerminal() {
    glBegin(GL_LINES);
    glVertex2f(0,17);
    glVertex2f(w,17);
    glEnd();

    glColor3f(bgcolor[0], bgcolor[1], bgcolor[2]);

    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f); glVertex2f(w, 17.0f); glVertex2f(0.0f, 17.0f);
    glEnd();
    glColor3f(textcolor[0], textcolor[1], textcolor[2]);
    if (Term::buffer.size() > 0) {
        glRasterPos3f(0,5,0);
        const char *s = Term::buffer.c_str();
        for(unsigned int j=0;j<Term::buffer.size(); j++,s++)
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*s);

    }   
}

void Scene::printCurves() {
    Scene::toggleLights(false);
    glColor3f(1.0,1.0,1.0);

    /* emulate this in animation too */
    if (((ivMODE&INSERT_MODE) || (ivMODE&EDIT_MODE)) && curve)  {
        if (drawMODE&NO_INTERP) curve->noInterpolation();
        if (drawMODE&BEZIER_INTERP) {
            curve->Bezier(curve->points.size()+5);
            glColor3f(0.2,0.2,0.2);
            curve->noInterpolation();
            glColor3d(1,1,1);
        }
        if (drawMODE&BSPLINE_INTERP) {
            curve->Bspline(5);
            glColor3f(0.2,0.2,0.2);
            curve->noInterpolation();
            glColor3d(1,1,1);
        }
        curve->printPoints();
        if (ivMODE&EDIT_MODE) curve->EditHandle();
    }

    Scene::toggleLights(true);

}


void Scene::constructionPlane() {

    toggleLights(false);
    glLineWidth(1.0) ;
    glColor3f(gridcolor[0], gridcolor[1], gridcolor[2]);
    glBegin(GL_LINES);
    for (int i=-gridSize;i<=gridSize;i+=gridStep) {
        glVertex3f(-gridSize,0,i);
        glVertex3f(gridSize,0,i);
        glVertex3f(i,0,-gridSize);
        glVertex3f(i,0,gridSize);
    }
    glEnd();
    toggleLights(true);
}

/* world axis */
void Scene::axis() {

    Scene::toggleLights(false);
    glLineWidth(3.0);

    glBegin(GL_LINES);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(0.0f,0.0f,gridStep*1.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(0.0f,gridStep*1.0f,0.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(gridStep*1.0f,0.0f,0.0f);
    glEnd();   
    /* label */
    glLineWidth(1.0);
    glColor3f(1.0,1.0,1.0);
    glRasterPos3f(gridStep*1.2f,0.0f,0.0f);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'x');
    glRasterPos3f(0.0f,gridStep*1.2f,0.0f);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'y');
    glRasterPos3f(0.0f,0.0f,gridStep*1.2f);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,'z');
    Scene::toggleLights(true);
}

int Scene::go(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20,20);
    glutCreateWindow("vim3D");
    init();

    if (argc == 2) {
        printf("loading %s...\n",*(argv+1));
        loadScene(argv[1]);
    } else
        loadScene((char *) "scene1");

    glutDisplayFunc(display);
    glutSpecialFunc(specialKey);
    timerFunc(0);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    glutReshapeFunc(reshape);
    glutReshapeWindow(640, 480);
    //glutReshapeWindow(screenWidth(), screenHeight());
    glutMainLoop();
    return 0;
}

void Scene::add(Generic * o) {
    if (isLight(o)) addLight(L(o));
    else if (isCamera(o)) addCamera(C(o));
    else if (isGeo(o)) addGeo(G(o));
    else if (isPoly(o)) addPoly(PL(o));
    else if (isTexture(o)) addTexture(T(o));
    else if (isChannel(o)) addChannel(CH(o));
}

void Scene::addGeo(Geo * g) { allObjects.push_back(GEN(g));setActive(GEN(g));}
void Scene::addPoly(Poly * p) { allObjects.push_back(GEN(p));setActive(GEN(p));}
void Scene::addTexture(Texture * t) { allObjects.push_back(GEN(t));}
void Scene::addLight(Light * l) {  allObjects.push_back(GEN(l));setActive(GEN(l));}
void Scene::addCamera(Camera * c) { allObjects.push_back(GEN(c));setActive(GEN(c));}
void Scene::addChannel(Channel * c) { allObjects.push_back(GEN(c));setActive(GEN(c));}

void Scene::loadScene(String filename) {
    FILE * fd = fopen(filename.c_str(), "r");
    if (!fd) return;
    char s[512];
    while((fgets(s, 512, fd)) != NULL) {
        String str = String(s);
        Term::parse(str, false);
    }
    fclose(fd);

    FILE * history = fopen(String(".history").c_str(), "r");
    if (history) {
        while((fgets(s, 512, fd)) != NULL) {
            const char * t = s;
            while(*t=='\r'||*t=='\t'||*t=='\n'||*t=='\v'||*t==':') {t++;if(*t=='\0')break;}
            String str = String(t);
            Term::lines.push_back(str); 
        }
        fclose(history);   
    }


}

void Scene::dumpScene(String filename) {
    if (allObjects.size() <= 0) {
        printf("no objects\n");
        return;
    }
    FILE * fd = fopen(filename.c_str(), "w"); if (!fd) return;
    //String dirname = String(filename+"-files");
    // Directory::makeDir(dirname);

    fprintf(fd,"#created by Dan Lynch\n");
    gen_t::iterator cursor; 
    for(cursor = allObjects.begin(); cursor < allObjects.end(); cursor++) {
        Generic * o = *cursor;
        if (isLight(o)) {
            String lightInfo = Utils::LightProperties(L(o));
            fprintf(fd, "%s\n", lightInfo.c_str());
        } else if (isGeo(o)) {
            String geoInfo = Utils::GeoProperties(G(o));
            fprintf(fd, "%s\n", geoInfo.c_str());
        } else if (isCamera(o)) {
            String camInfo = Utils::CamProperties(C(o));
            fprintf(fd, "%s\n", camInfo.c_str());
        } else if (isChannel(o)) {

            // TO DO, if procedurally generated, what expression???

            String chanInfo = String("load channel " + o->filepath);
            fprintf(fd, "%s\n", chanInfo.c_str());
            fprintf(fd, "name %s\n", o->name.c_str());
        } else if (isPoly(o)) {
            // write out OBJs!
        }
    }

    if (animations.size() > 0) {
        fprintf(fd, "\n#animations\n");
        vector<animation_t>::iterator csr = animations.begin();
        for(; csr!= animations.end(); csr++) {
            animation_t a = *csr;
            if (!a.object || !a.chan) continue;
            fprintf(fd, "register %s %s %s %f %f %f %d %lu %lu\n",
                    a.object->name.c_str(),
                    (Utils::Param(a.param)).c_str(),
                    a.chan->name.c_str(),
                    a.freqscalar, a.scalar, a.offset, a.timeShift, a.start, a.end); 
        }
    }

    if (references.size() > 0) {
        fprintf(fd, "\n#references\n");
        vector<reference_t>::iterator csr = references.begin();
        for(; csr!= references.end(); csr++) {
            reference_t a = *csr;
            if (!a.refObject || !a.targObject) continue;
            if (isGeo(a.targObject) && G(a.targObject)->inst) continue;
            fprintf(fd, "reference %s %s %s %s %d %f %f %d %lu %lu\n",
                    a.refObject->name.c_str(),
                    (Utils::Param(a.paramRef)).c_str(),
                    a.targObject->name.c_str(),
                    (Utils::Param(a.paramTarget)).c_str(),
                    a.freqscalar,a.scalar, a.offset, a.timeShift, a.start, a.end); 
        }
    }
}

void deleteObj (Generic * o) { delete o; }
void Scene::Exit() {
    printf("exiting...\n");
    dumpScene((char *) "dump");

    for_each (allObjects.begin(), allObjects.end(), deleteObj);
    delete window;
    exit(0);
}

void Scene::setActive(Generic * obj) {
    if (selOBJ != NULL) selOBJ->active=false;
    obj->active = true;
    selOBJ = obj;
}
