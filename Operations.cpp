#include "Operations.h"
#include "Elements.h"
#include "Primitives.h"
#include "Scene.h"
#include "Window.h"
#include "Func.h"
#include "Utils.h"
#include "Render.h"
#include "Ray.h"
#include "Cloth.h"
#include "Term.h"

#define args(x) (*(args.begin()+x))
#define s2f(x) (atof(args(x).c_str()))
#define s2i(x) (atoi(args(x).c_str()))
#define match(x) (!type.compare(String(x)))
#define objByName(x) (Scene::getObjectByName(allOBJS,args(x)))

/* messages */
static const char * NO_FILE_SPECIFIED= "Please specify a filename.";
static const char * NO_GEO_SELECTED = "You have not selected a geometry object.";
static const char * NO_AMOUNT_SPECIFIED = "Please specify an amount.";
static const char * WRONG_ARGUMENTS = "You have specified the wrong arguments:";
static const char * WRONG_OBJECT = "You have selected the wrong type of object for operation";



/* for help files */ 
enum OP_TYPE {
    OP_QUIT = 0,
    OP_SAVE,
    OP_READOBJ,
    OP_WRITE,
    OP_ZFAR,
    OP_ZNEAR,
    OP_FOVY,
    OP_TEXTURE,
    OP_DRAW,
    OP_BEZIER2,
    OP_BEZIER,
    OP_BSPLINE,
    OP_EXTRUDE,
    OP_REVOLVE,
    OP_COMPUTE,
    OP_BOUNDS,
    OP_DIFFUSE,
    OP_SPECULAR,
    OP_AMBIENT,
    OP_SHININESS,
    OP_ROTATE,
    OP_TRANSLATE,
    OP_SCALE,
    OP_ADD,
    OP_LOAD,
    OP_DELETE,
    OP_VEXPLORE,
    OP_SEXPLORE,
    OP_MATRIX,
    OP_POSITION,
    OP_UP,
    OP_LOOKAT,
    OP_NAME,
    OP_CLEAR,
    OP_PROPERTIES,
    OP_MATRIX_INFO,
    OP_LIST,
    OP_VIEW,
    OP_CHANNEL,
    OP_HISTORY,
    OP_LIGHT,
    OP_GEO,
    OP_COPY,
    OP_HELP,
    OP_SELECT,
    OP_CAMERA,
    OP_OBJECT,
    OP_BEGIN,
    OP_END,
    OP_REGISTER,
    OP_REFERENCE,
    OP_SWEEP,
    OP_PATH,
    OP_POLY,
    OP_RENDER,
    OP_TIMELINE,
    OP_DISPLAY,
    OP_CLEARALL,
    OP_CHILDREN,
    OP_RAYTRACE,
    OP_CONVERT,
    OP_EVAL,
    OP_TESTING,
    OP_REFLECTION,
    OP_REFRACTION,
    OP_EMISSION,
    OP_NORMALMAP,
    OP_INVERTNORMALS,
    OP_CLOTH,
    OP_SERVER,
    OP_CONNECT,
    OP_SEND,
    MAX_OPS
};

typedef struct {
    OP_TYPE type;
    const char * help;
    const char * def;
    bool append; // whether to append the instruction to an object or not
} help_t;

help_t help[] = {
    {OP_QUIT,       "quit",                             "quits the program", false},
    {OP_SAVE,       "save <scene name>",                "saves a scene file", false},
    {OP_READOBJ,    "open <filename>",                  "reads in an OBJ file", false},
    {OP_WRITE,      "write <filename>",                  "writes an OBJ file", false},
    {OP_ZFAR,       "zfar <amount>",                    "sets the current cameras zFar parameter", true},
    {OP_ZNEAR,      "znear <amount>",                   "sets the current cameras zNear parameter", true},
    {OP_FOVY,       "fovy <amount>",                    "sets the current cameras fovy parameter", true},
    {OP_TEXTURE,    "texture <filename>",               "applies a texture to the current selected geometry", true},
    {OP_DRAW,       "draw",                             "sets draw mode", false},
    {OP_BEZIER2,    "bezier2 <lod>",                    "bezier interpolation of the current geometry", true},
    {OP_BEZIER,     "bezier <lod>",                     "bezier interpolation of the current geometry", true},
    {OP_BSPLINE,    "bspline <lod>",                    "bspline interpolation of the current geometry", true},
    {OP_EXTRUDE,    "extrude <x> <y> <z>",              "extrudes the current geometry along an axis", true},
    {OP_REVOLVE,    "revolve <degrees> <x> <y> <z>",    "revolves the current geometry around an arbitrary axis", true},
    {OP_COMPUTE,    "compute [meld]",					"computes normals for basic geometry and [melds vertex indices]", true},
    {OP_BOUNDS,     "bounds",                           "displays the bounding box for the selected geometry", false},
    {OP_DIFFUSE,    "diffuse <R> <G> <B> <A>",          "sets the diffuse property for the current object", true},
    {OP_SPECULAR,   "specular <R> <G> <B> <A>",         "sets the specular property for the current object", true},
    {OP_AMBIENT,    "ambient <R> <G> <B> <A>",          "sets the ambient property for the current object", true},
    {OP_SHININESS,  "shininess <amount>",               "sets the shininess property for the current geometry", true},
    {OP_ROTATE,     "rotate <degress> <x> <y> <z>",     "rotates the current object around an arbitrary axis", true},
    {OP_TRANSLATE,  "translate <x> <y> <z>",            "translates the current object", true},
    {OP_SCALE,      "scale <x> <y> <z>",                "scales the current object", true},
    {OP_ADD,        "add <light|cam|grid|cube|sphere> ", "adds a new object to the scene", false},
    {OP_LOAD,       "load <scene name>",                "loads the specified scene into the current scene", false},
    {OP_DELETE,     "delete",                           "deletes the current object", false},
    {OP_VEXPLORE,   "Vexplore",                         "split the window vertically", false},
    {OP_SEXPLORE,   "Sexplore",                         "split the window horizontally", false},
    {OP_MATRIX,     "matrix <0> <1> ... <15>",          "set the the 4x4 matrix for the current object", true},
    {OP_POSITION,   "position <x> <y> <z>",             "set the position for the current camera", true},
    {OP_UP,         "up <x> <y> <z>",                   "set the up vector for the current camera", true},
    {OP_LOOKAT,     "lookat <obj> <refObj>",               "set up a lookat relation", true},
    {OP_NAME,       "name <name>",                      "name the current object", false},
    {OP_CLEAR,      "clear",                            "clears the buffer", false},
    {OP_PROPERTIES, "properties",                       "lists the properties of the current object", false},
    {OP_MATRIX_INFO,"minfo",                            "lists the matrix of the current object", false},
    {OP_LIST,       "list",                             "lists all objects", false},
    {OP_VIEW,       "view <model|channel>",             "changes the current window view", false},
    {OP_CHANNEL,    "channel <expr(tx|ty|tz|sx|sy)>",   "mathematically modify a channel", true},
    {OP_HISTORY,    "history",                          "displays history commands of an object", false},
    {OP_LIGHT,      "light <param> <value>",            "modify light parameters", true},
    {OP_GEO,        "geo <param> <value>",              "modify geometry parameters", true},
    {OP_COPY,       "copy <target> <template>",         "modify geometry parameters", false},
    {OP_HELP,       "help",                             "help documentation", false},
    {OP_SELECT,     "select <objname>",                 "select an object by name", false},
    {OP_CAMERA,     "cam <command>",                    "control the camera", false},
    {OP_OBJECT,     "obj",                              "commands for objects", false},
    {OP_BEGIN,      "begin",                            "begin command", false},
    {OP_END,        "end",                              "end command", false},
    {OP_REGISTER,   "register <object> <param> <channel> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]", "register an animation", false},
    {OP_REFERENCE,  "reference <refObject> <refParam> <object> <param> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]", "allows objects parameters to be controlled by others", false},
    {OP_SWEEP,	    "sweep <crossSection> [<hallway>, <onpath>]", "sweeps an object along a curve, onpath->along selected positions", true},
    {OP_PATH,	    "path <target> <path> [<dx> <dy> <dz> <freqscalar> <timeShift> <start> <end>]", "create a path object", false},
    {OP_POLY,	    "poly <centoid|zmin|zmax|...|texture> [<s> <t>]", "modify polygons", true},
    {OP_RENDER,	    "render", "render current screen", false},
    {OP_TIMELINE,	"timeline <play|stop|set> [<arg>]", "modify the timeline", false},
    {OP_DISPLAY,	"display <none|default>", "change display settings", false},
    {OP_CLEARALL,	"clearall",							"clears the polygon of the selected geo", false},
    {OP_CHILDREN,	"children <lookat>",		   	"perform operations of children of an object", true},
    {OP_RAYTRACE,	    "raytrace", "raytraces current screen", false},
    {OP_CONVERT,	    "convert <obj> <type>", "convert object types", false},
    {OP_EVAL, "eval <some expression>", "evaluate expression", false},
    {OP_TESTING, "testing", "", false},
    {OP_REFLECTION, "reflection <r> <g> <b>", "modify reflection attributes", true},
    {OP_REFRACTION, "refraction <index>", "modify refractive index", true},
    {OP_EMISSION, "emission <r> <g> <b>", "modify emission attributes", true},
    {OP_NORMALMAP, "normalmap <filename>", "add normal map", true},
    {OP_INVERTNORMALS, "invertNormals", "inverts normals", true},
    {OP_CLOTH, "cloth", "creates a cloth object", true},
    {OP_SERVER, "server <start|stop>", "start or stop a server", false},
    {OP_CONNECT, "connect <ip> <port>", "connect to a server", false},
    {OP_SEND, "send <msg>", "sends a message over a socket connection", false},
};

