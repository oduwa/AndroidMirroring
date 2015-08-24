//
// Created by Oduwa Edo Osagie on 03/08/2015.
//
#include <string.h>
#include <jni.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "TCPServer.h"
#include "AndroidServer.h"
#include "bZip2/bzlib.h"
#include "zlib/zlib.h"

#include "ashmem.h"
#include <sys/mman.h>

#include "vnc/Vnc.h"


#define SHARED_MEM_SIZE 100000



void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void runServer(char *arg, char *argi)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    /*
     if (argc < 2) {
     fprintf(stderr,"ERROR, no port provided\n");
     exit(1);
     }
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argi);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    __android_log_print(ANDROID_LOG_DEBUG, "JNIDummy", "Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);
}

void runServer2(int portNumber)
{
    int sockfd; // Socket file descriptor
    int newsockfd; // Socket file descriptor
    int portno; // Port no. to connect on


    socklen_t clilen; // stores the size of the address of the client. This is needed for the accept system call.
    int n; // the return value for the read() and write() calls; i.e. it contains the number of characters read or written.
    char buffer[256]; // buffer to read data/characters into from the connection


    /*
     * structs containing inet addresses.

     The struct is defined as below:
     struct sockaddr_in
     {
        short   sin_family; // must be AF_INET
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8]; // Not used, must be zero
     }
     */
    struct sockaddr_in serv_addr, cli_addr;

    /*
     * The socket() system call creates a new socket. It takes three arguments. The first is the address domain of the socket.
     Recall that there are two possible address domains, the unix domain for two processes which share a common file system, and the Internet domain for any two hosts on the Internet. The symbol constant AF_UNIX is used for the former, and AF_INET for the latter (there are actually many other options which can be used here for specialized purposes).

     The second argument is the type of socket. Recall that there are two choices here, a stream socket in which characters are read in a continuous stream as if from a file or pipe, and a datagram socket, in which messages are read in chunks. The two symbolic constants are SOCK_STREAM and SOCK_DGRAM.

     The third argument is the protocol. If this argument is zero (and it always should be except for unusual circumstances), the operating system will choose the most appropriate protocol. It will choose TCP for stream sockets and UDP for datagram sockets.
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("ERROR opening socket");
    }


    /* The function bzero() sets all values in a buffer to zero. It takes two arguments, the first is a pointer to the buffer and the second is the size of the buffer. Thus, this line initializes serv_addr to zeros. */
    bzero((char *) &serv_addr, sizeof(serv_addr));


    portno = portNumber;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno); // converts number to big endian (network byte order) if on little endian host
    serv_addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }


    /*
     * The listen system call allows the process to listen on the socket for connections. The first argument is the socket file descriptor, and the second is the size of the backlog queue, i.e., the number of connections that can be waiting while the process is handling a particular connection. This should be set to 5, the maximum size permitted by most systems.
     *
     * The accept() system call causes the process to block until a client connects to the server. Thus, it wakes up the process when a connection from a client has been successfully established.
     */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }


    /*
     * Note also that the read() will block until there is something for it to read in the socket.
     *
     * It will read either the total number of characters in the socket or 255, whichever is less,
     * and return the number of characters read. The read stuff is actually in the buffer.
     */
    while(1){
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0){
            error("ERROR reading from socket");
        }
        printf("Here is the message: %s",buffer);
        __android_log_print(ANDROID_LOG_DEBUG, "JNIDummy", "Here is the message: %s\n",buffer);
    }



}




