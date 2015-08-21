//
//  AndroidViewer.h
//  HellowWorld2
//
//  Created by Odie Edo-Osagie on 14/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __HellowWorld2__AndroidViewer__
#define __HellowWorld2__AndroidViewer__

#include <stdio.h>
#include "TCPClient.h"

class AndroidViewer {

    
private:
    
    
    
public:
    class AndroidViewerCallbackInterface{
    public:
        virtual void newFrameAvailable(signed char* data, int size) = 0;
        virtual void debug() = 0;
    };
    
    TCPClient *client;
    AndroidViewerCallbackInterface *callback;
    char* ipAddr;
    int portNo;
    bool isViewing;
    signed char *frameBuffer;
    
    AndroidViewer();
    AndroidViewer(char* ip, int port, AndroidViewerCallbackInterface* callback);
    ~AndroidViewer();
    
    bool connect();
    void startViewing();
    
};

#endif /* defined(__HellowWorld2__AndroidViewer__) */