typedef void (*fn_) (vector<String>tokens);
typedef struct { const char * name; OP_TYPE type; } cmdMap_t;
cmdMap_t Map[] = {
    {"save",        OP_SAVE},
    {"quit",        OP_QUIT},
    {"q",           OP_QUIT},
    {"file",        OP_READOBJ},
    {"open",        OP_READOBJ},
    {"o",           OP_READOBJ},
    {"write",       OP_WRITE},
    {"znear",       OP_ZNEAR},
    {"zfar",        OP_ZFAR},
    {"fovy",        OP_FOVY},
    {"texture",     OP_TEXTURE},
    {"draw",        OP_DRAW},
    {"bezier2",     OP_BEZIER2},
    {"bezier",      OP_BEZIER},
    {"bspline",     OP_BSPLINE},
    {"extrude",     OP_EXTRUDE},
    {"revolve",     OP_REVOLVE},
    {"compute",     OP_COMPUTE},
    {"bounds",      OP_BOUNDS},
    {"diffuse",     OP_DIFFUSE},
    {"specular",    OP_SPECULAR},
    {"ambient",     OP_AMBIENT},
    {"shininess",   OP_SHININESS},
    {"rotate",      OP_ROTATE},
    {"translate",   OP_TRANSLATE},
    {"m",           OP_TRANSLATE},
    {"mv",          OP_TRANSLATE},
    {"scale",       OP_SCALE},
    {"add",         OP_ADD},
    {"load",        OP_LOAD},
    {"delete",      OP_DELETE},
    {"Vexplore",    OP_VEXPLORE},
    {"Sexplore",    OP_SEXPLORE},
    {"matrix",      OP_MATRIX},
    {"position",    OP_POSITION},
    {"up",          OP_UP},
    {"lookat",      OP_LOOKAT},
    {"name",        OP_NAME},
    {"clear",       OP_CLEAR},
    {"pinfo",       OP_PROPERTIES},
    {"minfo",       OP_MATRIX_INFO},
    {"ls",          OP_LIST},
    {"list",        OP_LIST},
    {"view",        OP_VIEW},
    {"channel",     OP_CHANNEL},
    {"chan",        OP_CHANNEL},
    {"c",           OP_CHANNEL},
    {"history",     OP_HISTORY},
    {"hinfo",       OP_HISTORY},
    {"light",       OP_LIGHT},
    {"l",           OP_LIGHT},
    {"geo",         OP_GEO},
    {"copy",        OP_COPY},
    {"cp",          OP_COPY},
    {"h",           OP_HELP},
    {"help",        OP_HELP},
    {"s",           OP_SELECT},
    {"select",      OP_SELECT},
    {"cam",         OP_CAMERA},
    {"camera",      OP_CAMERA},
    {"object",      OP_OBJECT},
    {"obj",         OP_OBJECT},
    {"begin",       OP_BEGIN},
    {"end",         OP_END},
    {"reg",         OP_REGISTER},
    {"register",    OP_REGISTER},
    {"ref",         OP_REFERENCE},
    {"reference",   OP_REFERENCE},
    {"sweep",       OP_SWEEP},
    {"path",        OP_PATH},
    {"poly",        OP_POLY},
    {"render",      OP_RENDER},
    {"timeline",    OP_TIMELINE},
    {"time",        OP_TIMELINE},
    {"display",     OP_DISPLAY},
    {"disp",        OP_DISPLAY},
    {"clearall",	OP_CLEARALL},
    {"children",	OP_CHILDREN},
    {"raytrace",	OP_RAYTRACE},
    {"convert",	    OP_CONVERT},
    {"eval",        OP_EVAL},
    {"testing",		OP_TESTING},
    {"reflection",  OP_REFLECTION},
    {"refl",		OP_REFLECTION},
    {"refraction",	OP_REFRACTION},
    {"emission",	OP_EMISSION},
    {"normalmap",	OP_NORMALMAP},
    {"invertNormals", OP_INVERTNORMALS},
    {"cloth", OP_CLOTH},
    {"server", OP_SERVER},
    {"server", OP_CONNECT},
    {"send", OP_SEND},
};


/* helpers */

bool Operations::add(String cmdName) {
    OP_TYPE type = (OP_TYPE)NULL;
    for(unsigned int i=0;i<sizeof(Map)/sizeof(cmdMap_t); i++) {
        if (!strcmp(cmdName.c_str(), Map[i].name)) type = Map[i].type;
    }
    for(unsigned int i=0;i<MAX_OPS;i++) {
        if (type == help[i].type) return help[i].append;
    }
    return false;
}

String wrong_args(OP_TYPE type) {return String(WRONG_ARGUMENTS + String("\n") + String(help[type].help));}

void threeFloats(vector<String>&args, float * x, float * y, float * z) {
    if (args.size() >= 3) {
        *x = s2f(0);
        *y = s2f(1);
        *z = s2f(2);
    } else {*x=*y=*z=0;}
}

void fourFloats(vector<String>&args, float * x, float * y, float * z, float *w) {
    if (args.size() >= 4) {
        threeFloats(args, x, y, z);
        *w = s2f(3);
    } else {*x=*y=*z=*w=0;}
}

Operations::Operations() {}

/* closes the window */
void Operations::quit(vector<String>args) {
    Scene::window->close();
}

/* save scene files 
 *
 *  save <filename>
 *
 */

void Operations::save(vector<String>args) {
    if (args.size() > 0) {
        String filename = args(0);
        Scene::dumpScene(filename.c_str());
        outbuf_ = String(filename + " has been written");
    } else outbuf_ = String("Please specify a Scene name");
}

/*
 * reads obj files and creates a new geometry
 *
 * open <filename>
 */

void Operations::readOBJ(vector<String>args) {

    /* did they specify a filename? */
    if (args.size() <= 0) {
        outbuf_ = String(NO_FILE_SPECIFIED);
        return; 
    }
    String filename = *(args.begin());

    /* does the file exist? */
    if (FILE * file = fopen(filename.c_str(), "r")) 
        fclose(file);
    else {
        outbuf_ = String(filename + " does not exist");
        return;
    }

    Geo * g = new Geo();
    Scene::addGeo(g);
    Poly * p = NULL;
    if ((p = PL(Scene::getObjectByFile(filename))) == NULL) {
        p = new Poly(filename);
        Scene::addPoly(p);	
        outbuf_ = String(filename + " read into memory");
    } else outbuf_ = String(filename + " already in memory");
    P(g)=p;
    Scene::setActive(GEN(g)); 
}


/*
 * writes an files
 * 
 * write <filename>
 *
 */

void Operations::write(vector<String>args) { 
    if (args.size() > 0) {
        if (isGeo(selOBJ) && P(selOBJ)) {
            vector<String>tokens=vector<String>();
            Term::Tokenize(args[0], tokens, ".");
            if (tokens.size() > 0) {
                String basename = tokens[0];
                String obj = basename + String(".obj");
                String omtl = basename + String("-obj.mtl");
                String mtl = basename + String(".mtl");
                G(selOBJ)->writeMTL(omtl);
                P(selOBJ)->writeMTL(mtl);
                P(selOBJ)->writeOBJ(obj);
                outbuf_ = String(obj + ", " + mtl + " written");
            } else {
                P(selOBJ)->writeOBJ(args[0]);        
                outbuf_ = String(args[0] + " written");
            }
        } else if (isPoly(selOBJ)) {
            PL(selOBJ)->writeOBJ(args[0]);
            outbuf_ = String(args[0] + " written");
        } else if (isChannel(selOBJ)) {
            CH(selOBJ)->WriteCHAN(args[0]);
            outbuf_ = String(args[0] + " written");
        } else if (isTexture(selOBJ)) {

        } else if (isImage(selOBJ)) {
            I(selOBJ)->WriteImage(args[0]);
            outbuf_ = String(args[0] + " written");
        } else {outbuf_ = String(NO_GEO_SELECTED);return;}
    } else outbuf_ = wrong_args(OP_WRITE);
}

/*
 * sets the cameras zNear parameter
 *
 * zNear <amount>
 *
 */

void Operations::zNear(vector<String>args) { 
    if (args.size() > 0) {
        SceneCAM->zNear = s2f(0);
        outbuf_ = String("zNear set");
    } else outbuf_ = String(NO_AMOUNT_SPECIFIED);
}

/*
 * sets the cameras zFar parameter
 *
 * zFar <amount>
 *
 */

void Operations::zFar(vector<String>args) { 
    if (args.size() > 0) {
        SceneCAM->zFar = s2f(0);
        outbuf_ = String("zFar set");
    } else outbuf_ = String(NO_AMOUNT_SPECIFIED);
}

void Operations::invertNormals(vector<String>args) { 
    if (isGeo(selOBJ)) {
        if (P(selOBJ)){
            P(selOBJ)->invertNormals();
            outbuf_ = String("inverted normals");			
        }
    }
}

/*
 * sets the cameras fovy parameter
 *
 * fovy <amount>
 *
 */

void Operations::fovy(vector<String>args) { 
    if (args.size() > 0) {
        SceneCAM->fovy = s2f(0);
        outbuf_ = String("fovy set");
    } else outbuf_ = String(NO_AMOUNT_SPECIFIED);
}


/* 
 *  draw
 *  ---- >> maybe if args are there, we should plot these points???? 
 */

void Operations::draw(vector<String>args) {

}

/*
 * applies texture for the current geometry
 *
 * normalmap <filename>
 *
 */

void Operations::normalMap(vector<String>args) {
    if (args.size() > 0) {
        String filename = args[0];
        Image * normmap = NULL;
        Generic * obj = selOBJ;
        if (!(normmap = I(Scene::getObjectByFile(filename)))) {
            normmap = new Image(filename);
            normmap->filepath = filename;
            Scene::add(GEN(normmap));
        }
        selOBJ = obj;
        if (isGeo(selOBJ)) {
            G(selOBJ)->normalMap = normmap;
            if (!P(selOBJ)) {
                for(unsigned int o=0;o<G(selOBJ)->objs.size();o++) {
                    if (isGeo(G(selOBJ)->objs[o])) G(G(selOBJ)->objs[o])->normalMap = normmap;
                }
            }
            outbuf_ = String("Map applied");
        } else outbuf_ = String("texture in memory, but not applied to anything");  

    } else outbuf_ = String(NO_FILE_SPECIFIED);
}
/*
 * applies texture for the current geometry
 *
 * texture <filename>
 *
 */

