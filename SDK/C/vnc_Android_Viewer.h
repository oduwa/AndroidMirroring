

#ifndef __VNC__vnc_Android_Viewer__
#define __VNC__vnc_Android_Viewer__

#include <stdio.h>
#include <mob/TCPClient.h>

class vnc_Android_Viewer {

    
private:
    

public:
    class AndroidViewerCallbackInterface{
    public:
        virtual void newFrameAvailable(signed char* data, int size) = 0;
    };
    
    TCPClient *client;
    AndroidViewerCallbackInterface *callback;
    char* ipAddr;
    int portNo;
    bool isViewing;
    signed char *frameBuffer;
    
    vnc_Android_Viewer();
    vnc_Android_Viewer(char* ip, int port, AndroidViewerCallbackInterface* callback);
    ~vnc_Android_Viewer();
    
    bool connect();
    void startViewing();
    
};

#endif /* defined(__VNC__vnc_Android_Viewer__) */
