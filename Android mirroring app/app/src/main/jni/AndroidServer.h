//
//  AndroidServer.h
//  HelloWorld
//
//  Created by Odie Edo-Osagie on 16/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __HelloWorld__AndroidServer__
#define __HelloWorld__AndroidServer__

#include <stdio.h>
#include "ISocketServer.h"

class AndroidServer {
    
private:
    
    
public:
    ISocketServer *socketServer;
    int portNumber;
    
    AndroidServer();
    AndroidServer(int port);
    bool connect();
    ~AndroidServer();
    
    
};

#endif /* defined(__HelloWorld__AndroidServer__) */