void Operations::texture(vector<String>args) {
    if (args.size() > 0) {
        String filename = args[0];
        Generic * obj = selOBJ;
        Texture * tex = Texture::addTexture(filename);
        selOBJ = obj;
        if (isGeo(selOBJ)) {
            G(selOBJ)->tex = tex;
            if (!P(selOBJ)) {
                for(unsigned int o=0;o<G(selOBJ)->objs.size();o++) {
                    if (isGeo(G(selOBJ)->objs[o])) G(G(selOBJ)->objs[o])->tex = tex;
                }
            }
            outbuf_ = String("Texture applied");
        } else outbuf_ = String("texture in memory, but not applied to anything");  

    } else outbuf_ = String(NO_FILE_SPECIFIED);
}

/*
 * applies a bezier interpolation 
 *
 * bezier <level of detail>
 *
 */

void Operations::Bezier(vector<String>args) {
    String lod = String();
    if (args.size() > 0) lod = args(0);
    else lod = String("20");
    if (isGeo(selOBJ)) {
        P(selOBJ)->Bezier(atoi(lod.c_str()));
        outbuf_ = String("Bezier interpolation applied");
    } else outbuf_ = String(NO_GEO_SELECTED);  
}

/*
 * applies a bspline interpolation 
 *
 * bspline <level of detail>
 *
 */

void Operations::Bspline(vector<String>args) {
    String lod = String();
    if (args.size() > 0) lod = args(0);
    else lod = String("5");
    if (isGeo(selOBJ)) {
        P(selOBJ)->Bspline(atoi(lod.c_str()));
        outbuf_ = String("Bspline interpolation applied");
    } else outbuf_ = String(NO_GEO_SELECTED);  
}

/*
 * applies a bezier interpolation using recursive subdivision
 *
 * bezier2 <level of detail>
 *
 */

void Operations::Bezier2(vector<String>args) {
    String lod = String();
    if (args.size() > 0) lod = args(0);
    else lod = String("5");
    if (isGeo(selOBJ)) {
        P(selOBJ)->Bezier2(atoi(lod.c_str()));
        outbuf_ = String("Bezier interpolation applied");
    } else outbuf_ = String(NO_GEO_SELECTED);  
}

/*
 * extrudes the current geos polys around an arbitrary axis
 *
 * extrude <x> <y> <z>
 *
 */

void Operations::Extrude(vector<String>args) {
    if (args.size() == 3) {
        if (isGeo(selOBJ)) {
            P(selOBJ)->Extrude(s2f(0),s2f(1),s2f(2));
            outbuf_ = String("extruded geometry");
            G(selOBJ)->bound();
        } else outbuf_ = String(NO_GEO_SELECTED);
    } else if (args.size() == 4) {

        if (isGeo(selOBJ)) {
            P(selOBJ)->ExtrudeFace((unsigned int) s2i(0), Point(s2f(1), s2f(2), s2f(3)));
            outbuf_ = String("extruded geometry");
            G(selOBJ)->bound();
        } else outbuf_ = String(NO_GEO_SELECTED); 

    } else outbuf_ = wrong_args(OP_EXTRUDE);
}

/*
 * revolves the current geos polys around an arbitrary axis
 *
 * revolve <degrees> <x> <y> <z>
 *
 */

void Operations::Revolve(vector<String>args) {
    if (args.size() == 4) {
        if (isGeo(selOBJ)) {
            float degs, rx, ry, rz;
            fourFloats(args, &degs, &rx, &ry, &rz);
            P(selOBJ)->Revolve(degs, rx, ry, rz);
            outbuf_ = String("revolved geometry");
            G(selOBJ)->bound();
        } else outbuf_ = String(NO_GEO_SELECTED);
    } else outbuf_ = wrong_args(OP_REVOLVE);
}

/*
 * computes normals
 *  
 * compute
 */

void Operations::compute(vector<String>args) {
    if (isGeo(selOBJ) && P(selOBJ)) {
        if (!args.size()) {
            P(selOBJ)->computeNormals();
            outbuf_ = String("computed normals");
        }
        else if (args.size() >= 1) {
            String type = args(0);
            if (match("smooth")) {
                P(selOBJ)->computeSmoothNormals();
                outbuf_ = String("computed smooth normals");
            }
        } else outbuf_ = wrong_args(OP_COMPUTE);
    } else outbuf_ = String(NO_GEO_SELECTED);
}

/* 
 * displays bounding box
 *
 * bounds
 * 
 */

void Operations::bounds(vector<String>args) {
    if (isGeo(selOBJ)) {
        //if (P(selOBJ)) P(selOBJ)->showBounds = ! (P(selOBJ)->showBounds);
        G(selOBJ)->bound();
        G(selOBJ)->showBounds = ! (G(selOBJ)->showBounds);
        outbuf_ = String("bounds...");
    } else if (isChannel(selOBJ)) {
        CH(selOBJ)->setBounds();
        outbuf_ = String("bounds...");
    }
}

/*
 * set the diffuse property for objects
 *
 * diffuse <R> <G> <B> <A>
 */

