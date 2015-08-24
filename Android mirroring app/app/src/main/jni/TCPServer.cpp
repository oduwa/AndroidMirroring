//
//  TCPServer.cpp
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 10/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include "TCPServer.h"
#include "CommonFuncs.h"
#include "bZip2/bzlib.h"
#include <android/log.h>

TCPServer::TCPServer()
{
    
}

bool TCPServer::connect(int port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR opening socket");
        CommonFuncs::error("ERROR opening socket");
        return false;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); // converts number to big endian (network byte order) if on little endian host
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR on binding");
        CommonFuncs::error("ERROR on binding");
        return false;
    }
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR on accept");
        CommonFuncs::error("ERROR on accept");
        return false;
    }
    
    
    return true;
}

void TCPServer::startListening()
{
    isListening = true;
    int n;
    
    while(isListening){
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0){
            __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR reading from socket");
            CommonFuncs::error("ERROR reading from socket");
        }
        if(n > 0){
            printf("Here is the message: %s",buffer);
            __android_log_print(ANDROID_LOG_DEBUG, "JNIDummy", "Here is the message: %s",buffer);
        }
    }
}

void TCPServer::sendData(signed char* data, int len)
{
    unsigned int initialSize = len;
    unsigned int resultingSize = len*0.25;
    char compressedData[resultingSize]; bzero(compressedData, resultingSize);
    BZ2_bzBuffToBuffCompress(compressedData, &resultingSize, (char *)data, initialSize, 5, 0, 0);
    __android_log_print(ANDROID_LOG_ERROR, "JNIDummy", "COMPRESSED SIZE: %d", resultingSize);

    //int n = write(newsockfd,data,len);
    char lenStr[10];
    sprintf(lenStr, "%d", resultingSize);
    int n = write(newsockfd,lenStr,10);


    n = write(newsockfd,compressedData,resultingSize);
    if (n < 0){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR writing to socket");
        //CommonFuncs::error("ERROR writing to socket");
    }
    else {
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "SERVER WROTE TO SOCKET");
    }
}

void TCPServer::sendTextData(const char* data, int len)
{
    char lenStr[10];
    sprintf(lenStr, "%d", len);
    int n = write(newsockfd,lenStr,10);


    n = write(newsockfd,data,len);
    if (n < 0){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "ERROR writing to socket");
        CommonFuncs::error("ERROR writing to socket");
    }
    else{
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "SERVER WROTE TO SOCKET");
    }

    close(newsockfd);
}
