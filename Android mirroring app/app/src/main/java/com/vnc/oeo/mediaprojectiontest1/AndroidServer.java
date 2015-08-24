package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.hardware.display.VirtualDisplay;
import android.util.Base64;
import android.util.Log;

import com.vnc.oeo.mediaprojectiontest1.R;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import main.Networking.RFBServer;
import main.VNCServerJNIWrapper.VNCAndroidServer;

/**
 * Created by oeo on 07/08/2015.
 */
public class AndroidServer {

    private VNCProjection mVNCProjection;
    private RFBServer sharedServer;
    private Activity mManagingActivity;

    private boolean isConnectedToAViewer;
    public static Bitmap bitmap;
    boolean testDone = false;

    private VirtualDisplay.Callback mVirtualDisplayCallback;

    private VNCProjection.VNCProjectionImageCallback mProjectionCallback = new VNCProjection.VNCProjectionImageCallback() {
        @Override
        public void didGetImage(ByteBuffer buffer, int storedBitmapWidth, int storedBitmapHeight, int pixelStride) {

            // No need to do anything unless connected to a viewer
            if(/*isConnectedToAViewer*/true){

                // Initialize Bitmap
                if(bitmap == null){
                    bitmap = Bitmap.createBitmap(storedBitmapWidth, storedBitmapHeight, Bitmap.Config.ARGB_8888);
                }

                // Populate bitmap with pixels from the byte buffer
                buffer.rewind();
                bitmap.copyPixelsFromBuffer(buffer);

                // Start service to run VNC even when app is backgrounded.
                Intent serviceIntent = new Intent(mManagingActivity, MyService.class);
                mManagingActivity.startService(serviceIntent);

                ServerActivity sa = (ServerActivity)mManagingActivity; sa.mImageView.setImageBitmap(bitmap);

            }



            if(testDone){
                testDone = true;

                // Initialize Bitmap
                if(bitmap == null){
                    bitmap = Bitmap.createBitmap(storedBitmapWidth, storedBitmapHeight, Bitmap.Config.ARGB_4444);
                }

                // Populate bitmap with pixels from the byte buffer
                buffer.rewind();

                /*
                int size3 = buffer.remaining()*4;
                byte[] data3 = new byte[size3];
                int w = 0, x = 1,y = 2,z = 3;
                for(int i = 0; i < size3/4; i++){
                    try{
                        data3[w] = (byte) Color.red((int) buffer.get(i)); w+=4;
                        data3[x] = (byte) Color.green((int) buffer.get(i)); x+=4;
                        data3[y] = (byte) Color.blue((int) buffer.get(i)); y+=4;
                        data3[z] = (byte) Color.alpha((int) buffer.get(i)); z+=4;
                    }
                    catch(IndexOutOfBoundsException e){
                        break;
                    }
                }
                //for(int i = 0; i < buffer.remaining(); i++){Log.d("PKKKKK", "BUFF: "+buffer.get(i));}
                */
                //bitmap = BitmapFactory.decodeResource(mManagingActivity.getResources(), R.drawable.image);

                bitmap.copyPixelsFromBuffer(buffer);
                byte[] byteArray = VNCUtility.bitmapToArray(bitmap);
                //byte[] byteArray = new byte[buffer.remaining()];
                //buffer.get(byteArray);
                Log.e("PKKKKK", "(" + bitmap.getWidth() + ", " + bitmap.getHeight() + ")");
                //bitmap = BitmapFactory.decodeResource(mManagingActivity.getResources(), R.mipmap.ic_launcher);
//                ByteArrayOutputStream stream = new ByteArrayOutputStream();
//                bitmap.compress(Bitmap.CompressFormat.JPEG, 0, stream);
//                byte[] byteArray = stream.toByteArray();
//
//
//
                //String encoded = Base64.encodeToString(byteArray, Base64.NO_WRAP | Base64.NO_PADDING);
                //byteArray = encoded.getBytes();

                //Log.e("PKKKKK", encoded);
                //VNCAndroidServer.sendTextMessage(encoded, encoded.length());
                VNCAndroidServer.setNewScreen(byteArray, byteArray.length);
                //VNCAndroidServer serv = new VNCAndroidServer();
                //serv.newFrameAvailable(byteArray, byteArray.length);
                //serv.sendTextMessage(encoded, encoded.length());
                ServerActivity sa = (ServerActivity)mManagingActivity; sa.mImageView.setImageBitmap(bitmap);
            }
        }



    };

    public interface ConnectionCallback{
        void connectionResult(boolean result);
    }


    public AndroidServer(Activity managingActivity){

        // Store managing activity
        mManagingActivity = managingActivity;

        // Create RFB Server
        sharedServer = RFBServer.getSharedInstance();

        // Create projection object
        mVNCProjection = new VNCProjection(mManagingActivity, null, mProjectionCallback);

    }


    public AndroidServer(Activity managingActivity, VirtualDisplay.Callback virtualDisplayCallback){

        // Store managing activity
        mManagingActivity = managingActivity;

        // Store virtual display callback
        mVirtualDisplayCallback = virtualDisplayCallback;

        // Create RFB Server
        sharedServer = RFBServer.getSharedInstance();

        // Create projection object
        mVNCProjection = new VNCProjection(mManagingActivity, mVirtualDisplayCallback, mProjectionCallback);

    }

    public void start(){
        mVNCProjection.startProjection();
    }

    public void stop(){
        mVNCProjection.stopProjection();
    }


    public void listenForConnections(final ConnectionCallback callback) {

        Thread listenThread = new Thread(new Runnable() {
            @Override
            public void run() {
                boolean success = sharedServer.connect(6880);
                isConnectedToAViewer = success;

                if(success){
                    try{
                        sharedServer.startListeningForMessages();
                    }
                    catch (Exception e){
                        success = false;
                        isConnectedToAViewer = success;
                        e.printStackTrace();
                    }

                    callback.connectionResult(success);
                }
            }
        });

        listenThread.start();
    }


    /**
     * Handle permission request and response of screen capture.<br><br/><br><br/>
     *
     * The managing activity must implement the onActivityResultMethod. Within that method, call
     * this method on your AndroidServer instance to let it handle it.<br><br/>
     *
     * This method checks the users decision on whether to enable screen capture and proceeds
     * depending on that decision.
     *
     * @param requestCode The request code for the activity request.
     * @param resultCode The result code for the activity request.
     * @param data Data that may be contained in the result of the request.
     */
    public void handleActivityResult(int requestCode, int resultCode, Intent data){
        mVNCProjection.handleActivityResult(requestCode, resultCode, data);
    }


    static {
        System.loadLibrary("JNIDummy");
    }
}