void Operations::diffuse(vector<String>args) {
    if (args.size() == 4) {
        float r,g,b,a;
        fourFloats(args, &r, &g, &b, &a);
        if (isGeo(selOBJ)) {
            G(selOBJ)->diffuse[0] = r;
            G(selOBJ)->diffuse[1] = g;
            G(selOBJ)->diffuse[2] = b;
            G(selOBJ)->diffuse[3] = a;
        } else if (isLight(selOBJ)) {
            L(selOBJ)->diffuse[0] = r;
            L(selOBJ)->diffuse[1] = g;
            L(selOBJ)->diffuse[2] = b;
            L(selOBJ)->diffuse[3] = a;
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("diffuse propery set");
    } else outbuf_ = wrong_args(OP_DIFFUSE);
}

/*
 * set the specular property for objects
 *
 * specular <R> <G> <B> <A>
 */

void Operations::specular(vector<String>args) {
    if (args.size() == 4) {
        float r,g,b,a;
        fourFloats(args, &r, &g, &b, &a);
        if (isGeo(selOBJ)) {
            G(selOBJ)->specular[0] = r;
            G(selOBJ)->specular[1] = g;
            G(selOBJ)->specular[2] = b;
            G(selOBJ)->specular[3] = a;
        } else if (isLight(selOBJ)) {
            L(selOBJ)->specular[0] = r;
            L(selOBJ)->specular[1] = g;
            L(selOBJ)->specular[2] = b;
            L(selOBJ)->specular[3] = a;
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("specular propery set");
    } else outbuf_ = wrong_args(OP_SPECULAR);
}

/*
 * set the ambient property for objects
 *
 * ambient <R> <G> <B> <A>
 */

void Operations::ambient(vector<String>args) {
    if (args.size() == 4) {
        float r,g,b,a;
        fourFloats(args, &r, &g, &b, &a);
        if (isGeo(selOBJ)) {
            G(selOBJ)->ambient[0] = r;
            G(selOBJ)->ambient[1] = g;
            G(selOBJ)->ambient[2] = b;
            G(selOBJ)->ambient[3] = a;
        } else if (isLight(selOBJ)) {
            L(selOBJ)->ambient[0] = r;
            L(selOBJ)->ambient[1] = g;
            L(selOBJ)->ambient[2] = b;
            L(selOBJ)->ambient[3] = a;
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("ambient propery set");
    } else outbuf_ = wrong_args(OP_AMBIENT);
}

/*
 * set the shininess property for objects
 *
 * shininess <amount>
 */

void Operations::shininess(vector<String>args) {
    if (args.size() == 1) {
        if (isGeo(selOBJ)) {
            G(selOBJ)->shininess[0] = s2f(0);
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("shininess propery set");
    } else outbuf_ = wrong_args(OP_SHININESS);
}

void Operations::reflection(vector<String>args) {
    if (args.size() == 3) {
        if (isGeo(selOBJ)) {
            G(selOBJ)->reflection[0] = s2f(0);
            G(selOBJ)->reflection[1] = s2f(1);
            G(selOBJ)->reflection[2] = s2f(2);
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("reflection propery set");
    } else outbuf_ = wrong_args(OP_REFLECTION);
}
void Operations::refraction(vector<String>args) {
    if (args.size() == 1) {
        if (isGeo(selOBJ)) {
            G(selOBJ)->refractiveIndex = s2f(0);
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("refractive propery set");
    } else outbuf_ = wrong_args(OP_REFRACTION);
}

void Operations::emission(vector<String>args) {
    if (args.size() == 4) {
        if (isGeo(selOBJ)) {
            G(selOBJ)->emission[0] = s2f(0);
            G(selOBJ)->emission[1] = s2f(1);
            G(selOBJ)->emission[2] = s2f(2);
            G(selOBJ)->emission[3] = s2f(3);
        } else {
            outbuf_ = String(WRONG_OBJECT);
            return;
        }
        outbuf_ = String("emission propery set");
    } else outbuf_ = wrong_args(OP_EMISSION);
}

void Operations::rotate(vector<String>args) {
    if (args.size() == 4) {
        float deg, x, y, z;
        fourFloats(args, &deg, &x, &y, &z);
        Point axis(x, y, z);
        if (isGeo(selOBJ)) {
            G(selOBJ)->rotateObj(deg, axis);
        } else if (isLight(selOBJ)) {
            L(selOBJ)->rotateLight(deg, axis); 
        } else if (isCamera(selOBJ)) {
            // not done yet... 
        } else outbuf_ = String(WRONG_OBJECT);
    } else outbuf_ = wrong_args(OP_ROTATE);
}

void Operations::translate(vector<String>args) {
    if (args.size() == 3) {
        float tx, ty, tz;
        threeFloats(args, &tx, &ty, &tz);
        if (isCamera(selOBJ)) {
            C(selOBJ)->eye.x +=tx;
            C(selOBJ)->eye.y +=ty;
            C(selOBJ)->eye.z +=tz;
            C(selOBJ)->focus.x +=tx;
            C(selOBJ)->focus.y +=ty;
            C(selOBJ)->focus.z +=tz;
        } else if (isObject(selOBJ)) {
            O(selOBJ)->M.a03 += tx;
            O(selOBJ)->M.a13 += ty;
            O(selOBJ)->M.a23 += tz;
        } else outbuf_ = String(WRONG_OBJECT);
    } else outbuf_ = wrong_args(OP_TRANSLATE);
}

void Operations::scale(vector<String>args) {
    if (args.size() == 3) {
        if (isGeo(selOBJ)) {
            float sx, sy, sz;
            threeFloats(args, &sx, &sy, &sz);
            if ( sx && sy && sz) {
                O(selOBJ)->M.a00 *= sx;
                O(selOBJ)->M.a11 *= sy;
                O(selOBJ)->M.a22 *= sz;
                if(G(selOBJ)->p){
                    mat4 Nmult;				
                    for(unsigned int i = 0; i<G(selOBJ)->p->normals.size(); i++){
                        G(selOBJ)->p->scaleNormals(sx, sy, sz);
                    }
                }
                for(unsigned int i =0; i<OBJS(G(selOBJ)).size();i++){
                    if(isGeo(OBJS(G(selOBJ))[i])){
                        if(G(OBJS(G(selOBJ))[i])->p){
                            G(OBJS(G(selOBJ))[i])->p->scaleNormals(sx, sy, sz);
                        }
                    }
                }	
            } else outbuf_ = String("cannot scale by 0");
        } else outbuf_ = String(NO_GEO_SELECTED);
    } else outbuf_ = wrong_args(OP_SCALE);
}

void Operations::add(vector<String>args) {
    if (args.size() >= 1) {
        String type = args(0);
        if (match("light")) {
            int numLights = 0;
            for(unsigned int i=0;i<allOBJS.size();i++) {
                if (isLight(allOBJS[i])) numLights++;
            }
            if (numLights < GL_MAX_LIGHTS) {
                Light * l = new Light();
                Scene::addLight(l);
            } else outbuf_ = String("you have the maximum number of lights");

        } else if (match("cam") || match("camera")) {
            Camera * c = new Camera();
            WindowManager::setCamera(c);
            SceneCAM = c;
            Scene::addCamera(c);
        } else if (match("geo")) {
            Geo * g = new Geo();
            Scene::addGeo(g);
        } else if (match("cube")) {
            Geo * g = (Geo *) new Cube();
            Scene::addGeo(g);
        } else if (match("grid")) {
            if (args.size() == 1) {
                Geo * grid = (Geo *) new Grid();
                Scene::addGeo(grid);
            } else if (args.size()==2) {
                // bscenes wont work after this ... Geo * grid = (Geo *) new Grid((s2i(1)>2)?s2i(1):2);
                Geo * grid = (Geo *) new Grid(s2i(1));
                Scene::addGeo(grid);
            } else if (args.size()==3) {
                Geo * grid = (Geo *) new Grid(s2f(1), s2f(2));
                Scene::addGeo(grid);
            } else if (args.size()==4) {
                Geo * grid = (Geo *) new Grid(s2i(1), s2i(2), s2f(3), 0.0f, 0.0f);
                Scene::addGeo(grid);
            } else if (args.size()==5) {
                Geo * grid = (Geo *) new Grid(s2i(1), s2i(2), s2f(3), s2f(4));
                Scene::addGeo(grid);
            } else if (args.size()==6) {
                Geo * grid = (Geo*) new Grid(s2i(1), s2i(2), s2f(3), s2f(4), s2f(5));
                Scene::addGeo(grid);
            }
        } else if (match("sphere")) {
            if (args.size()==1) {
                Geo * sphere = (Geo *) new Sphere(10.0f, 10.0f, 32);
                Scene::addGeo(sphere);	
            } else if (args.size()==2){
                Geo * sphere = (Geo *) new Sphere(s2f(1), s2f(1), 32);
                Scene::addGeo(sphere);						
            } else if (args.size()==3){
                Geo * sphere = (Geo *) new Sphere(s2f(1), s2f(2), 32);
                Scene::addGeo(sphere);						
            } else if (args.size()==4) {
                Geo * sphere = (Geo *) new Sphere(s2f(1), s2f(2), s2i(3));
                Scene::addGeo(sphere);		
            }
        } else if (match("ellipseFace")) {
            if (args.size()==1) {
                Geo * ell = (Geo *) new Ellipse(20.0f, 10.0f, 32, true);
                Scene::addGeo(ell);	
            } else if (args.size()==2) {
                Geo * ellipse = (Geo *) new Ellipse(20.0f, 10.0f, s2i(1), true);
                Scene::addGeo(ellipse);		
            } else if (args.size()==3) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), 32, true);
                Scene::addGeo(ellipse);		
            } else if (args.size()==4){
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), s2i(3), true);
                Scene::addGeo(ellipse);						
            }
        } else if (match("circleFace")) {
            if (args.size()==1) {
                Geo * ell = (Geo *) new Ellipse(10.0f, 10.0f, 32, true);
                Scene::addGeo(ell);	
            } else if (args.size()==2) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(1), 32, true);
                Scene::addGeo(ellipse);		
            } else if (args.size()==3) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), 32, true);
                Scene::addGeo(ellipse);		
            } else if (args.size()==4){
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), s2i(3), true);
                Scene::addGeo(ellipse);						
            }
        } else if (match("ellipse")) {
            if (args.size()==1) {
                Geo * ellipse = (Geo *) new Ellipse(10.0f, 15.0f, 32, false);
                Scene::addGeo(ellipse);	
            } else if (args.size()==2) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(1), 32, false);
                Scene::addGeo(ellipse);		
            } else if (args.size()==3) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), 32, false);
                Scene::addGeo(ellipse);		
            } else if (args.size()==4){
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), s2f(3), false);
                Scene::addGeo(ellipse);						
            }
        } else if (match("circle")) {
            if (args.size()==1) {
                Geo * ellipse = (Geo *) new Ellipse(10.0f, 10.0f, 32, false);
                Scene::addGeo(ellipse);	
            } else if (args.size()==2) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(1), 32, false);
                Scene::addGeo(ellipse);		
            } else if (args.size()==3) {
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(1), s2i(2), false);
                Scene::addGeo(ellipse);		
            } else if (args.size()==4){
                Geo * ellipse = (Geo *) new Ellipse(s2f(1), s2f(2), s2i(3), false);
                Scene::addGeo(ellipse);						
            }
        } else if (match("ring")) {
            int Vdepth = 32;
            float height = 10.0f;
            float width = 10.0f;
            float halfringWidth = .5f;
            if (args.size()>1) {
                height = s2f(1);
                width = s2f(1);
                if (args.size()==3) {
                    Vdepth = s2i(2);		
                } 
                if (args.size()>3){
                    width = s2f(2);
                    Vdepth = s2i(3);

                    if (args.size()>4){
                        halfringWidth = s2f(4)/.5f;
                    }
                }
            }
            Geo * ring1 = (Geo*) new Ellipse(height - halfringWidth, width - halfringWidth, Vdepth, false); 
            Geo * ring2 = (Geo*) new Ellipse(height + halfringWidth, width + halfringWidth, Vdepth, false);
            Geo * ring = new Geo();

            Poly * outerRing = ring1->p;
            Poly * innerRing = ring2->p;
            Poly * newP = new Poly();
            point_t::iterator cursor, cursor2;
            cursor = innerRing->vertices.begin();
            cursor2 = outerRing->vertices.begin();
            Point p1,  p2, p3, p4;
            while(true){
                if(cursor2>=outerRing->vertices.end()-1) break; //less than 3 vertices on this side
                p1 = *(cursor);
                p2 = *(cursor2);
                cursor2++;
                p3 = *(cursor2);
                cursor++;
                p4 = *(cursor);
                newP->addFace(p1, p2, p3, p4);
            }				
            ring->p = newP;
            Scene::addGeo(ring);

        } else if (match("channel")) {
            Channel * chan = (Channel *) new Channel();
            Scene::addChannel(chan);
        } else {
            outbuf_ = String("nothing to add");
            return;
        }
        outbuf_ = String("added " + type);
    } else wrong_args(OP_ADD);
}

void Operations::load(vector<String>args) {
    if (args.size() == 1) {
        String sceneName = args(0);
        Scene::loadScene(sceneName);
        outbuf_ = String("loading " + sceneName);
    } else if (args.size() == 2) {
        String type = args(0);
        if (match("channel")) {
            String dat = args(1);
            Channel * chan = new Channel(dat);
            Scene::addChannel(chan);
            outbuf_ = String("loading channel: " + dat);
        }

    } else outbuf_ = wrong_args(OP_LOAD);
}

void Operations::Delete(vector<String>args) {

  if (isCamera(selOBJ)) return;

  //  Generic * obj = selOBJ;
  //  selOBJ = NextElement(allOBJS, selOBJ);
  //  if (selOBJ == obj) {
  //      delete selOBJ;
  //      selOBJ = (Generic *) NULL;
  //      outbuf_ = String("Deleting last object");
  //  } else {
        gen_t::iterator cursor;
        for(cursor = allOBJS.begin(); cursor != allOBJS.end(); cursor++) {
            //if (obj == *cursor) {
            if (selOBJ == *cursor) {
                delete *cursor; // if any instances are referencing this, we should let them know
                allOBJS.erase( cursor );
                outbuf_ = String("Deleting object");
                break;
            }
        }
   // }
    selOBJ = NULL;
}

/* window functions */
void Operations::Vexplore(vector<String>args) {Scene::window->splitVertical();}
void Operations::Sexplore(vector<String>args) {Scene::window->splitHorizontal();}



