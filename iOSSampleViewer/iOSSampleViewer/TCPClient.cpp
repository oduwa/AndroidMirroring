//
//  TCPClient.cpp
//  HelloWorld
//
//  Created by Oduwa Edo Osagie on 13/08/2015.
//  Copyright (c) 2015 Oduwa Edo Osagie. All rights reserved.
//

#include "TCPClient.h"

 int TCPClient::FRAME_BUFFER_SIZE = 20000000;
 int TCPClient::MESSAGE_BUFFER_SIZE = 256;
const char TCPClient::FRAME_BUFFER_REQUEST_MESSAGE[] = "wantFrame";
const char TCPClient::STOP_LISTENING_MESSAGE[] = "plsstop";

TCPClient::TCPClient()
{
    
}

bool TCPClient::connect(char *ipAddr, int port)
{
    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        CommonFuncs::error("ERROR opening socket");
    server = gethostbyname(ipAddr);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (::connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        CommonFuncs::error("ERROR connecting");
    
    return true;
}

void TCPClient::sendMessage()
{
    printf("Please enter the message: ");
    bzero(messageBuffer,256);
    fgets(messageBuffer,255,stdin);
    n = write(sockfd,messageBuffer,strlen(messageBuffer));
    if (n < 0){
        CommonFuncs::error("ERROR writing to socket");
    }
}

void TCPClient::sendTextMessage(const char* msg)
{
    int n = 0;
    n = write(sockfd, msg, strlen(msg));
    if (n < 0){
        CommonFuncs::error("ERROR writing to socket");
    }
}

char* TCPClient::receiveMessageDebug()
{
    // READ MESSAGE LENGTH
    char tempBuff[256];
    bzero(frameBuffer,20000000);
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,10);
    
    int len = atoi(tempBuff);
    printf("READ: %d\n",len);
    
    
    // READ MESSAGE IN CHUNKS
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,256);
    bool finished = false;
    int offset = 0;
    while(finished == false){
        if(n < 0){
            //CommonFuncs::error("ERROR reading from socket");
            finished = true;
        }
        else if(offset < len){
            printf("N: %d\n",n);
            for(int i = 0; i < 256; i++){
                frameBuffer[i+offset] = tempBuff[i];
            }
            offset += n;
            printf("OFFSET: %d\n",offset);
            if(offset < len){
                n = read(sockfd,tempBuff,255);
            }
            else{
                n = len;
            }
        }
        else{
            
//            int count = 0;
//            for(int i = 0; i < len; i++){
//                char c = buffer[i];
//                printf("%d\n",c);
//                //printf("0x%06X\n",c);
//                if(c == 0x05){
//                    count++;
//                }
//            }
//            printf("COUNT: %d\n",count);
            
            finished = true;
        }
        
    }
    
    return (char*)frameBuffer;
}

char* TCPClient::receiveMessage()
{
    // READ MESSAGE LENGTH
    char tempBuff[256];
    bzero(frameBuffer, FRAME_BUFFER_SIZE);
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,10);
    
    int len = atoi(tempBuff);
    printf("READ: %d\n",len);
    
    
    
    // READ MESSAGE IN CHUNKS
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,256);
    bool finished = false;
    int offset = 0;
    int readCount = 0;
    while(finished == false){
        if(n < 0){
            //CommonFuncs::error("ERROR reading from socket");
            finished = true;
        }
        else if(offset < len){
            memmove(frameBuffer+offset, tempBuff, n);
            offset += n;
            readCount += n;
            if(readCount < len){
                n = read(sockfd,tempBuff,256);
            }
            else{
                finished = true;
            }
        }
        else{
            finished = true;
        }
        
    }
    
    return (char*)frameBuffer;
}

bool TCPClient::receiveData()
{
    // READ MESSAGE LENGTH
    int chunkSize = 1024;
    char tempBuff[chunkSize];
    bzero(frameBuffer, FRAME_BUFFER_SIZE);
    bzero(tempBuff,chunkSize);
    n = read(sockfd,tempBuff,10);
    
    int len = atoi(tempBuff);
    printf("READ: %d\n",len);
    lastMessageSize = len;
    
    if(n <= 0){
        return false;
    }
    
    
    // READ MESSAGE IN CHUNKS
    bzero(tempBuff,chunkSize);
    n = read(sockfd,tempBuff,chunkSize);
    bool finished = false;
    int offset = 0;
    int readCount = 0;
    while(finished == false){
        if(n < 0){
            //CommonFuncs::error("ERROR reading from socket");
            finished = true;
        }
        else if(offset < len){
            memmove(frameBuffer+offset, tempBuff, n);
            offset += n;
            readCount += n;
            //printf("READ PROGRESS: %d\n", readCount);
            if(readCount < len){
                n = read(sockfd,tempBuff,chunkSize);
            }
            else{
                printf("READCOUNT: %d\n\n", readCount);
                finished = true;
                return true;
            }
        }
        else{
            printf("READCOUNT: %d\n\n", readCount);
            finished = true;
            return true;
        }
        
    }
    
    printf("READCOUNT: %d", readCount);
    return false;
}

bool TCPClient::receiveShortTextMessage()
{
    // READ MESSAGE
    bzero(messageBuffer, MESSAGE_BUFFER_SIZE);
    n = read(sockfd, messageBuffer, MESSAGE_BUFFER_SIZE);
    
    if(n <= 0){
        return false;
    }
    else{
        return true;
    }
}

char* TCPClient::receiveLongTextMessage()
{
    // READ MESSAGE LENGTH
    char tempBuff[256];
    char* buffer;
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,10);
    
    if(n < 0){
        return buffer;
    }
    
    int len = atoi(tempBuff);
    printf("READ: %d\n",len);
    
    
    // READ MESSAGE IN CHUNKS
    /*
    bzero(tempBuff,256);
    n = read(sockfd,tempBuff,256);
    bool finished = false;
    int offset = 0;
    int readCount = 0;
    while(finished == false){
        if(n < 0){
            //CommonFuncs::error("ERROR reading from socket");
            finished = true;
        }
        else if(offset < len){
            memmove(frameBuffer+offset, tempBuff, n);
            offset += n;
            //printf("READ PROGRESS: %d\n", readCount);
            if(readCount < len){
                n = read(sockfd,tempBuff,256);
            }
            else{
                printf("READCOUNT: %d\n\n", readCount);
                finished = true;
                return (char*)frameBuffer;
            }
        }
        else{
            printf("READCOUNT: %d\n\n", readCount);
            finished = true;
            return (char*)frameBuffer;
        }
        
    }
     */
    n = read(sockfd,frameBuffer,len);
    
    return buffer;
}

void TCPClient::sendFrameBufferRequest()
{
    int n = 0;
    n = write(sockfd, TCPClient::FRAME_BUFFER_REQUEST_MESSAGE , strlen(TCPClient::FRAME_BUFFER_REQUEST_MESSAGE));
    printf("WRITE: %d\n", n);
    if (n < 0){
        CommonFuncs::error("ERROR writing to socket");
    }
}


