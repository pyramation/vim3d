#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>

#include "Window.h"
#include "Packages.h"
#include "Elements.h"
#include "Primitives.h"
#include "Geometry.h"
#include "Server.h"
#include "Client.h"

#define PI_ 3.14159265358979

#define SCENE_FPS 30

#define SceneCAM Scene::camera
#define outbuf_ Scene::output_buffer
#define allOBJS  Scene::allObjects
#define selOBJ  Scene::selected
#define curSTATE Scene::displayState
#define drawMODE Scene::drawingState
#define ivMODE Scene::interactiveState

/* modes */
#define NO_CURR_STATE 0
#define BEGIN_MODE 1
#define COMMAND_MODE 2
#define INSERT_MODE 4
#define EDIT_MODE 8
#define ROTATE_MODE 16
#define TRANSLATE_MODE 32
#define YAXIS_MODE 64
#define ORTHO_MODE 128
#define SELECT_MODE 256

/* some drawing modes */
#define NO_INTERP 1
#define BEZIER_INTERP 2
#define BSPLINE_INTERP 4
#define CONTROL_PTS 8

/*selectors*/
#define GEN(x) ((Generic*)x)
#define O(x) ((Object*)x)
#define G(x) ((Geo*)x)
#define PL(x) ((Poly*)x)
#define L(x) ((Light*)x)
#define C(x) ((Camera*)x)
#define T(x) ((Texture*)x)
#define CH(x) ((Channel*)x)
#define CP(x) (CH(x)->points)
#define I(x) ((Image*)x)
#define P(x) (G(x)->p)
#define CV(x) (G(x)->curve)
#define V(x) (P(x)->vertices)
#define OBJS(x) (O(x)->objs)

#define isLight Generic::isLight
#define isCamera Generic::isCamera
#define isGeo Generic::isGeo
#define isObject Generic::isObject
#define isChannel Generic::isChannel
#define isTexture Generic::isTexture
#define isPoly Generic::isPoly
#define isImage Generic::isImage

#define isSphere Geo::isSphere
#define isGrid Geo::isGrid
#define isCube Geo::isCube

#define LIGHTS_ON     1 
#define LIGHTS_DISP   2
#define TEXTURE_ON    4
#define SHOW_TERM     8
#define WIREFRAME     16
#define POINT_DISP    32
#define POINT_NORMALS 64
#define POINT_NUMS    128
#define FACE_NORMALS  256
#define FACE_NUMS     512
#define SMOOTH_SD     1024
#define ORTHO_GRID    2048
#define SHOW_AXIS     4096
#define DISPLAY_LIST  8192
#define PLAY          16384
#define SHOW_SELECT   32768
#define RENDERING     65536
#define RAYTRACING    131072
#define DEFAULT_STATE (SHOW_SELECT | DISPLAY_LIST | LIGHTS_ON | TEXTURE_ON | SHOW_TERM | SMOOTH_SD | ORTHO_GRID | SHOW_AXIS | PLAY)
#define DISPLAY_STATE (DISPLAY_LIST | LIGHTS_ON | TEXTURE_ON | SMOOTH_SD | PLAY)

class WindowManager;

/* animation */
typedef struct {
    Object * object;
    int param;
    Channel * chan;
    float freqscalar;
    float scalar;
    float offset;
    int timeShift;
    unsigned long int start;
    unsigned long int end;
} animation_t;

typedef struct {
    Generic * refObject;
    int paramRef;
    Generic * targObject;
    int paramTarget;
    int freqscalar;
    float scalar;
    float offset;
    int timeShift;
    unsigned long int start;
    unsigned long int end;
} reference_t;

class Scene {
    public:

        static Server * server;
        static Client * client;

        /* timeline */
        static unsigned long int start;
        static unsigned long int end;

        static float bgcolor[4];
        static float textcolor[3];
        static float gridcolor[3];

        static WindowManager * window;

        static void setActive(Generic * obj);
        static Generic * selected;
        static Camera * camera;

        static int gridSize;
        static int gridStep;
        static int amount;

        static int displayState;
        static int interactiveState;
        static int drawingState;

        static void timerFunc(int value);
        static long unsigned int time;
        static int w, h;

        static bool toggleRMouse;
        static bool toggleMMouse;

        static void add(Generic * o);
        static void addGeo(Geo * g);
        static void addPoly(Poly * p);
        static void addTexture(Texture * t);
        static void addLight(Light * l);
        static void addCamera(Camera * c);
        static void addChannel(Channel * c);
        static void addCurve();

        static gen_t allObjects; // pointer to all objects

        /* ANIMATION */
        static vector<animation_t>animations;
        static void register_animation(Object * obj, int param, Channel * chan, float freqscalar, float scalar, float offset, int timeShift, unsigned long int start, unsigned long int end);
        static void animCallbackFunc();

        /* REFERENCES */
        static vector<reference_t>references;
        static void register_reference(Generic * ref, int refParam, Generic * targ, int targParam, float freqscalar, float scalar, float offset, int timeShift, unsigned long int start, unsigned long int end); 
        static void refCallbackFunc();

        static unsigned int id;
        static unsigned int newID();
        static bool nameExists(gen_t &list,String name);
        static String name(String name);

        static Generic * getObjectByFile(String filepath);
        static Generic * getObjectByName(gen_t &list,String name);

        static GLint light_lookup[8];

        static class Curve * curve;



        /* display stuff */
        static void display();
        static void printCurves();
        static void constructionPlane();
        static void axis();
        static void displayGeometry(obj_t * obj); 
        static void displayObjects();
        static void displayLights();
        static void showScene();
        static void printTerminal();
        static void printChannel();
        static void printOutput(String &buffer, float w, float h);
        static void set3DMode();
        static void set2DMode(float x, float y,float w, float h);
        static void toggleLights(bool onOff);

        static void loadScene(String filename);
        static void dumpScene(String filename);


        /* buffer and commands */
        static String output_buffer;

        static GLint state_, x_, y_, dx_, dy_, btn_;

        enum {M_NONE, M_DRAG};
        enum {S_NONE, S_COMMAND, S_MENU};


        static void passiveMotion(int x, int y);
        static void motion(int x, int y);
        static void mouse(int btn, int state, int x, int y);
        static void keyboard(unsigned char key, int x, int y);
        static void specialKey(int key, int x, int y);
        static void reshape(int width, int height);
        static void init();


        static void Exit();

        static Point window2scene(int x, int y);

        int go(int argc, char* argv[]);

        Scene();
        virtual ~Scene();

};




#endif