void Operations::Matrix(vector<String>args) {
    if (args.size() == 16) {
        if (selOBJ!=NULL) {
            String arg1 = args(0); String arg2 = args(1); String arg3 = args(2);  String arg4 = args(3);
            String arg5 = args(4); String arg6 = args(5); String arg7 = args(6);  String arg8 = args(7);
            String arg9 = args(8); String arg10 = args(9); String arg11 = args(10);  String arg12 = args(11);
            String arg13 = args(12); String arg14 = args(13); String arg15 = args(14);  String arg16 = args(15);

            O(selOBJ)->M.a00 = atof(arg1.c_str());  O(selOBJ)->M.a01 = atof(arg2.c_str()); O(selOBJ)->M.a02 = atof(arg3.c_str()); O(selOBJ)->M.a03 = atof(arg4.c_str());
            O(selOBJ)->M.a10 = atof(arg5.c_str());  O(selOBJ)->M.a11 = atof(arg6.c_str()); O(selOBJ)->M.a12 = atof(arg7.c_str()); O(selOBJ)->M.a13 = atof(arg8.c_str());
            O(selOBJ)->M.a20 = atof(arg9.c_str());  O(selOBJ)->M.a21 = atof(arg10.c_str()); O(selOBJ)->M.a22 = atof(arg11.c_str()); O(selOBJ)->M.a23 = atof(arg12.c_str());
            O(selOBJ)->M.a30 = atof(arg13.c_str());  O(selOBJ)->M.a31 = atof(arg14.c_str()); O(selOBJ)->M.a32 = atof(arg15.c_str()); O(selOBJ)->M.a33 = atof(arg16.c_str());
        }
    } else outbuf_ = wrong_args(OP_MATRIX);
}

void Operations::Position(vector<String>args) {
    if (args.size() == 3) {
        float x, y, z;
        threeFloats(args, &x, &y, &z);
        if (isCamera(selOBJ)) {
            C(selOBJ)->eye.x = x;
            C(selOBJ)->eye.y = y;
            C(selOBJ)->eye.z = z;
        } else if (isObject(selOBJ)) {
            O(selOBJ)->M.a03 = x;
            O(selOBJ)->M.a13 = y;
            O(selOBJ)->M.a23 = z;
        } 
    } else outbuf_ = wrong_args(OP_POSITION);
}

void Operations::Up(vector<String>args) {
    if (args.size() == 3) {
        if (isCamera(selOBJ)) {
            float x, y, z;
            threeFloats(args, &x, &y, &z);
            C(selOBJ)->up.x = x;
            C(selOBJ)->up.y = y;
            C(selOBJ)->up.z = z;
        } else outbuf_ = String(WRONG_OBJECT);
    } else outbuf_ = wrong_args(OP_UP);
}

// lookat <obj> <lookatObject> [<start> <end>]
void Operations::Lookat(vector<String>args) {
    if (args.size() >= 2) {
        Generic * obj1 = objByName(0); 
        Generic * obj2 = objByName(1); 
        if (obj1 && obj2) {
            unsigned long int start = Scene::start;
            unsigned long int end = Scene::end;
            if (args.size() >= 4) {
                start = s2i(2);
                end = s2i(3);
            }
            Scene::register_reference(obj2, POS, obj1, LOOKAT, 1, -1, 0, 0, start, end);  
        } else outbuf_ = String(WRONG_OBJECT); 

    } else outbuf_ = wrong_args(OP_LOOKAT);
}

void Operations::Children(vector<String>args) {
    if (args.size() >= 2) {
        String type = args[0];
        if (match("lookat")) {
            Generic * lookatObj = objByName(1); 
            unsigned long int start = Scene::start;
            unsigned long int end = Scene::end;
            if (args.size() >= 4) {
                start = s2i(2);
                end = s2i(3);
            }
            if (isObject(selOBJ) && lookatObj) {
                for(unsigned int i=0;i<OBJS(selOBJ).size();i++) {
                    Scene::register_reference(lookatObj, POS, GEN(OBJS(selOBJ)[i]), LOOKAT, 1, -1, 0, 0, start, end);  
                }
            }
        }
    } 
}

void Operations::Name(vector<String>args) {
    if (args.size() == 1) {
        String name = *(args.begin());
        if (selOBJ != NULL) {
            if (Scene::nameExists(allOBJS,name)) {
                outbuf_ = String(name + " already exists.");
            } else {
                selOBJ->name = name;
                outbuf_ = String("named object " + name);
            }
        }
    } else outbuf_ = wrong_args(OP_NAME);
}
void Operations::Clear(vector<String>args) {outbuf_.clear();}

void Operations::clearall(vector<String>args){
    if(selOBJ){
        if(isGeo(selOBJ)){
            if (P(selOBJ)) P(selOBJ)->clearAll();
        }
    } else {
        outbuf_ = wrong_args(OP_NAME);
    }
}

void Operations::Properties(vector<String>args) {
    if (isObject(selOBJ)) {
        outbuf_ = O(selOBJ)->propertyInfo();
    } else if (isChannel(selOBJ)) outbuf_=CH(selOBJ)->propertyInfo(); 
}

void Operations::MatrixInfo(vector<String>args) {
    if (isObject(selOBJ)) {
        outbuf_ = O(selOBJ)->matrixInfo();
    } 
}

void Operations::History(vector<String>args) {
    if (isObject(selOBJ)) {
        outbuf_ = O(selOBJ)->historyInfo();
    } 
}

//void printme(Object * o) {outbuf_.append( o->printInfo() );}
void printGeo(Generic * o) { if (isGeo(o)) outbuf_.append( o->printInfo() ); }
void printCam(Generic * o) { if (isCamera(o)) outbuf_.append( o->printInfo() ); }
void printLight(Generic * o) { if (isLight(o)) outbuf_.append( o->printInfo() ); }
void printPolyInfo(Generic * p){ if (isPoly(p)) outbuf_.append( p->printInfo() );  }
void printTextureInfo(Generic * t){ if(isTexture(t)) outbuf_.append( t->printInfo() );}
void printChannelInfo(Generic * c){ if(isChannel(c))  outbuf_.append( c->printInfo() );}

