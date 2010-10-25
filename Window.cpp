#include "Window.h"

vector <Window *> WindowManager::windows;
Window * WindowManager::root;
Window * WindowManager::current;

WindowManager::WindowManager() {
    current = new Window();
    root = current;
    windows.push_back(current);  
}


void WindowManager::changeView(View view) {
    current->view = view;
}

void WindowManager::getViewport(GLint * viewp, int x_, int y_) {
    viewp[0] = current->x;
    viewp[1] = current->y;
    viewp[2] = current->w;
    viewp[3] = current->h;
}

void WindowManager::setWindow(int x_, int y_) {
    vector<Window *>::iterator cursor;
    for(cursor = windows.begin(); cursor != windows.end(); cursor++) {
        Window * w = *cursor;
        int winy = Scene::h - y_;
        if ((w->x <= x_ && x_ <= (w->x + w->w)) &&
                (w->y <= winy && winy <= (w->y + w->h))) {
            current = w;
            return;
        }
    }
}
Camera * WindowManager::getCamera(int x_, int y_) {
    setWindow(x_,y_);
    if (current->view == MODEL_VIEW) return current->cam;
    else return NULL;
}

void showWin(Window *w) {w->display();}
void WindowManager::display(){
    for_each( windows.begin(), windows.end(), showWin);
}

void WindowManager::setCamera(Camera * camera) {
    current->cam = camera;
}

void WindowManager::setSize(int x_, int y_, int w_, int h_) {
    current->x = x_; current->y = y_;
    current->w = w_; current->h = h_;
}

void WindowManager::close() {
    if (root == current) root = root->child;
    FoldChildren( windows, current );
    DeleteElement( windows, current );
    if (!windows.size()) Scene::Exit();
    current = *(windows.begin());
    root->setSize(0,0,Scene::w, Scene::h);
}

void WindowManager::splitVertical(){
    if (current->child != NULL) return;
    Window * win = new Window();
    windows.push_back(win);  
    win->cam = current->cam;
    current->split = VERT_SPLIT;
    current->child = win; 
    current = win;
    root->setSize(0,0,Scene::w, Scene::h);
}
void WindowManager::splitHorizontal(){
    if (current->child != NULL) return;
    Window * win = new Window();
    windows.push_back(win); 
    win->cam = current->cam; 
    current->split = HORIZ_SPLIT;
    current->child = win;
    current = win;
    root->setSize(0,0,Scene::w, Scene::h);
}

void WindowManager::reshape(int w_, int h_) {
    root->setSize(0,0,w_,h_); 
}

WindowManager::~WindowManager() {
    printf("deleting WindowManager()\n");
    FreeClear( WindowManager::windows );
}

Window::Window(){
    cam = NULL;
    split = NO_SPLIT;
    view = MODEL_VIEW;
    child = NULL;

    scopeB = 0;
    scopeAmountx = 1.0f;
    scopeAmounty = 1.0f;

}
Window::Window(Camera * cam_, int x_, int y_, int w_, int h_) {
    cam = cam_;
    x = x_; y = y_;
    w = w_; h = h_;
}  

void Window::setSize(int x_, int y_, int w_, int h_) {
    if (child != NULL) {
        if (split == HORIZ_SPLIT) {
            child->setSize(x_, y_+h_/2, w_, h_/2);
            x = x_; y = y_; w = w_; h = h_/2;
        } else if (split == VERT_SPLIT) {
            child->setSize(x_+w_/2, y_, w_/2, h_);
            x = x_; y = y_; w = w_/2; h = h_;
        }
    } else {
        x = x_; y = y_; w = w_; h = h_;
    }
}

void Window::display() {

    if (view == MODEL_VIEW) {
        if (curSTATE & WIREFRAME) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            Scene::toggleLights(false);
        } else if (curSTATE & POINT_DISP) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            Scene::toggleLights(false);
        } else { 
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            Scene::toggleLights(true);
        }
        if (curSTATE & SMOOTH_SD) {
            glShadeModel(GL_SMOOTH);
        } else {
            glShadeModel(GL_FLAT);
        }

        glViewport(x, y, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (cam != NULL) {
            gluPerspective(cam->fovy, (GLdouble)w/(GLdouble)h, cam->zNear, cam->zFar);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            // USE GLULOOKAT
            mat4 M = cam->lookAt();
            glMultMatrixf(M.mat_array);
        }
        Scene::displayLights();
        Scene::showScene();
    } else if (view == IMAGE_VIEW) {
        // display grid with current selected texture.... 
    } else if (view == CHANNEL_VIEW) {
        glViewport(x, y, w, h);
        Scene::toggleLights(false);
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(x,x+w,y,y+h);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glLineWidth(3.0f);
        for(unsigned int i=0;i<allOBJS.size();i++) {
            if (isChannel(allOBJS[i]) && CH(allOBJS[i])->enabled) {
                //glColor3f(float((i+1) & 4), float((i+1) & 2), float((i+1) & 1));
                float r = float((i+1) & 4); float g = float((i+1) & 2); float b= float((i+1) & 1);
                if (! bool(r+g+b) ) r = g = b = 1;
                glColor3f(r,g,b);
                CH(allOBJS[i])->display(x,y,w,h,scopeB, scopeAmountx, scopeAmounty);
            }
        }
        glLineWidth(1.0f);
        glColor3f(1.0f,1.0f,1.0f);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
        Scene::toggleLights(true);

    }


}
Window::~Window(){
    printf("window closed...\n");
}

