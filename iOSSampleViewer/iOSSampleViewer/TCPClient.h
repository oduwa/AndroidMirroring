//
//  TCPClient.h
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 13/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __HelloWorld__TCPClient__
#define __HelloWorld__TCPClient__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include "CommonFuncs.h"
#include <thread>

class TCPClient {
    
private:
    static const char FRAME_BUFFER_REQUEST_MESSAGE[];
    static const char STOP_LISTENING_MESSAGE[];
    
    
public:
    static int FRAME_BUFFER_SIZE;
    static int MESSAGE_BUFFER_SIZE;
    
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char messageBuffer[256];
    signed char frameBuffer[20000000];
    
    TCPClient();
    bool connect(char *ipAddr, int port);
    void sendMessage();
    bool receiveData();
    char* receiveMessage();
    char* receiveMessageDebug();
    bool receiveShortTextMessage();
    char* receiveLongTextMessage();
    void sendTextMessage(const char* msg);
    void sendFrameBufferRequest();
    int lastMessageSize;
    
};

#endif /* defined(__HelloWorld__TCPClient__) */
