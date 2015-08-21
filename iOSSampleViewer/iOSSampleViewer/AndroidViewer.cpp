//
//  AndroidViewer.cpp
//  HellowWorld2
//
//  Created by Odie Edo-Osagie on 14/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include "AndroidViewer.h"


AndroidViewer::AndroidViewer()
{
    ipAddr = "";
    portNo = 0;
    
    client = NULL;
    callback = NULL;
}

AndroidViewer::AndroidViewer(char* ip, int port, AndroidViewerCallbackInterface *callback)
{
    AndroidViewer::ipAddr = ip;
    AndroidViewer::portNo = port;
    AndroidViewer::callback = callback;
    
    AndroidViewer::client = new TCPClient();
}

AndroidViewer::~AndroidViewer()
{
    delete client;
}

bool AndroidViewer::connect()
{
    if(client != NULL){
        return client->connect(ipAddr, portNo);
    }
    else{
        return false;
    }
}

void AndroidViewer::startViewing()
{
    isViewing = true;
    while(isViewing == true){
        // Send initial frame buffer request
        client->sendFrameBufferRequest();
        sleep(0.2);
        
        bool result = client->receiveData();
        while(result == false){
            result = client->receiveData();
        }
        
        callback->newFrameAvailable(client->frameBuffer, client->lastMessageSize);
        //callback->debug();
        sleep(0.2);
    }
}



