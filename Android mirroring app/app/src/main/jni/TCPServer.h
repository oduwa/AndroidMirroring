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

class TCPServer {
    
    
public:
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    bool isListening;
    
    TCPServer();
    bool connect(int port);
    void startListening();
    void sendData(signed char* data, int len);
    void sendTextData(const char* data, int len);
    
};

#endif /* defined(__HelloWorld__TCPServer__) */
