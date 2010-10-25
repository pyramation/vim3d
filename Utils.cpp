#include "Utils.h"

Utils::Utils() {}


String Utils::matrix(Object * o) {
    char fd[2048];
    sprintf(fd, "matrix %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
            o->M.a00,  o->M.a01, o->M.a02, o->M.a03,
            o->M.a10,  o->M.a11, o->M.a12, o->M.a13,
            o->M.a20,  o->M.a21, o->M.a22, o->M.a23,
            o->M.a30,  o->M.a31, o->M.a32, o->M.a33);
    sprintf(fd,"%s\n", fd);
    String f(fd);
    return f;
}

String Utils::GeoProperties(Geo * g) {
    char fd[65536];
    sprintf(fd,"#geometry\n");
    if (g->group) {
        sprintf(fd, "%sobj group ", fd);
        for(unsigned int j=0;j<g->objs.size();j++) sprintf(fd, "%s%s ", fd, g->objs[j]->name.c_str()); 
        sprintf(fd, "%s\n", fd); 
    } else if (g->copy) sprintf(fd, "%scopy %s %s\n",fd, g->Target->name.c_str(), g->Template->name.c_str()); 
    else if (P(g) && (P(g)->filepath.size() > 0)) sprintf(fd,"%sfile %s\n", fd,P(g)->filepath.c_str());
    else if (isSphere(g)) {
        sprintf(fd, "%sadd sphere\n", fd);
    } else if (isCube(g)) {
        sprintf(fd, "%sadd cube\n", fd);
    } else if (isGrid(g)) {
        Grid * grid = (Grid*)g;
        sprintf(fd, "%sadd grid %d %d %f %f %f\n", fd, grid->dx, grid->dz, grid->segmentSize, grid->tx, grid->tz);
    } else if (P(g) && !CV(g)) {
        P(g)->filepath = String(String("objects/objs/") + g->name + String(".obj"));
        P(g)->writeOBJ(P(g)->filepath);
        sprintf(fd,"%sfile %s\n", fd,P(g)->filepath.c_str());
    } else if (CV(g)) {
        sprintf(fd, "%sbegin draw\n", fd);
        for(unsigned int i=0;i<CV(g)->points.size();i++) {
            sprintf(fd, "%s%f %f %f\n", fd, CV(g)->points[i].x, CV(g)->points[i].y, CV(g)->points[i].z);
        }
        sprintf(fd, "%send\n", fd);
    } else sprintf(fd, "%sadd geo\n", fd);
    sprintf(fd,"%sname %s\n", fd,g->name.c_str());
    if (g->history.size() > 0) {
        for(unsigned int i=0;i<g->history.size();i++) {
            sprintf(fd, "%s%s\n", fd, g->history[i].c_str());
        }
    }
    if (g->child) {
        sprintf(fd, "%sobj child %s %s\n", fd, g->parent->name.c_str(), g->name.c_str());
    }
    sprintf(fd,"%sdiffuse %f %f %f %f\n", fd,g->diffuse[0], g->diffuse[1], g->diffuse[2], g->diffuse[3]);
    sprintf(fd,"%sspecular %f %f %f %f\n", fd,g->specular[0], g->specular[1], g->specular[2], g->specular[3]);
    sprintf(fd,"%sambient %f %f %f %f\n", fd,g->ambient[0], g->ambient[1], g->ambient[2], g->ambient[3]);
    sprintf(fd,"%sreflection %f %f %f\n", fd,g->reflection[0], g->reflection[1], g->reflection[2]);
    sprintf(fd,"%srefraction %f\n", fd,g->refractiveIndex);
    sprintf(fd,"%sshininess %f \n", fd, g->shininess[0]);
    sprintf(fd,"%semission %f %f %f %f\n", fd,g->emission[0], g->emission[1], g->emission[2], g->emission[3]);
    sprintf(fd,"%s%s\n", fd, (matrix(O(g))).c_str());
    if (g->tex) sprintf(fd,"%stexture %s\n", fd,g->tex->filepath.c_str());
    String f(fd);
    return f;
}

String Utils::CamProperties(Camera * c) {
    char fd[2048];
    sprintf(fd,"#camera\n");
    sprintf(fd,"%sadd camera\n",fd);
    sprintf(fd,"%sname %s\n", fd,c->name.c_str());
    sprintf(fd, "%szNear %f\n", fd,c->zNear);
    sprintf(fd, "%szFar %f\n",fd, c->zFar);
    sprintf(fd, "%sfovy %f\n", fd,c->fovy);
    sprintf(fd, "%sposition %f %f %f\n", fd,c->eye.x, c->eye.y, c->eye.z);
    sprintf(fd, "%scamera lookat %f %f %f\n", fd,c->focus.x, c->focus.y, c->focus.z);
    sprintf(fd, "%sup %f %f %f\n", fd,c->up.x, c->up.y, c->up.z);
    String f(fd + matrix(O(c)));
    return f;
}

