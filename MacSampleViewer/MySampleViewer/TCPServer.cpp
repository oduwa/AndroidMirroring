//
//  TCPServer.cpp
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 10/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include "TCPServer.h"
#include "CommonFuncs.h"

const char TCPServer::FRAME_BUFFER_REQUEST_MESSAGE[] = "wantFrame";
const char TCPServer::STOP_LISTENING_MESSAGE[] = "plsstop";


void TCPServer::listenThreadFunction()
{
    int n = 0;
    while(isListening){
        bool hasMsg = readMessageIntoBuffer();
        
        if(hasMsg == true){
            printf("RECEIVED MSG FROM CLIENT: %s\n", messageBuffer);
            
            if(strcmp(messageBuffer, TCPServer::FRAME_BUFFER_REQUEST_MESSAGE) == 0){
                sendData(data, 251152);
            }
            else if(strcmp(messageBuffer, TCPServer::STOP_LISTENING_MESSAGE) == 0){
                n = write(newsockfd, "Stopping..", 10);
                if(n <= 0){
                    CommonFuncs::error("ERROR writing to socket");
                }
                stopListening();
            }
            else{
                n = write(newsockfd, "gotcha", 6);
                if(n <= 0){
                    CommonFuncs::error("ERROR writing to socket");
                }
            }
        }
    }
}


bool TCPServer::readMessageIntoBuffer()
{
    int n = 0;
    bzero(messageBuffer,256);
    n = read(newsockfd,messageBuffer,255);
    printf("READ: %d\n", n);
    if (n < 0){
        //CommonFuncs::error("ERROR reading from socket");
        return false;
    }
    if(n > 0){
        printf("Here is the message: %s",messageBuffer);
        return true;
    }
    
    return false;
}

TCPServer::TCPServer()
{
    data = new signed char[251152];
    for(int i = 0; i < 251152; i++){
        data[i] = 25;
    }
}

TCPServer::~TCPServer()
{
    listenThread->detach();
    delete listenThread;
}

bool TCPServer::connect(int port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        CommonFuncs::error("ERROR opening socket");
        return false;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); // converts number to big endian (network byte order) if on little endian host
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        CommonFuncs::error("ERROR on binding");
        return false;
    }
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        CommonFuncs::error("ERROR on accept");
        return false;
    }
    
    
    return true;
}


void TCPServer::startListening()
{
    isListening = true;
    int n;
    
    listenThread = new std::thread(&TCPServer::listenThreadFunction, this);
    listenThread->join();
    listenThread->detach();
    
    //listenThreadFunction();
    
//    while(true){
//        printf("pikachu");
//        sleep(5);
//    }
}

void TCPServer::stopListening()
{
    isListening = false;
}

void TCPServer::sendData(signed char* data, int len)
{
    // Write length of data
    char lenStr[10];
    sprintf(lenStr, "%d", len);
    int n = write(newsockfd,lenStr,10);
    
    
    // Write data
    n = write(newsockfd,data,len);
    if (n < 0){
        CommonFuncs::error("ERROR writing to socket");
    }
    else{
        printf("WROTE TO SOCKET");
    }
}
