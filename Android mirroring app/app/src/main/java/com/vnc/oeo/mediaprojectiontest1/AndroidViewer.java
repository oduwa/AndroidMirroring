package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.IOException;

import main.Networking.RFBClient;

/**
 * Created by oeo on 07/08/2015.
 */
public class AndroidViewer {

    private static final String TAG = AndroidViewer.class.getName();

    private String ipAddress;
    private int port;
    private RFBClient rfbClient;
    private Thread viewingThread;
    private boolean isViewing;

    public interface ViewerCallback{
        public void connectionResult(boolean success);
        public void didGetFrame(byte[] frame);
    }


    public AndroidViewer(){
        rfbClient = new RFBClient();
    }

    public void connect(String ip, int port, ViewerCallback viewerCallback){
        this.ipAddress = ip;
        this.port = port;

        viewingThread = new ViewerThread(viewerCallback);
        viewingThread.start();
    }

    public void resumeViewing(){
        isViewing = true;
        viewingThread.start();
    }

    public void pauseViewing(){
        isViewing = false;
    }

    private class ViewerThread extends Thread {

        private ViewerCallback mViewerCallback;

        public ViewerThread(ViewerCallback viewerCallback){
            super();
            mViewerCallback = viewerCallback;
        }

        @Override
        public void run(){

            // Connect to server
            boolean success;
            if(rfbClient.socket == null){
                success = rfbClient.connect(ipAddress, port);
            }
            else{
                success = true;
            }
            mViewerCallback.connectionResult(success);
            isViewing = success;

            if(success){
                // Send initial frame request
                Log.d(TAG, "SENT FRAME REQUEST MESSAGE");
                try {
                    rfbClient.requestFrameBufferUpdate();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                // Viewing loop where frame buffer update requests are made
                while(isViewing){
                    try{
                        // Get new frame
                        byte[] screen = rfbClient.getFrameBuffer();

                        // If frame is invalid, request new frame
                        if(screen == null){
                            Log.d(TAG, "SENT FRAME REQUEST MESSAGE");
                            rfbClient.requestFrameBufferUpdate();
                        }
                        else{
                            mViewerCallback.didGetFrame(screen);
                        }
                    }
                    catch (Exception e){
                        e.printStackTrace();
                    }
                }
            }
        }

    }



}
