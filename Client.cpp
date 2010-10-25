#include "Client.h"

void *client_loop_wrapper(void * client) { ((Client*)client)->main_loop(); return NULL;}
