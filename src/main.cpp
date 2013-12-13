#include "Scene.h"

int main(int argc, char* argv[]) {

    Scene *scene = new Scene();
    scene->go(argc, argv);
    delete scene;
    return 0;
}
