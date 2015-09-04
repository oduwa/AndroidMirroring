
#ifndef __HelloWorld__AndroidServer__
#define __HelloWorld__AndroidServer__

#include <stdio.h>
#include <mob/ISocketServer.h>


class AndroidServer {
    
private:
    
    
public:
    ISocketServer *socketServer;
    int portNumber;
    
    AndroidServer();
    AndroidServer(int port);
    ~AndroidServer();
    
    bool connect();
    void startCommunications();
    void stopCommunications();
    
    
};


#endif /* defined(__HelloWorld__AndroidServer__) */
