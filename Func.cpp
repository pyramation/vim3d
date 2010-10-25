#include "Func.h"
#include "Elements.h"
#include "Scene.h"
#include "Term.h"
#include "Operations.h"

Func::Func() {}

Func::~Func() {}

/* to use this function interactively:
  one good way, select the camera, move around (or select an object and move it)
  then simply hit "+" button at each position that you like
  then find the curve, select it (you won't be able to see it), so look at the list for
    Curve0 or Curve1, etc and then hit enter twice */

void Func::addPoint() {
    if (selOBJ) {
        Generic * obj = selOBJ;
        float x, y, z;
        if (isCamera(selOBJ)) {
            x = C(selOBJ)->eye.x;
            y = C(selOBJ)->eye.y;
            z = C(selOBJ)->eye.z;
        } else if (isObject(selOBJ)) {
            x = O(selOBJ)->M.a03;
            y = O(selOBJ)->M.a13;
            z = O(selOBJ)->M.a23;
        } else {x=y=z=0;}
       
        if (!Scene::curve)Scene::addCurve();
        Scene::curve->points.push_back(Point(x,y,z));
        Scene::setActive(obj);

    }
}

void Func::orthoPoint(Point&a) {
    a.x = float(int(a.x/Scene::gridStep)*Scene::gridStep);
    a.y = float(int(a.y/Scene::gridStep)*Scene::gridStep);
    a.z = float(int(a.z/Scene::gridStep)*Scene::gridStep);
}

void Func::Undo() {
    if (ivMODE&EDIT_MODE) {
        //
    } else if (ivMODE&INSERT_MODE) {
        if (Scene::curve && (Scene::curve->points.size() > 0))
            Scene::curve->points.erase(Scene::curve->points.end());
    } else {
        if (isObject(selOBJ)) O(selOBJ)->Undo();
        if (isGeo(selOBJ)) G(selOBJ)->recook();
        else if (isObject(selOBJ)) O(selOBJ)->recook();
        vector<String>args = vector<String>();
        Operations::History(args);
    }
}

void Func::Redo() {
    if (ivMODE&EDIT_MODE) {
        //
    } else if (ivMODE&INSERT_MODE) {
        //
    } else {
        if (isObject(selOBJ)) O(selOBJ)->Redo();
        if (isGeo(selOBJ)) G(selOBJ)->recook();
        else if (isObject(selOBJ)) O(selOBJ)->recook();
        vector<String>args = vector<String>();
        Operations::History(args);
    }
}

void Func::Begin_COMMAND_MODE() {
    Term::commandLine();
}

void Func::End_COMMAND_MODE() {
    ivMODE &=~COMMAND_MODE;
    // parse the buffer and store history
    //    Term::lines.push_back(Term::buffer); // got moved to parse()
    Term::parse(Term::buffer, false);
    Term::buffer.clear();
}

void Func::Begin_INSERT_MODE() {
    ivMODE|=INSERT_MODE;
    Scene::addCurve();
    glutSetCursor(GLUT_CURSOR_INFO);
    Term::buffer = String("-- INSERT --");
}

void Func::End_INSERT_MODE() {
    ivMODE&=~INSERT_MODE;
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    // COOK NODE
    if (selOBJ) G(selOBJ)->recook();
    Term::buffer.clear();
}


void Func::Begin_EDIT_MODE() {
    if (isGeo(selOBJ)) {
        Term::buffer = String("-- EDIT --");
        ivMODE = EDIT_MODE;
        if (CV(selOBJ)) {
            Scene::curve = CV(selOBJ);
        } else if (P(selOBJ)) {
            CV(selOBJ) = Scene::curve = new Curve(P(selOBJ)->vertices);
        }
    }
}

void Func::Zoomfactor(int dir){
	Scene::camera->zoomScale = Scene::camera->zoomScale*(dir=1)?1.4:.7f;
}

