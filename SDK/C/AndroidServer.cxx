//
//  AndroidServer.cpp
//  HelloWorld
//
//  Created by Odie Edo-Osagie on 16/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include <mob/AndroidServer.h>
#include <android/log.h>


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
    return success;
}

void AndroidServer::startCommunications()
{
    socketServer->startListening();
}

void AndroidServer::stopCommunications()
{
    socketServer->stopListening();
}


