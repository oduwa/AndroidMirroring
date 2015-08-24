//
//  AndroidServer.cpp
//  HelloWorld
//
//  Created by Odie Edo-Osagie on 16/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include "AndroidServer.h"


AndroidServer::AndroidServer()
{
    socketServer=  NULL;
    portNumber = 0;
}

AndroidServer::~AndroidServer()
{
    delete socketServer;
}

AndroidServer::AndroidServer(int port)
{
    portNumber = port;
    socketServer = new ISocketServer();
}

bool AndroidServer::connect()
{
    bool success = socketServer->connect(portNumber);
    
    // Start receiving messages from connected client
    if(success == true){
        socketServer->startListening();
    }
    
    return success;
}