void Func::End_EDIT_MODE() {
    Term::buffer.clear();
    ivMODE=NO_CURR_STATE;
    Term::buffer.clear();
    // COOK NODE
    if (isGeo(selOBJ)) G(selOBJ)->recook();
    Scene::curve = NULL;
}
/* interactive modes */
void Func::Toggle_TRANSLATE_MODE() { ivMODE=(ivMODE&TRANSLATE_MODE)?(ivMODE&~TRANSLATE_MODE):(ivMODE|TRANSLATE_MODE);ivMODE&=~ROTATE_MODE;}
void Func::Toggle_ROTATE_MODE() {ivMODE=(ivMODE&ROTATE_MODE)?(ivMODE&~ROTATE_MODE):(ivMODE|ROTATE_MODE); ivMODE&=~TRANSLATE_MODE;}
void Func::Toggle_YAXIS_MODE() {ivMODE=(ivMODE&YAXIS_MODE)?(ivMODE&~YAXIS_MODE):(ivMODE|YAXIS_MODE);}
void Func::Toggle_ORTHO_MODE() {ivMODE=(ivMODE&ORTHO_MODE)?(ivMODE&~ORTHO_MODE):(ivMODE|ORTHO_MODE);}
void Func::Toggle_SELECT_MODE() {ivMODE=(ivMODE&SELECT_MODE)?(ivMODE&~SELECT_MODE):(ivMODE|SELECT_MODE);}


/* states */
void Func::Toggle_LIGHTS_ON() { curSTATE=(curSTATE&LIGHTS_ON)?(curSTATE&~LIGHTS_ON):(curSTATE|LIGHTS_ON); }
void Func::Toggle_LIGHTS_DISP() { curSTATE=(curSTATE&LIGHTS_DISP)?(curSTATE&~LIGHTS_DISP):(curSTATE|LIGHTS_DISP); }
void Func::Toggle_TEXTURE_ON() { curSTATE=(curSTATE&TEXTURE_ON)?(curSTATE&~TEXTURE_ON):(curSTATE|TEXTURE_ON); }
void Func::Toggle_SHOW_TERM() { curSTATE=(curSTATE&SHOW_TERM)?(curSTATE&~SHOW_TERM):(curSTATE|SHOW_TERM); }
void Func::Toggle_WIREFRAME() { curSTATE=(curSTATE&WIREFRAME)?(curSTATE&~WIREFRAME):(curSTATE|WIREFRAME);curSTATE&=~POINT_DISP; }
void Func::Toggle_POINT_DISP() { curSTATE=(curSTATE&POINT_DISP)?(curSTATE&~POINT_DISP):(curSTATE|POINT_DISP);curSTATE&=~WIREFRAME; }
void Func::Toggle_POINT_NORMALS() { curSTATE=(curSTATE&POINT_NORMALS)?(curSTATE&~POINT_NORMALS):(curSTATE|POINT_NORMALS); }
void Func::Toggle_POINT_NUMS() { curSTATE=(curSTATE&POINT_NUMS)?(curSTATE&~POINT_NUMS):(curSTATE|POINT_NUMS); }
void Func::Toggle_FACE_NORMALS() { curSTATE=(curSTATE&FACE_NORMALS)?(curSTATE&~FACE_NORMALS):(curSTATE|FACE_NORMALS); }
void Func::Toggle_FACE_NUMS() { curSTATE=(curSTATE&FACE_NUMS)?(curSTATE&~FACE_NUMS):(curSTATE|FACE_NUMS); }
void Func::Toggle_SMOOTH_SD() { curSTATE=(curSTATE&SMOOTH_SD)?(curSTATE&~SMOOTH_SD):(curSTATE|SMOOTH_SD); }
void Func::Toggle_ORTHO_GRID() { curSTATE=(curSTATE&ORTHO_GRID)?(curSTATE&~ORTHO_GRID):(curSTATE|ORTHO_GRID); }
void Func::Toggle_SHOW_AXIS() { curSTATE=(curSTATE&SHOW_AXIS)?(curSTATE&~SHOW_AXIS):(curSTATE|SHOW_AXIS); }
void Func::Toggle_DISPLAY_LIST() { curSTATE=(curSTATE&DISPLAY_LIST)?(curSTATE&~DISPLAY_LIST):(curSTATE|DISPLAY_LIST); }
void Func::Toggle_PLAY() { curSTATE=(curSTATE&PLAY)?(curSTATE&~PLAY):(curSTATE|PLAY); }
void Func::Toggle_SHOW_SELECT() { curSTATE=(curSTATE&SHOW_SELECT)?(curSTATE&~SHOW_SELECT):(curSTATE|SHOW_SELECT); }
void Func::Toggle_RENDERING() { curSTATE=(curSTATE&RENDERING)?(curSTATE&~RENDERING):(curSTATE|RENDERING); }
void Func::Toggle_RAYTRACING() { curSTATE=(curSTATE&RAYTRACING)?(curSTATE&~RAYTRACING):(curSTATE|RAYTRACING); }
