//
//  ISocketServer.h
//  HelloWorld
//
//  Created by Odie Edo-Osagie on 16/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __HelloWorld__ISocketServer__
#define __HelloWorld__ISocketServer__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
//#include <thread>

class ISocketServer {
    
private:
    static const char FRAME_BUFFER_REQUEST_MESSAGE[];
    static const char STOP_LISTENING_MESSAGE[];
    bool readMessageIntoBuffer();
    void listenThreadFunction();
    
    
public:
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char messageBuffer[256];
    char frameBuffer[20000000];
    struct sockaddr_in serv_addr, cli_addr;
    bool isListening;
    //std::thread *listenThread;
    signed char* data;
    int width, height;
    int frameSize;
    bool isFrameDataBeingUsed;
    bool safeToSend;
    
    ISocketServer();
    ~ISocketServer();
    bool connect(int port);
    void sendData(signed char* data, int len);
    void startListening();
    void stopListening();
    
    
};

#endif /* defined(__HelloWorld__ISocketServer__) */
