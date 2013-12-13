#include "Term.h"
#include "Func.h"
#include "Primitives.h"
#include "Scene.h"
#include "Operations.h"

#include <stdlib.h>
#include <algorithm>

#define toks(x) (atof((*(tokens.begin()+x)).c_str()))

// FUNCTION POINTERS

typedef void (*function_handle) (vector<String>tokens);
typedef struct {
    const char * name;
    function_handle fn;
} cmd_t;

void nilly (vector<String>tokens){}

cmd_t cmds[] = {
    {"save",        Operations::save},
    {"quit",        Operations::quit},
    {"q",           Operations::quit},
    {"file",        Operations::readOBJ},
    {"open",        Operations::readOBJ},
    {"o",           Operations::readOBJ},
    {"write",       Operations::write},
    {"znear",       Operations::zNear},
    {"zfar",        Operations::zFar},
    {"fovy",        Operations::fovy},
    {"texture",     Operations::texture},
    {"draw",        Operations::draw},
    {"bezier2",     Operations::Bezier2},
    {"bezier",      Operations::Bezier},
    {"bspline",     Operations::Bspline},
    {"extrude",     Operations::Extrude},
    {"revolve",     Operations::Revolve},
    {"compute",     Operations::compute},
    {"bounds",      Operations::bounds},
    {"diffuse",     Operations::diffuse},
    {"specular",    Operations::specular},
    {"ambient",     Operations::ambient},
    {"shininess",   Operations::shininess},
    {"rotate",      Operations::rotate},
    {"translate",   Operations::translate},
    {"m",           Operations::translate},
    {"mv",          Operations::translate},
    {"scale",       Operations::scale},
    {"add",         Operations::add},
    {"load",        Operations::load},
    {"delete",      Operations::Delete},
    {"Vexplore",    Operations::Vexplore},
    {"Sexplore",    Operations::Sexplore},
    {"matrix",      Operations::Matrix},
    {"position",    Operations::Position},
    {"up",          Operations::Up},
    {"lookat",      Operations::Lookat},
    {"name",        Operations::Name},
    {"clear",       Operations::Clear},
    {"pinfo",       Operations::Properties},
    {"minfo",       Operations::MatrixInfo},
    {"ls",          Operations::List},
    {"list",        Operations::List},
    {"view",        Operations::View},
    {"channel",     Operations::ChannelOp},
    {"chan",        Operations::ChannelOp},
    {"c",           Operations::ChannelOp},
    {"history",     Operations::History},
    {"hinfo",       Operations::History},
    {"light",       Operations::LightOp},
    {"l",           Operations::LightOp},
    {"geo",         Operations::GeoOp},
    {"copy",        Operations::Copy},
    {"cp",          Operations::Copy},
    {"h",           Operations::Help},
    {"help",        Operations::Help},
    {"s",           Operations::Select},
    {"select",      Operations::Select},
    {"cam",         Operations::CameraOp},
    {"camera",      Operations::CameraOp},
    {"object",      Operations::ObjectOp},
    {"obj",         Operations::ObjectOp},
    {"begin",       Operations::BeginOp},
    {"end",         Operations::EndOp},
    {"reg",         Operations::Register},
    {"register",    Operations::Register},
    {"ref",         Operations::Reference},
    {"reference",   Operations::Reference},
    {"sweep",       Operations::Sweep},
    {"path",        Operations::Path},
    {"poly",        Operations::PolyOp},
    {"render",      Operations::Render},
    {"timeline",    Operations::Timeline},
    {"time",        Operations::Timeline},
    {"disp",        Operations::Display},
    {"display",     Operations::Display},
	{"clearall",	Operations::clearall},
    {"children",    Operations::Children},
	{"tree",		Operations::tree},
    {"raytrace",    Operations::RayTrace},
    {"convert",     Operations::Convert},
    {"eval",        Operations::Eval},
    {"refl",        Operations::reflection},
    {"reflection",  Operations::reflection},
	{"testing",		Operations::testing},
	{"emission",		Operations::emission},
	{"refraction",		Operations::refraction},
	{"normalmap",		Operations::normalMap},
	{"invertNormals",  Operations::invertNormals},
	{"cloth",  Operations::cloth},
	{"server",  Operations::server},
	{"connect",  Operations::connect},
	{"send",  Operations::send},
};

