#include "Object.h"
#include "Scene.h"
#include "Term.h"

Object::Object() {
    type = OBJECT_TYPE;
    active = false;
    filepath = String();
    name = String();
    history_index = 0;
    vector <String> history=vector<String>();
    vector <String> redo=vector<String>();
    id = Scene::newID();
    objs = obj_t();
    parent=NULL;
    child = false;
}

String Object::propertyInfo() {
    char s[8];
    sprintf(s, "%s\n", (char *) "object");
    return String(s);
}

String Object::printInfo() {
    char s[8];
    sprintf(s, "%s\n", (char *) "object");
    return String(s);
}

String Object::historyInfo() {
    String n = String("Command History:\n");
    if (history.size() > 0) {
        vector<String>::iterator cursor = history.begin();
        for(; cursor != history.end(); cursor++) {
            String a = *cursor;
            n.append( a + "\n" );
        }
    }
    return n;
}


void Object::addObj(Object * obj) {
    objs.push_back(obj);
}

void Object::display() {}
void Object::display(int pass) {}

void Object::Undo() {
    if (history.size() > 0) {
        String a = *(history.end()-1);
        history.erase(history.end()-1);
        redo.push_back(a);
    }
}

void Object::Redo() {
    if (redo.size() > 0) {
        String a = *(redo.end()-1);
        redo.erase(redo.end()-1);
        history.push_back(a);
    }
}

void Object::recook() {
    this->loadIdentity();
    Scene::setActive(O(this));
    for(unsigned int i=0; i < history.size(); i++) {
        //if (i > presentState) break;
        Term::parse(history[i], true);
    }
}

Object::~Object(){}
