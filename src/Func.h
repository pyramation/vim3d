#ifndef FUNC_H
#define FUNC_H

#include "Packages.h"

#include <stdio.h>
#include <math.h>

#include "Geometry.h"

class Func {
    public:

        static void Undo();
        static void Redo();

        static void addPoint();

        static void orthoPoint(Point &a);

        static void Begin_COMMAND_MODE();
        static void End_COMMAND_MODE();

        static void Begin_EDIT_MODE();
        static void End_EDIT_MODE();

        static void Begin_INSERT_MODE();
        static void End_INSERT_MODE();

        static void Toggle_TRANSLATE_MODE();
        static void Toggle_ROTATE_MODE();
        static void Toggle_YAXIS_MODE();
        static void Toggle_ORTHO_MODE();
        static void Toggle_SELECT_MODE();


        static void Toggle_LIGHTS_ON();
        static void Toggle_LIGHTS_DISP();
        static void Toggle_TEXTURE_ON();
        static void Toggle_SHOW_TERM();
        static void Toggle_WIREFRAME();
        static void Toggle_POINT_DISP();
        static void Toggle_POINT_NORMALS();
        static void Toggle_POINT_NUMS();
        static void Toggle_FACE_NORMALS();
        static void Toggle_FACE_NUMS();
        static void Toggle_SMOOTH_SD();
        static void Toggle_ORTHO_GRID();
        static void Toggle_SHOW_AXIS();
        static void Toggle_DISPLAY_LIST();
        static void Toggle_PLAY();
        static void Toggle_SHOW_SELECT();
        static void Toggle_RENDERING();
        static void Toggle_RAYTRACING();
		static void Zoomfactor(int dir);

        Func();
        virtual ~Func();

};

#endif