function_handle lookup_cmd(String cmdName) {
    for(unsigned int i=0; i<sizeof(cmds)/sizeof(cmd_t);i++) {
        if (!strcmp(cmdName.c_str(), cmds[i].name))
            return cmds[i].fn;
    } 
    return nilly;
}

// VARIABLES

String Term::buffer;
vector<String> Term::lines;
unsigned int Term::hist_index;


// METHODS

void Term::commandLine() {
    Term::buffer.push_back(':');
    ivMODE|=COMMAND_MODE;
}

int modulo(int a, int m) {
    if (m==0) return 0;
    if ((a%m)<0) return a%m+m;
    return a%m;
}

void Term::goBack() {
    if (lines.size() > 0) {
        hist_index = modulo((hist_index-1),lines.size());
        buffer.clear();
        buffer = lines[hist_index];
    }
}

void Term::goForward() {
    if (lines.size() > 0) {
        hist_index = modulo((hist_index+1),lines.size());
        buffer.clear();
        buffer = lines[hist_index];
    }
}

void Term::parse(String& str, bool hist) {


    vector<String> tokens;
    Tokenize(str, tokens, " :\n");
    if (tokens.size() > 0) {
        vector<String>::iterator cursor;
        String command = *(tokens.begin());

        // store history
        if (!hist) lines.push_back(str);


        // catch begin clauses (stricly for files)
        if (!command.compare("end")) {
            ivMODE&=~BEGIN_MODE;
            if (selOBJ) G(selOBJ)->recook();
        }
        if (ivMODE & BEGIN_MODE) {
            if (!Scene::curve) Scene::addCurve();
            if (tokens.size() == 3) {
                Scene::curve->addPoint(Point(toks(0), toks(1), toks(2)));
            } else if (tokens.size() == 2 ) {
                Scene::curve->addPoint(Point(toks(0), 0.0, toks(1)));
            } else if (tokens.size() == 1 ) {
                Scene::curve->addPoint(Point(Scene::curve->points.size(), toks(0), 0.0));
            }
        } else {

            /* only add history if this isn't a history call */
            if (!hist && isObject(selOBJ)) {
                const char * t = str.c_str();
                while( *t == ':') t++;
                str = String(t);
                bool add = Operations::add(command);
                if (add && str.size() > 0) O(selOBJ)->history.push_back(str);
            }

            function_handle func = lookup_cmd(command);
            tokens.erase(tokens.begin());
            func(tokens);
        }
    }
}

void Term::Tokenize(const String& str,
        vector<String>& tokens,
        const String& delimiters) {
    // Skip delimiters at beginning.
    String::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    String::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (String::npos != pos || String::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

int Term::match(char *s, char *t) {
    for( ; *s == *t; s++, t++) if (*s == '\0') return 1;
    return 0;
}
int Term::is_lower(char * s) {return ( 'a' <= *s && *s <= 'z');}
int Term::is_upper(char * s) {return ( 'A' <= *s && *s <= 'Z');}
int Term::nocase_match(char *s, char *t) {
    if (is_lower(s) && is_lower(t) && *s == *t) return 1;
    if (is_lower(s) && is_upper(t) && *s == *t + 32) return 1;
    if (is_upper(s) && is_lower(t) && *s + 32 == *t) return 1;
    if (is_upper(s) && is_upper(t) && *s ==*t) return 1;
    return 0;
}
int Term::cmd_match(char *s, char *t) {
    while (*s == ' ') s++;
    while (*t == ' ') t++;
    for ( ; nocase_match(s,t); s++, t++) {
        if ((*s == ' ' || *s == '\0' || *s == '\n') && *t == '\0') return 1;
    }
    if ((*s == ' ' || *s == '\0' || *s == '\n') && *t == '\0') return 1;
    return 0;
}

char * Term::prompt() {
    int i = 0;
    int len = 10; 
    char * s;
    char c;
    s = (char *) malloc(sizeof(char) * len);
    printf(">");
    while((c=getchar()) != '\n') {
        if (c == EOF) break;
        if (i >= (len - 1)) {
            len = len * 2;
            s = (char *) realloc(s, sizeof(char) * len);
        }
        s[i++] = c;
    }
    s[i] = '\0';
    return s;
}

Term::Term() {
    lines = vector<String>();
    buffer = String();
    hist_index=0;
}

Term::~Term() {}
