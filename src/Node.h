#ifndef NODE_H
#define NODE_H

#include "Packages.h"
#include "Elements.h"

#include <stdio.h>
#include <math.h>

class Node {
    public:

/*

the idea is to store a list of transformations
so that Geo objects have lists of nodes that alter there geo

            @opcode : specifies the type of transformation, or operation
            @arg1   : first arguent
            @arg2   : second argument
            
            @T      : associated transformation matrices, 
            @R      : whose values are calculated using
            @S      : the opcode and arguments

*/


        int opcode;
        int arg1;
        int arg2;

        mat4 T;
        mat4 R;
        mat4 S;

        Node();
        virtual ~Node();

};

typedef vector <Node> node_t;

#endif