/* LIGHT TYPE needed */
String Utils::LightProperties(Light * l) {
    char fd[2048];
    sprintf(fd,"#light\n");
    sprintf(fd,"%sadd light\n", fd);
    sprintf(fd,"%sname %s\n", fd,l->name.c_str());
    sprintf(fd,"%sdiffuse %f %f %f %f\n", fd,l->diffuse[0], l->diffuse[1], l->diffuse[2], l->diffuse[3]);
    sprintf(fd,"%sspecular %f %f %f %f\n", fd,l->specular[0], l->specular[1], l->specular[2], l->specular[3]);
    sprintf(fd,"%sambient %f %f %f %f\n", fd,l->ambient[0], l->ambient[1], l->ambient[2], l->ambient[3]);
    sprintf(fd,"%slight constant %f\n", fd, l->constant);
    sprintf(fd,"%slight linear %f\n", fd, l->linear);
    sprintf(fd,"%slight quadratic %f\n", fd, l->quadratic);
    sprintf(fd,"%slight ltype %d\n", fd, l->ltype);
    sprintf(fd,"%slight cutoff %f\n", fd, l->cutoff);
    sprintf(fd,"%slight spot %f %f %f\n", fd, l->spotDir[0], l->spotDir[1],l->spotDir[2]);
    String f(fd + matrix(O(l)));
    return f;
}

#define is(x) (!param.compare(String(x)))
int Utils::Param(String param) {

    if      (is("tx")) return TX; 
    else if (is("ty")) return TY; 
    else if (is("tz")) return TZ;

    else if (is("rx")) return RX; 
    else if (is("ry")) return RY; 
    else if (is("rz")) return RZ;

    else if (is("diffr")) return KD_R;
    else if (is("diffg")) return KD_G;
    else if (is("diffb")) return KD_B;
    else if (is("diffa")) return KD_A;

    else if (is("specr")) return KS_R;
    else if (is("specg")) return KS_G;
    else if (is("specb")) return KS_B;
    else if (is("speca")) return KS_A;

    else if (is("ambr")) return KA_R;
    else if (is("ambg")) return KA_G;
    else if (is("ambb")) return KA_B;
    else if (is("amba")) return KA_A;

    else if (is("shine")) return KSHINE;

    else if (is("lookat")) return LOOKAT;
    else if (is("pos")) return POS;
    else if (is("update")) return UPDATE;

    return 0;
}

String Utils::Param(int param) {
    String a = String();
    switch(param) {
        case TX: a = String("tx");break;
        case TY: a = String("ty");break;
        case TZ: a = String("tz");break;

        case RX: a = String("rx");break;
        case RY: a = String("ry");break;
        case RZ: a = String("rz");break;

        case SX: a = String("sx");break;
        case SY: a = String("sy");break;
        case SZ: a = String("sz");break;

        case KD_R: a = String("diffr");break;
        case KD_G: a = String("diffg");break;
        case KD_B: a = String("diffb");break;
        case KD_A: a = String("diffa");break;

        case KS_R: a = String("specr");break;
        case KS_G: a = String("specg");break;
        case KS_B: a = String("specb");break;
        case KS_A: a = String("speca");break;

        case KA_R: a = String("ambr");break;
        case KA_G: a = String("ambg");break;
        case KA_B: a = String("ambb");break;
        case KA_A: a = String("amba");break;

        case LOOKAT: a = String("lookat");break;
        case POS: a = String("pos");break;
        case UPDATE: a = String("update");break;

    }
    return a;
}

void Utils::writeMTL(FILE * fd, Material &mat) {
    fprintf(fd, "newmtl %s\n", mat.name.c_str());
    fprintf(fd, "Ka %f %f %f\n", mat.ambient[0], mat.ambient[1], mat.ambient[2]);
    fprintf(fd, "Kd %f %f %f\n", mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
    fprintf(fd, "Ks %f %f %f\n", mat.specular[0], mat.specular[1], mat.specular[2]);
    fprintf(fd, "Tr %f\n", mat.ambient[3]);
    fprintf(fd, "Ns %f\n", mat.shininess);
    if (mat.texture) fprintf(fd, "map_Ka %s\n", mat.texture->filepath.c_str());
    fprintf(fd, "refr %f\n", mat.refractiveIndex);
    fprintf(fd, "refl %f %f %f\n", mat.reflection[0], mat.reflection[1], mat.reflection[2]);
}

Utils::~Utils() {}

