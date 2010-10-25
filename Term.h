#ifndef TERM_H
#define TERM_H

#include "Packages.h"
#include <stdio.h>

class Term {
public:

    static String buffer;
    static vector <String> lines;
    static unsigned int hist_index;
    static void goBack();
    static void goForward();

    static int match(char *s, char *t);
    static int is_lower(char *s);
    static int is_upper(char *s);
    static int nocase_match(char *s, char *t);
    static int cmd_match(char *s, char *t);
    static char * prompt();
    static void commandLine(); 
    
    // Tokenizer:
    // http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
    static void Tokenize(
                        const String& str,
                        vector<String>& tokens,
                        const String& delimiters);

    static void parse(String& str, bool hist);
    
   // static int openSocket(char *port);
   // static int sendMessage(char * msg);
    

    Term();
	virtual ~Term();

};

#endif

