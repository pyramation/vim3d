#ifndef WINDOW_H
#define WINDOW_H

#include "OpenGLinc.h"
#include "Packages.h"
#include "Geometry.h"
#include "vectorUtils.h"
#include "Scene.h"
#include "Channel.h"

enum View {MODEL_VIEW, CHANNEL_VIEW, IMAGE_VIEW};
enum Split {NO_SPLIT, HORIZ_SPLIT, VERT_SPLIT};

class Window {
    public:
    int x,y, w, h; 
    Camera * cam;
    View view; 
    Window * child;
    Split split;

    /* channels */
    int scopeB;
    float scopeAmountx;
    float scopeAmounty;

    void display();
    void setSize(int x_, int y_, int w_, int h_);
    Window(Camera * cam, int x_, int y_, int w_, int h_); 
    Window();
    ~Window();
};

class WindowManager {
    private:
    static Window * root;
    static vector <Window *> windows;
    public:
    static Camera * twoD;
    static Window * current;
    static void close();
    static void display();
    static void reshape(int w_, int h_);
    static void splitVertical();
    static void splitHorizontal();
    static void setCamera(Camera * camera);
    static void setSize(int x_, int y_, int w_, int h_);

    static Point unproject(int x_, int y_);
    static void getViewport(GLint * viewp, int x_, int y_);
    static Camera * getCamera(int x_, int y_);
    static void setWindow(int x_, int y_);

    static void changeView(View view);

    WindowManager();
    ~WindowManager();
};

#endif
