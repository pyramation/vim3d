#ifndef OBJECT_H
#define OBJECT_H

#include "Transform.h"

class Object;
typedef vector <Object *> obj_t;

class Object : public Transform {
 public:

    String printInfo();
    virtual String propertyInfo();

    Object * pathObject;
    bool child;    


    Object * parent;
    obj_t objs;
    void addObj(Object * obj);

    String historyInfo();
    vector <String> history;
    vector <String> redo;
    unsigned int history_index;

    void Undo();
    void Redo();
    void recook();
    virtual void display();
    int displayPass;
    virtual void display(int pass);

    Object();
    ~Object();

};


#endif
