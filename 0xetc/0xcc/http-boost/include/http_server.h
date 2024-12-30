#ifndef _CC_HTTP_SERVER_
#define _CC_HTTP_SERVER_

#include "http_server.h"

class Router {

public:

    Router(){

        state = 0;

    }


    void PrintState();

private:

    int state;


};


#endif 