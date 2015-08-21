//
//  TCPServer.h
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 10/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#ifndef __HelloWorld__TCPServer__
#define __HelloWorld__TCPServer__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <thread>

class TCPServer {
    
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
    std::thread *listenThread;
    signed char* data;
    
    TCPServer();
    ~TCPServer();
    bool connect(int port);
    void sendData(signed char* data, int len);
    void startListening();
    void stopListening();
    
    
};

#endif /* defined(__HelloWorld__TCPServer__) */
