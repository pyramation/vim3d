#include "Directory.h"

Directory::Directory(){}

void Directory::makeDir(String dir) {
    if (dirExists(dir.c_str())) {
        return;
    }
    mkdir(dir.c_str(),0700);
}

bool Directory::dirExists(String dir) {
    DIR*dp = opendir(dir.c_str());
    if (dp == NULL)     
        return false;
    closedir(dp);
    return true;    
}

/*function... might want it in some class?*/
void Directory::getdir (String dir, vector<String> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error opening " << dir << std::endl;
        return;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(String(dirp->d_name));
    }
    closedir(dp);
}

void Directory::List() {
    String dir = String(".");
    vector<String> files = vector<String>();

    getdir(dir,files);

    for (unsigned int i = 0;i < files.size();i++) {
        std::cout << files[i] << std::endl;
    }
}

Directory::~Directory(){}