void Operations::List(vector<String>args) {
    outbuf_.clear();
    if (args.size() == 0) {
        outbuf_.append(String("OBJECTS:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printGeo);
        outbuf_.append(String("\nPOLYGONS:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printPolyInfo);
        outbuf_.append(String("\nLIGHTS:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printLight);
        outbuf_.append(String("\nCAMERAS:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printCam);
        outbuf_.append(String("\nTEXTURES:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printTextureInfo);
        outbuf_.append(String("\nCHANNELS:\n"));
        for_each (allOBJS.begin(), allOBJS.end(), printChannelInfo);
    } else {
        for(unsigned int i=0; i<args.size();i++) {
            String type = args[i];
            if (match("chan")) {
                outbuf_.append(String("\nCHANNELS:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printChannelInfo);
            } else if (match("tex")) {
                outbuf_.append(String("\nTEXTURES:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printTextureInfo);
            } else if (match("cam")) {
                outbuf_.append(String("\nCAMERAS:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printCam);
            } else if (match("light")) {
                outbuf_.append(String("\nLIGHTS:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printLight);
            } else if (match("poly")) {
                outbuf_.append(String("\nPOLYGONS:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printPolyInfo);
            } else if (match("geo")) {
                outbuf_.append(String("OBJECTS:\n"));
                for_each (allOBJS.begin(), allOBJS.end(), printGeo);
            }
        }
    }
}

/* Channels */

void Operations::View(vector<String>args) {
    if (args.size() == 1) {
        String type = args[0];
        if (match("model")) Scene::window->changeView(MODEL_VIEW);
        else if (match("channel")) Scene::window->changeView(CHANNEL_VIEW);
        else if (match("pnormals")) Func::Toggle_POINT_NORMALS();
        else if (match("pnums")) Func::Toggle_POINT_NUMS();
        else if (match("fnormals") || match("normals")) Func::Toggle_FACE_NORMALS();
        else if (match("fnums") || match("nums")) Func::Toggle_FACE_NUMS();
    } else if (args.size()==2) {
        String type = args[0];
        if (match("cam") || match("camera")) {
            Generic * o = objByName(1);
            if (isCamera(o)) {
                Scene::window->setCamera(C(o));
                outbuf_ = String("camera changed");
                SceneCAM = C(o);
            }
        }
    } else if (args.size()==4) {
        String type = args[0];
        if (match("bgcolor")) {
            Scene::bgcolor[0] = s2f(1);
            Scene::bgcolor[1] = s2f(2);
            Scene::bgcolor[2] = s2f(3);
        } else if (match("textcolor")) {
            Scene::textcolor[0] = s2f(1);
            Scene::textcolor[1] = s2f(2);
            Scene::textcolor[2] = s2f(3);
        } else if (match("gridcolor")) {
            Scene::gridcolor[0] = s2f(1);
            Scene::gridcolor[1] = s2f(2);
            Scene::gridcolor[2] = s2f(3);
        }

    } else outbuf_ = wrong_args(OP_VIEW);
}

// can we use eval??
void Operations::ChannelOp(vector<String>args) {
    if (args.size() == 1 && isChannel(selOBJ)) {
        String type = args[0];
        if (match("derivative")) {
            Channel * diff = CH(selOBJ)->differentiate();
            diff->name = Scene::name(String("d" + CH(selOBJ)->name).c_str());
            Scene::add(GEN(diff));
        } else  if (match("sine")) {
            CH(selOBJ)->Sin();
        } else if (match("cosine")) {
            CH(selOBJ)->Cos();
        } else if (match("enable")) {
            CH(selOBJ)->enabled = true;
        } else if (match("disable")) {
            CH(selOBJ)->enabled = false;
        } else if (match("solo")) {
            for(unsigned int i=0; i<allOBJS.size();i++) {
                if (isChannel(allOBJS[i]) && allOBJS[i] != selOBJ) {
                    CH(allOBJS[i])->enabled = false;
                }
            }
            CH(selOBJ)->enabled = true;
        }
    } else if (args.size() == 2) {
        String type = args[0];
        if (match("start")) {
            CH(selOBJ)->start = s2i(1);
        } else if (match("end")) {
            CH(selOBJ)->end = s2i(1);
        } else if (match("scopeStart")) {
            CH(selOBJ)->scopeStart = s2i(1);
        } else if (match("scopeEnd")) {
            CH(selOBJ)->scopeEnd = s2i(1);
        }
    } else if (args.size() == 3) {
        // channel <op> <scalar>
        if (isChannel(selOBJ)) {
            String type = args[0];
            if (match("add")) {
                outbuf_ = String("adding");
                *CH(selOBJ) = *CH(selOBJ) + s2f(1);
            } else if (match("sub") || match("subtract")) {
                *CH(selOBJ) = *CH(selOBJ) - s2f(1);
            } else if (match("mult") || match("multiply")) {
                *CH(selOBJ) = *CH(selOBJ) * s2f(1);
            } else if (match("div") || match("divide")) {
                *CH(selOBJ) = *CH(selOBJ) / s2f(1);
            }
        }
    } else if (args.size() >= 4) {
        // differentiate, add, subtract 
        // channel <op> <storeObj> <opObj1> <opObj2>
        String type = args[0];
        Generic * storeObj = objByName(1);
        Channel * storeChan = NULL;

        // where to store data
        if (storeObj && !isChannel(storeObj)) {
            outbuf_ = String("Wrong type of object!");
            return;
        } else if (isChannel(storeObj)) {
            storeChan = CH(storeObj);
        } else {
            storeChan = new Channel();
            storeChan->name = Scene::name(args[1].c_str());
            Scene::add(storeChan);
        }
        String name = storeChan->name;

        // where to get data
        Generic * op1 = objByName(2);
        Generic * op2 = objByName(3);
        if (!isChannel(op1) || !isChannel(op2)) {
            outbuf_ = String("Wrong type of object!");
            return;
        }


        if (match("add")) {
            *storeChan = *CH(op1) + *CH(op2);
        } else if (match("sub") || match("subtract")) {
            *storeChan = *CH(op1) - *CH(op2);
        } else if (match("mult") || match("multiply")) {
            *storeChan = *CH(op2) * *CH(op1);
        } else if (match("div") || match("divide")) {
            *storeChan = *CH(op1) / *CH(op2);
        } else if (match("project")) {
            *storeChan = CH(op1)->project(CH(op2)); 
        } else if (match("combine")) {
            if (args.size() == 5) {
                Generic * op3 = objByName(4);
                if (isChannel(op3)) *storeChan = Channel::combine(CH(op1), CH(op2), CH(op3));
            } else *storeChan = Channel::combine(CH(op1), CH(op2));
            // maybe also have a convert function that takes three channels as args
        } else if (match("convolve")) {
            *storeChan = Channel::convolve(CH(op1),CH(op2)); 

        }
        storeChan->name = name; // because it may have been overwritten
        Scene::setActive(GEN(storeChan));

    } else outbuf_ = wrong_args(OP_CHANNEL);
}

void Operations::LightOp(vector<String>args) {
    if (isLight(selOBJ)) {
        if (args.size() >= 2) {
            String type = args(0);
            if (match("ltype")) {
                L(selOBJ)->ltype = s2i(1);
            } else if (match("cutoff")) {
                L(selOBJ)->cutoff = s2f(1);
            } else if (match("spot")) {
                if (args.size() == 4) {
                    L(selOBJ)->spotDir[0] = s2f(1);
                    L(selOBJ)->spotDir[1] = s2f(2);
                    L(selOBJ)->spotDir[2] = s2f(3);
                    L(selOBJ)->ltype = L_SPOT;
                }
            } else if (match("toggle")) { 
                L(selOBJ)->on = (bool) s2f(1);
            } else if (match("constant")) { 
                L(selOBJ)->constant = s2f(1);
            } else if (match("linear")) { 
                L(selOBJ)->linear = s2f(1);
            } else if (match("quadratic")) {
                L(selOBJ)->quadratic = s2f(1);
            } else if (match("atten")) {
                if (args.size() == 4) {
                    L(selOBJ)->constant = s2f(1);
                    L(selOBJ)->linear = s2f(2);
                    L(selOBJ)->quadratic = s2f(3);
                } else {
                    L(selOBJ)->constant = s2f(1);
                    L(selOBJ)->linear = s2f(1);
                    L(selOBJ)->quadratic = s2f(1);
                }
                outbuf_ = String("attenuation set");
            } else if (match("area")) {
                if (args.size() == 3) {
                    L(selOBJ)->ltype = L_AREA;
                    L(selOBJ)->w = s2f(1);
                    L(selOBJ)->h = s2f(2);
                    outbuf_ = String("set to area light");
                }
            }
        } else if (args.size() == 1) {
            String type = args(0);
            if (match("on")) {
                curSTATE |= LIGHTS_ON;
            } else if (match("off")) {
                curSTATE &= ~LIGHTS_ON;
            } else if (match("spot")) {
                /* set up a spot light */
                L(selOBJ)->spotDir[0] = -1.0f;
                L(selOBJ)->spotDir[1] = -1.0f;
                L(selOBJ)->spotDir[2] = 0.0f;
                L(selOBJ)->ltype = L_SPOT;
            } else if (match("dir")) {
                L(selOBJ)->ltype = L_DIR;
            } else if (match("point")) {
                L(selOBJ)->ltype = L_POINT;
            }

        } else outbuf_ = wrong_args(OP_LIGHT);
    } else outbuf_ = String("no light selected");
}

void Operations::ObjectOp(vector<String>args) {
    if (args.size() >= 2) {
        String type = args(0);
        if (match("group")) {
            unsigned int i=1;
            /* get all good objects */
            Geo * master = new Geo();
            Scene::addGeo(master);
            for(;i<args.size();i++){
                Generic * o = objByName(i);
                if (isObject(o)) {
                    master->addObj(O(o));
                    O(o)->parent=master;
                }

            }
            master->name = Scene::name(String("Group"));
            master->group = true;
            outbuf_ = String(master->name + " created");
        } else if (args.size() == 3) {
            if (match("child")) {
                Generic * p = objByName(1);
                Generic * c = objByName(2);
                if (isObject(p) && isObject(c)) {
                    O(p)->addObj(O(c));
                    O(c)->parent = O(p);
                    O(c)->child = true;
                    outbuf_ = String(p->name + " is now the parent of " + c->name);
                } else outbuf_ = String("specified objects don't exist");
            }
        } else outbuf_ = wrong_args(OP_OBJECT);
    } else outbuf_ = wrong_args(OP_OBJECT);

}

void Operations::GeoOp(vector<String>args) {
    if (isGeo(selOBJ)){
        if (args.size() >= 2) {
            String type = args(0);

            // geo instance <nodeName>
            if (match("instance")) {
                Generic * o = objByName(1);
                if (isGeo(o) && (selOBJ != o)) {
                    P(selOBJ) = P(o);
                    outbuf_ = String(selOBJ->name + " is referencing " + o->name);
                }
                // geo child <nodeName>
            } else if (match("child")) {
                Generic * o = objByName(1);
                if (isGeo(o) && (selOBJ != o)) {
                    G(selOBJ)->addGeo(G(o));
                    outbuf_ = String(selOBJ->name + " is referencing " + o->name);
                }
            }  else if (match("emission")) {
                if (args.size() == 5) {
                    G(selOBJ)->emission[0] = s2f(1);
                    G(selOBJ)->emission[1] = s2f(2);
                    G(selOBJ)->emission[2] = s2f(3);
                    G(selOBJ)->emission[3] = s2f(4);
                }
            } else if (match("display")) {
                G(selOBJ)->disp = (bool) s2i(1);
            } else if (match("write")) {
                G(selOBJ)->writeGeometry(args[1]);
            }
        }
    } else outbuf_ = String("no geo selected");
}

void Operations::CameraOp(vector<String>args) {
    if (isCamera(SceneCAM)) {
        if (args.size() >= 1) {
            String type = args[0];
            if(match("top")) {
                SceneCAM->top();
            } else if(match("r") || match("right")) {
                SceneCAM->right();
            } else if(match("l") || match("left")) {
                SceneCAM->left();
            } else if(match("back") || match("b")){
                SceneCAM->back();
            } else if(match("front") || match("f")){
                SceneCAM->front();
            } else if (match("lookat")) {
                if (args.size() == 4) {
                    if (isCamera(selOBJ)) {
                        C(selOBJ)->focus.x = s2f(1);
                        C(selOBJ)->focus.y = s2f(2);
                        C(selOBJ)->focus.z = s2f(3);
                    } 
                } 
            } else if (match("distance")) {
                SceneCAM->setDistance();
            }

            if (args.size()==10) {
                Camera * c = new Camera();
                WindowManager::setCamera(c);
                SceneCAM = c;
                Scene::addCamera(c);
                c->eye = Point(s2f(0), s2f(1), s2f(2));
                c->focus = Point(s2f(3), s2f(4), s2f(5));
                c->up = Point(s2f(6), s2f(7), s2f(8));
                c->fovy = s2f(9);
                outbuf_ = String("added new camera");
            }
        } else outbuf_ = wrong_args(OP_CAMERA);
    } else outbuf_ = String("no camera selected");
}

/* copy <target> <template> */

void Operations::Copy(vector<String>args) {
    if (args.size() >= 2) {
        Generic * target = objByName(0);
        Generic * tmplt = objByName(1);
        if (isGeo(target) && isGeo(tmplt) && P(tmplt)) {
            Geo * master = new Geo();
            /* copy, Target, Template are stricly used
               for writing files */
            master->copy=true;
            master->Target = target;
            master->Template = tmplt;
            Point f=Point();

            bool align, useGeo, keep, pts;
            align = useGeo = keep = pts = false;
            for(unsigned int j=2;j<args.size();j++) {
                String type = args[j];
                if (match("geo")) useGeo = true;
                else if (match("align")) align = true;
                else if (match("keep")) keep = true;
                else if (match("points")) pts = true;
            }

            for(unsigned int i = 0; i < P(tmplt)->vertices.size(); i++) {

                Geo * child = new Geo(); 
                child->inst = true;
                child->geoType = INSTANCE_GEO;

                if (!useGeo) {
                    P(child)=P(target);
                } else {
                    G(child)->addGeo(G(target));
                }

                if (align) {
                    if (i < P(tmplt)->vertices.size()-1) {
                        f = P(tmplt)->vertices[i+1] - P(tmplt)->vertices[i];
                    }                
                    child->M = Transform::coordinateFrame(Point(0.001,1,0.002), f);
                }

                child->M.a03 = P(tmplt)->vertices[i].x;
                child->M.a13 = P(tmplt)->vertices[i].y;
                child->M.a23 = P(tmplt)->vertices[i].z;

                master->addGeo(child);
                /*
                   normally we reference the polygons, but if there
                   are not any, simply reference geometry itself

                 */
                if (!P(child)) {
                    child->addObj(O(target));
                    O(target)->parent = child;
                }
            }

            if (pts) {
                Poly * np = G(master)->getPoly();
                delete master;
                Geo * master = new Geo();
                P(master) = np;
                master->name = Scene::name("Points");    
            }
            Scene::add(GEN(master));

            if (!keep) {
                G(tmplt)->disp = false;
                if (!useGeo) G(target)->disp = false;
            }

            String cp("Copy");
            master->name = Scene::name(cp);
            outbuf_ = String(master->name + " is copied");

        } else if (isGeo(target)) {
            Geo * newG = new Geo();
            Scene::add(O(newG));
            if (P(target)) {
                P(newG)=P(target);
                outbuf_ = String("copied polygon reference");
            } else {
                newG->addObj(O(target));
                outbuf_ = String("referencing geo object");
            }
        }
    } else if (args.size() == 1) {
        Generic * o = objByName(0);
        if (isGeo(o)) {
            Geo * newGeo = new Geo();
            Scene::add(O(newGeo));
            if (P(o)) {
                P(newGeo)=P(o);
                outbuf_ = String("copied polygon reference");
            } else {
                newGeo->addObj(O(o));
                outbuf_ = String("referencing geo object");
            }

        }
    } else outbuf_ = wrong_args(OP_COPY);
}

// help

void Operations::Help(vector<String>args) {
    int i=0;outbuf_.clear();
    for (;i<MAX_OPS;i++) {
        char text[1024];
        sprintf(text, "%s SYNTAX: %s\n", help[i].def, help[i].help);
        String a = String(text);
        outbuf_.append(a); 
    }

}

// select <objname>

void Operations::Select(vector<String>args){
    if (args.size()==1) {
        Generic * o = objByName(0);
        if (o) Scene::setActive(o);
    } else outbuf_ = wrong_args(OP_SELECT);
} 

void Operations::BeginOp(vector<String>args){
    ivMODE=BEGIN_MODE;
    if (args.size()==1) {
        String type = args(0);
        if (match("draw")) {
            drawMODE=NO_INTERP;
        } else if (match("bezier")) {
            drawMODE=BEZIER_INTERP;
        } else if (match("bspline")) {
            drawMODE=BSPLINE_INTERP;
        }
    } else outbuf_ = wrong_args(OP_BEGIN);
} 

void Operations::EndOp(vector<String>args){
    ivMODE=NO_CURR_STATE;
    Scene::curve = (Curve *) NULL;
} 

// register <object> <param> <channel> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]
void Operations::Register(vector<String>args){
    if (args.size() >= 3) {
        Generic * o = objByName(0);
        if (!isObject(o)) {
            outbuf_ = String("not correct type or the object doesn't exist");
            return;
        }
        Channel * c = CH(objByName(2));
        int param = Utils::Param(args[1]);

        float freqscalar = 1;
        if (args.size() >= 4) freqscalar = s2f(3);
        float scalar = 1.0f;
        if (args.size() >= 5) scalar = s2f(4);
        float offset = 0.0f;
        if (args.size() >= 6) offset = s2f(5);

        /* time shift */
        int timeShift = 0;
        if (args.size() >= 7) timeShift = s2i(6); 

        /* start and stop frames */
        unsigned long int start = Scene::start;
        unsigned long int end = Scene::end;
        if (args.size() >= 9) {
            start = (unsigned long int) s2i(7);
            end = (unsigned long int) s2i(8);
        }

        if (c) Scene::register_animation(O(o), param, c, freqscalar, scalar, offset,timeShift,start,end);

    } else outbuf_ = wrong_args(OP_REGISTER);
}

// reference <object> <param> <object> <param> [<freqScalar> <scalar> <offset> <timeShift> <start> <end>]
void Operations::Reference(vector<String>args){
    if (args.size() >= 4) {
        Generic * ref = objByName(0);        
        Generic * targ = objByName(2);        
        if (!ref || !targ) {
            outbuf_ = String("not correct type or the object doesn't exist");
            return;
        }
        int paramRef  = Utils::Param(args[1]);
        int paramTarg = Utils::Param(args[3]);


        int freqscalar = 1;
        if (args.size() >= 5) freqscalar = s2f(4);        
        float scalar = 1.0f;
        if (args.size() >= 6) scalar = s2f(5);
        float offset = 0.0f;
        if (args.size() >= 7) offset = s2f(6);


        /* time shift */
        int timeShift = 0;
        if (args.size() >= 8) timeShift = s2i(7); 

        /* start and stop frames */
        unsigned long int start = Scene::start;
        unsigned long int end = Scene::end;
        if (args.size() >= 10) {
            start = (unsigned long int) s2i(8);
            end = (unsigned long int) s2i(9);
        }

        Scene::register_reference(ref, paramRef, targ, paramTarg, freqscalar, scalar, offset, timeShift, start, end);
        outbuf_= String("registered reference");
    } else outbuf_ = wrong_args(OP_REFERENCE);
}

void Operations::Sweep(vector<String>args) {
    if (args.size() == 1) {
        if (isGeo(selOBJ)) {
            Generic * crossSection = objByName(0);
            if (isGeo(crossSection)) {
                P(selOBJ)->Sweep(G(selOBJ), G(crossSection), false, false);
                //P(selOBJ)->curveMe(G(crossSection));
                G(selOBJ)->bound();
                outbuf_ = String("extruded along Curve");
            } else {
                outbuf_ = String("specified geometry doesn't exist");
            }
        }
    } else if (args.size() == 2) {
        //Generic * crossSection = objByName(0);
        Generic * crossSection = objByName(0);
        if (isGeo(crossSection) && isGeo(selOBJ)){
            String type = args(1);
            if(match("hallway")){
                P(selOBJ)->Sweep(G(selOBJ), G(crossSection), false, true);
                G(selOBJ)->bound();
                outbuf_ = String("extruded wall along Curve");
            } else if(match("onPath")||match("onpath")){
                P(selOBJ)->Sweep(G(selOBJ), G(crossSection), true, false);
                G(selOBJ)->bound();
                outbuf_ = String("extruded along Curve on the path");	
            }
        } else outbuf_ = String("specified geometry doesn't exist");
    } else if (args.size() == 3) {
        Generic * crossSection = objByName(0);
        if (isGeo(crossSection) && isGeo(selOBJ)){
            String type = args(1);
            if(match("hallway")){
                String type = args(2);
                if(match("onPath")||match("onpath")){
                    P(selOBJ)->Sweep(G(selOBJ), G(crossSection), true, true);
                    G(selOBJ)->bound();
                    outbuf_ = String("extruded hallway along Curve on the path");
                } else{
                    outbuf_ = wrong_args(OP_SWEEP);
                }
            } else if(match("onPath")||match("onpath")){
                String type = args(2);
                if(match("hallway")){
                    P(selOBJ)->Sweep(G(selOBJ), G(crossSection), true, false);
                    G(selOBJ)->bound();
                    outbuf_ = String("extruded along Curve");	
                }
            } else{
                outbuf_ = wrong_args(OP_SWEEP);
            }
        } else outbuf_ = String("specified geometry doesn't exist");	
    } else outbuf_ = wrong_args(OP_SWEEP);
}



// path <target> <path> [<dx> <dy> <dz> <freqscalar> <timeShift> <start> <end>]
void Operations::Path(vector<String>args) {
    if (args.size() >= 2) {
        Generic * target = objByName(0);    
        Generic * path   = objByName(1);    
        if (!target || !path) {
            outbuf_ = String("specified objects that do not exist");
            return;
        } 
        Poly * p = NULL;
        if (isGeo(path)) p = P(path);
        else if (isPoly(path)) p = PL(path);
        else { outbuf_ = String("the path object must be or must contain a polygon"); return; }

        /* path offset */
        float dx,dy,dz;
        dx = dy = dz = 0.0f;
        if (args.size() >= 5) {
            dx = s2f(2);
            dy = s2f(3);
            dz = s2f(4);
        }

        int freqscalar = 1;
        if (args.size() >= 6) freqscalar = s2i(5);

        /* time shift */
        int timeShift = 0;
        if (args.size() >= 7) timeShift = s2i(6); 

        /* start and stop frames */
        unsigned long int start = Scene::start;
        unsigned long int end = Scene::end;
        if (args.size() >= 9) {
            start = (unsigned long int) s2i(7);
            end = (unsigned long int) s2i(8);
        }

        Scene::register_reference(GEN(p), TX, target, TX, freqscalar, 1.0f, dx,timeShift,start,end);
        Scene::register_reference(GEN(p), TY, target, TY, freqscalar, 1.0f, dy,timeShift,start,end);
        Scene::register_reference(GEN(p), TZ, target, TZ, freqscalar, 1.0f, dz,timeShift,start,end);
    } else outbuf_ = wrong_args(OP_PATH);
}

// poly <centoid|texture> [<s> <t>]
void Operations::PolyOp(vector<String>args) {  
    if (args.size() >= 1) {
        Poly * p = NULL;
        if (isPoly(selOBJ)) p = PL(selOBJ);
        else if (isGeo(selOBJ) && P(selOBJ)) p = P(selOBJ);
        else {
            outbuf_ = String(NO_GEO_SELECTED);
            return;
        }

        String type = args[0];
        if (match("centroid")) {
            p->centroid();
        } else if (match("xmin")) {
            p->xmin();
        } else if (match("xmax")) {
            p->xmax();
        } else if (match("ymin")) {
            p->ymin();
        } else if (match("ymax")) {
            p->ymax();
        } else if (match("zmin")) {
            p->zmin();
        } else if (match("zmax")) {
            p->zmax();
        } else if (match("st")) {
            float s = 1.0f;
            float t = 1.0f;
            if (args.size() == 2) s = t = s2f(1);
            else if (args.size() == 3) {
                s = s2f(1);
                t = s2f(2);
            }
            p->multiplyTextureCoords(s, t);
        } else if (match("texture")) {
            float s = 1.0f;
            float t = 1.0f;
            if (args.size() == 2) s = t = s2f(1);
            else if (args.size() == 3) {
                s = s2f(1);
                t = s2f(2);
            }
            p->basicTextureCoords(s, t);
        } else if (match("noise")) {
            if(args.size() == 1){
                p->noise(1.0, 1.0, 1.0, 1.0);
            } else if(args.size() == 2){
                float s1 = s2f(1);
                p->noise(s1, 1.0, 1.0, 1.0);
            } else if(args.size() == 3){
                float s1 = s2f(1);
                float s2 = s2f(1);
                p->noise(s1, s2, 1.0, 1.0);
            } else if(args.size() == 4){
                float s1 = s2f(1);
                float s2 = s2f(2);
                float s3 = s2f(3);
                p->noise(s1, s2, s3, 1.0);
            } else if(args.size() == 5){
                float s1 = s2f(1);
                float s2 = s2f(2);
                float s3 = s2f(3);
                float s4 = s2f(4);
                p->noise(s1, s2, s3, s4);
            }
        } else if (match("cut")) {
            if (args.size() == 3) {
                p->cut(s2i(1), s2f(2));
            }
        } else if (match("close")) {
            p->close();
        } else if (match("meld")) {
            p->indexMeld();
        } else if (match("scale")) {
            float x,y,z;
            x=y=z=1.0f;
            if (args.size()==2) {
                x=y=z=s2f(1);
            } else if (args.size()==4) {
                x = s2f(1);
                y = s2f(2);
                z = s2f(3);
            }
            p->scale(x,y,z);
        } else if (match("translate")) {
            if (args.size() == 4) {
                p->transform(s2f(1),s2f(2),s2f(3)); 
            }
        } else if (match("rotate")) {
            if (args.size() == 5) {
                p->rotate(s2f(1),s2f(2),s2f(3),s2f(4)); 
            }
        } else if (match("group")) {
            if (args.size() == 5) {
                String type = args[1];
                if (match("texture")) {
                    // args[4] is some.ppm
                    p->textureGroup(s2i(2),s2i(3),args[4]); 
                    p->freeList();
                    outbuf_ = String("texture applied");
                } else if (match("material")) {
                    // args[4] is some.mtl
                    p->materialGroup(s2i(2),s2i(3),args[4]); 
                }
            }
        }

    } else outbuf_ = wrong_args(OP_POLY);
}

void Operations::cloth(vector<String>args){
    Geo * cloth = new Geo();
    cloth->p = new Cloth();
    if (args.size()>0) {
        ((Cloth*) cloth->p ) ->updateDistribution(s2f(0));
    } else{
        ((Cloth*) cloth->p )->updateDistribution(0.0);	
    }
    animation_t a = animation_t();
    ((Cloth*)cloth->p)->lastTime = Scene::time*((Cloth*)cloth->p)->timeScalar;
    a.object = cloth;
    a.param = UPDATE;
    a.start = 0.0;
    a.end = 999999999;
    Scene::animations.push_back(a);
    Scene::addGeo(cloth);	
}

void Operations::tree(vector<String>args){
    /*
       if(args.size()>=2){
       Geo * tree = new Geo();
       Generic* trunk = objByName(0);
       Generic* leaf = objByName(1);
       if(args.size()==2&&isGeo(trunk)&&isGeo(leaf)){
       tree->makeTree(G(trunk), G(leaf), 1.0, 1.0);
       } else {
       if(args.size()==4&&isGeo(trunk)&&isGeo(leaf)){
       float trunkheight = s2f(2);
       float leafheight = s2f(3);
       tree->makeTree(G(trunk), G(leaf), trunkheight, leafheight);
       }
       }
       }
     */
}

// raytrace [<filename> <bounces> <width> <height>]
void Operations::RayTrace(vector<String>args) {
    printf("raytracing...\n"); 
    if (args.size() >= 1) {
        if (args.size() == 1) Render::RayTrace(args[0]);
        else if (args.size() == 2) {
            Render::bounces = s2i(1);
            Render::RayTrace(args[0]);
        } else if (args.size() == 4) {
            Render::bounces = s2i(1);
            Render::w = s2i(2);
            Render::h = s2i(3);
            Render::RayTrace(args[0]);
        }
    } else {
        String file("capture.ppm");
        Render::RayTrace(file);
    }
}

void Operations::Render(vector<String>args) {
    if (args.size() >= 1) {
        if (args.size() == 1) Render::CurrentScreen(args[0], 0,0,Scene::w,Scene::h);
        else if (args.size() >= 2) {
            String type = args[0];
            if (match("set") || match("display")) {
                bool render = (bool) s2i(1);
                if (render) Scene::curSTATE|=RENDERING;
                else Scene::curSTATE&=~RENDERING;
            } else if (match("raytrace")) {
                bool render = (bool) s2i(1);
                if (render) Scene::curSTATE|=RAYTRACING;
                else Scene::curSTATE&=~RAYTRACING;
                if (render) Scene::time = Render::start;
            } else if (match("frames")) {
                if (args.size() == 3) {
                    Render::start = s2i(1);
                    Render::end = s2i(2);
                    outbuf_ = String("frame range set");
                }
            } else if (match("size")) {
                if (args.size() == 3) {
                    Render::w = s2i(1);
                    Render::h = s2i(2);
                    outbuf_ = String("size set");
                }
            } else if (match("bounces")) {
                Render::bounces = s2i(1);
                outbuf_ = String("bounces set");
            } else if (match("jitter")) {
                Render::jitter = (bool) s2i(1);
                outbuf_ = String("jitter set");
            } else if (match("samples")) {
                Render::samples = s2i(1);
                outbuf_ = String("samples set");
            } else if (match("depthmap")) {
                Render::depth_map = (bool) s2i(1);
                outbuf_ = String("depthmap set");
            } else if (match("cartoon")) {
                Render::cartoon = (bool) s2i(1);
                outbuf_ = String("cartoon set");
            }
        }
    } else {
        String file("capture.ppm");
        Render::CurrentScreen(file, 0,0,Scene::w,Scene::h);
    }
}

void Operations::Timeline(vector<String>args) {
    if (args.size() >= 1) {
        String type = args[0];
        if (match("play")) {
            Scene::curSTATE |= PLAY; 
        } else if (match("stop")) {
            Scene::curSTATE &= ~PLAY;
        }
        if (args.size() == 2) {
            if (match("set")) {
                // set the time to the current frame
                Scene::time = (long unsigned int) s2i(1);    
            }
        }
    } else outbuf_ = wrong_args(OP_TIMELINE);
    printf("time: %lu\n", Scene::time);
}

void Operations::Display(vector<String>args) {
    if (args.size() >= 1) {
        String type = args[0];
        if      (match("none"))         curSTATE = DISPLAY_STATE;
        else if (match("default"))      curSTATE = DEFAULT_STATE;    
        else if (match("wireframe"))    Func::Toggle_WIREFRAME();
        else if (match("axis"))         Func::Toggle_SHOW_AXIS();
        else if (match("ortho"))        Func::Toggle_ORTHO_GRID();
        else if (match("texture"))      Func::Toggle_TEXTURE_ON();
        else if (match("lights"))       Func::Toggle_LIGHTS_ON();
        else if (match("displaylists")) Func::Toggle_DISPLAY_LIST();
        else if (match("select"))       Func::Toggle_SHOW_SELECT();
    } else if (args.size()==2) {
        String type = args[0];
        if      (match("gridsize"))      Scene::gridSize = s2i(1);
        else if (match("gridstep"))      Scene::gridStep = s2i(1);    
    }
}

void Operations::Convert(vector<String>args) {
    if (args.size() == 2) {
        Generic * obj = objByName(0);
        String type = args[1];
        if (match("channel")) {
            if (isGeo(obj) && P(obj)) {

                Channel * chanX = new Channel();
                chanX->name = Scene::name("tx");
                Scene::add(GEN(chanX));
                Channel * chanY = new Channel();
                Scene::add(GEN(chanY));
                chanY->name = Scene::name("ty");
                Channel * chanZ = new Channel();
                Scene::add(GEN(chanZ));
                chanZ->name = Scene::name("tz");

                for(unsigned int i=0; i<V(obj).size();i++) 
                    chanX->points.push_back(Point(float(i),V(obj)[i].x,0.0f));
                for(unsigned int i=0; i<V(obj).size();i++) 
                    chanY->points.push_back(Point(float(i),V(obj)[i].y,0.0f));
                for(unsigned int i=0; i<V(obj).size();i++) 
                    chanZ->points.push_back(Point(float(i),V(obj)[i].z,0.0f));

                chanX->setBounds();
                chanY->setBounds();
                chanZ->setBounds();

                outbuf_ = String("added channels");
            }
        }
    }
}

void Operations::Eval(vector<String>args) {
    vector<String>operators;
    vector<String>operands;
    for(unsigned int i=0;i<args.size(); i++) {
        String type = args[i];
        if (match("sin")) {
            printf("sine!\n");
        } else if (match("cos\n")) {
            printf("cosine!");
        }

    }
}

void Operations::server(vector<String>args) {
    if (args.size() > 0) {
        String type = args[0];
        if (match("start")) {
            if (args.size() >= 2) {
                if (!Scene::server) Scene::server = new Server();
                Scene::server->start_server(s2i(1));
                outbuf_ = String("server started");
            } else {
                if (!Scene::server) Scene::server = new Server();
                Scene::server->start_server(5000);
                outbuf_ = String("server started");
            } 
        } else if (match("stop")) {
            if (Scene::server) delete Scene::server;
            Scene::server = NULL;
        }
    }
}

void Operations::connect(vector<String>args) {
    if (args.size() > 0) {
        if (args.size() >= 2) {
            if (!Scene::client) Scene::client = new Client();
            Scene::client->start_client(args[0], s2i(1));
            outbuf_ = String("connection");
        }
    } else {
        if (!Scene::client) Scene::client = new Client();
        Scene::client->start_client(String("127.0.0.1"), 5000);
        outbuf_ = String("connection");
    }
}

void Operations::send(vector<String>args) { 

    String msg = String();
    for(unsigned int i=0; i<args.size();i++) {
        msg.append(args[i]);
        msg.append(String(" "));
    }

    if (Scene::client) {
        Scene::client->send_msg(msg);
    }
    outbuf_ = String("sent " + msg);

}

void Operations::testing(vector<String>args) { }
Operations::~Operations() {}

