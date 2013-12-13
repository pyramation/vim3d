#include "Server.h"

void *loop_wrapper(void * server) { ((Server*)server)->main_loop(); return NULL;}
