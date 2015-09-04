
#include <mob/vnc_Android_Viewer.h>

#if defined(__ANDROID__)
#include <android/log.h>
#endif


vnc_Android_Viewer::vnc_Android_Viewer()
{
    ipAddr = "";
    portNo = 0;
    
    client = NULL;
    callback = NULL;
}

vnc_Android_Viewer::vnc_Android_Viewer(char* ip, int port, AndroidViewerCallbackInterface *callback)
{
    vnc_Android_Viewer::ipAddr = ip;
    vnc_Android_Viewer::portNo = port;
    vnc_Android_Viewer::callback = callback;
    
    vnc_Android_Viewer::client = new TCPClient();
}

vnc_Android_Viewer::~vnc_Android_Viewer()
{
    delete client;
}

bool vnc_Android_Viewer::connect()
{
    if(client != NULL){
        return client->connect(ipAddr, portNo);
    }
    else{
        return false;
    }
}

void vnc_Android_Viewer::startViewing()
{
    isViewing = true;
    while(isViewing == true){
        // Send initial frame buffer request
        client->sendFrameBufferRequest();
        sleep(1);
        
        bool result = client->receiveData();
        while(result == false){
            result = client->receiveData();
        }
        
        callback->newFrameAvailable(client->frameBuffer, client->lastMessageSize);
        sleep(0.2);
    }
}




