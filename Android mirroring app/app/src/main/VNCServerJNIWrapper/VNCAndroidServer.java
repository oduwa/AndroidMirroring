package main.VNCServerJNIWrapper;

import java.nio.ByteBuffer;

/**
 * Created by oeo on 03/08/2015.
 */
public class VNCAndroidServer {

    public native String getStringFromJNI();

    public native void newScreenAvailable(ByteBuffer buffer);
    public native void newFrameAvailable(byte[] frameData, int len);
    public static native void sendTextMessage(String msg, int len);
    public static native void newScreenAvailable2(byte[] frameData, int len);
    public static native void setNewScreen(byte[] frameData, int len);

    public static native void startSharedMemServer();
    public static native void startSharedMemClient();


    public static void start(){
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                VNCAndroidServer.newScreenAvailable2(null, 0);
            }
        });
        t.start();
    }

    static {
        System.loadLibrary("JNIDummy");
    }

}
