#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h> // mkdir()
#include <errno.h>
#include <iostream>
#include "Packages.h"

class Directory {
 public:
    static void List();
    static bool dirExists(String dir);
    static void getdir (String dir, vector<String> &files);
    static void makeDir(String dir);
    Directory();
    virtual ~Directory();

};


#endif