extern "C" {

static AndroidServer *androidServer = NULL;
bool isAndroidServerConnected = false;
int fd = 0;

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_startSharedMemServer(JNIEnv *env, jclass type) {

    /* Create a region of shared memory and get a file descriptor for it */
    fd = ashmem_create_region("VNCSharedMemory", SHARED_MEM_SIZE);

    /* Map the region */
    char *shm = (char*)mmap(NULL, SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    /* Use shared memory */
    memcpy(shm, "This is shared memory", 21);

    __android_log_print(ANDROID_LOG_ERROR, "AshMemServer", "ASHMEM FD: %d", fd);
    __android_log_print(ANDROID_LOG_ERROR, "AshMemServer", "SHARED MEM CONTENTS: %s", shm);

}

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_startSharedMemClient(JNIEnv *env, jclass type) {

    /* Map the region */
    char *shm = (char*)mmap(NULL, SHARED_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    /* Use shared memory */
    //memcpy(shm, "This is shared memory", 21);

    __android_log_print(ANDROID_LOG_ERROR, "AshMemClient", "ASHMEM FD: %d", fd);
    __android_log_print(ANDROID_LOG_ERROR, "AshMemClient", "SHARED MEM CONTENTS: %s", shm);

}

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_newScreenAvailable2(JNIEnv *env, jclass type,
                                                                   jbyteArray frameData_,
                                                                   jint len) {
    //jbyte *frameData = env->GetByteArrayElements(frameData_, NULL);
    //signed char* data = frameData;



    __android_log_write(ANDROID_LOG_ERROR, "JNIDummy", "CREATE NEW SERVER");


    // TODO
    if(androidServer == NULL){
        __android_log_write(ANDROID_LOG_ERROR, "JNIDummy", "CREATING SERVER");
        androidServer = new AndroidServer(6881);
        androidServer->connect();
        isAndroidServerConnected = true;
        __android_log_write(ANDROID_LOG_ERROR, "JNIDummy", "CREATED SERVER");
    }


    //env->ReleaseByteArrayElements(frameData_, frameData, 0);
}

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_setNewScreen(JNIEnv *env, jclass type,
                                                            jbyteArray frameData_, jint len) {
    jbyte *frameData = env->GetByteArrayElements(frameData_, NULL);


    // TODO
    if(androidServer == NULL/* || isAndroidServerConnected == false*/){
        __android_log_write(ANDROID_LOG_ERROR, "JNIDummy", "ANDROID SERVER IS NULL");
    }
    else{
        //if(androidServer->socketServer->isFrameDataBeingUsed == false){
            //__android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "SEND DATA");
        signed char* data = frameData;

        /* bZip2 Compression
        __android_log_print(ANDROID_LOG_ERROR, "JNIDummy", "ABOUT TO COMPRESS");
        // Compress data
        unsigned int initialSize = len;
        unsigned int resultingSize = len;
        char *compressedData = new char[resultingSize];
        BZ2_bzBuffToBuffCompress(compressedData, &resultingSize, (char *)data, initialSize, 5, 0, 0);
        __android_log_print(ANDROID_LOG_ERROR, "JNIDummy", "COMPRESSED TO %d", resultingSize);
         */

        // zlib Compression
        __android_log_print(ANDROID_LOG_ERROR, "JNIDummy", "ABOUT TO COMPRESS");
        uLong initialSize = len;
        uLong resultingSize = len;
        char *compressedData = new char[resultingSize];
        compress((Bytef*)compressedData, &resultingSize, (Bytef*)data, initialSize);
        __android_log_print(ANDROID_LOG_ERROR, "JNIDummy", "COMPRESSED TO %d", resultingSize);

        // Move compressed data into socketserver
        memmove(androidServer->socketServer->data, compressedData, resultingSize);
        androidServer->socketServer->frameSize = resultingSize;


        // free memory
        //delete [] compressedData;
        //}
    }

    env->ReleaseByteArrayElements(frameData_, frameData, 0);
}

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_sendTextMessage(JNIEnv *env, jobject instance,
                                                               jstring msg_, jint len) {
    const char *msg = env->GetStringUTFChars(msg_, 0);

    // TODO
    TCPServer serv = TCPServer();
    bool result = serv.connect(6881);
    if(result == true){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "Server start listening ... ... ...");
        serv.sendTextData(msg, len);
        //serv.startListening();

    }
    else{
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "BAD THINGS HAPPENED. FAILED TO CONNECT");
    }

    env->ReleaseStringUTFChars(msg_, msg);
}

JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_newFrameAvailable(JNIEnv *env, jobject instance,
                                                                 jbyteArray frameData_, jint len) {
    jbyte *frameData = env->GetByteArrayElements(frameData_, NULL);

    // TODO
    signed char* data = frameData;


    TCPServer serv = TCPServer();
    bool result = serv.connect(6881);
    if(result == true){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "Server start listening ... ... ...");
        serv.sendData(data, len);
        //serv.startListening();

    }
    else{
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "BAD THINGS HAPPENED. FAILED TO CONNECT");
    }

    env->ReleaseByteArrayElements(frameData_, frameData, 0);
}

JNIEXPORT jstring JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_getStringFromJNI(JNIEnv *env, jobject instance){

    return env->NewStringUTF("A STRING FROM JNI");
}




JNIEXPORT void JNICALL
Java_main_VNCServerJNIWrapper_VNCAndroidServer_newScreenAvailable(JNIEnv *env, jobject instance,
                                                                  jobject buffer) {

    // TODO
    // Do Something
    __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "Doing Jni stuff Doing Jni stuffDoing Jni stuff Doing Jni stuff");




    /*
    //runServer2(6881);
    TCPServer serv = TCPServer();
    bool result = serv.connect(6881);
    if(result == true){
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "Server start listening ... ... ...");
        //serv.startListening();

    }
    else{
        __android_log_write(ANDROID_LOG_DEBUG, "JNIDummy", "BAD THINGS HAPPENED. FAILED TO CONNECT");
    }
     */
}


}


