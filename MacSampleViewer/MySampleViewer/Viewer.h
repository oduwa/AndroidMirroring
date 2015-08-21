//
//  Viewer.h
//  MySampleViewer
//
//  Created by Oduwa Edo Osagie on 14/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __MySampleViewer__Viewer__
#define __MySampleViewer__Viewer__

#include <stdio.h>
#include "TCPClient.h"

class Viewer {
    
public:
    
    class FrameBufferUpdateCallback {
    public:
        virtual void newFrameAvailable(char* data) = 0;
    };
    TCPClient *client;
    
    Viewer();
    ~Viewer();
    bool connect(const char* ipAddr, int port);
    void sendFrameBufferRequest();
    
    
};

#endif /* defined(__MySampleViewer__Viewer__) */
