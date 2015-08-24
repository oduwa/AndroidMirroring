package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import com.vnc.oeo.mediaprojectiontest1.R;

import org.apache.http.conn.util.InetAddressUtils;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URL;
import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.List;

import main.Networking.RFBServer;
import main.Networking.TCPClient;
import main.Networking.TCPServer;


@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class ServerActivity extends Activity {

    private static final String TAG = ServerActivity.class.getName();
    private static final int REQUEST_CODE = 100;
    private static MediaProjection mMediaProjection;
    private static String store_directory;
    private static int images_produced;

    private MediaProjectionManager mProjectionManager;
    private ImageReader mImageReader;
    private VirtualDisplay mVirtualDisplay;
    private Handler mHandler;
    private int mWidth;
    private int mHeight;
    private int mBWidth;

    private VNCProjection mVNCProjection;
    public ImageView mImageView;
    private ImageView mViewerImageView;
    private SurfaceView mSurfaceView;
    private Surface mSurface;
    public static Bitmap mBmp;
    private Activity mActivity;
    private ByteBuffer mByteBuffer;
    private boolean saved = false;
    private boolean isViewing = false;

    static boolean done = false;

    public static byte[] sharedByteArray;
    public static Bitmap sharedBitmap;
    private boolean isConnectedToAViewer = false;
    RFBServer sharedServer;
    static AndroidServer mAndroidServer;


    //region Callbacks
    private VirtualDisplay.Callback mVirtualDisplayCallback = new VirtualDisplay.Callback() {
        @Override
        public void onPaused() {
            super.onPaused();
            Log.d(TAG, "VIRTUAL DISPLAY PAUSED");
        }

        @Override
        public void onResumed() {
            super.onResumed();
            Log.e(TAG, "VIRTUAL DISPLAY RESUMED");
        }

        @Override
        public void onStopped() {
            super.onStopped();
            Log.d(TAG, "VIRTUAL DISPLAY STOPPED");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_server);

        // Store Context
        mActivity = this;

        // ImageView
        mImageView = (ImageView) findViewById(R.id.serverImageView);

        // Create android server
        mAndroidServer = new AndroidServer(mActivity);

        // Start projection
        mAndroidServer.start();

        // start button
        Button startButton = (Button)findViewById(R.id.startButton);
        startButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                // Start listening for connections
                mAndroidServer.listenForConnections(new AndroidServer.ConnectionCallback() {
                    @Override
                    public void connectionResult(boolean result) {
                        if (result == true) {
                            Toast.makeText(mActivity, "SUCCESSFULLY CONNECTED TO A VIEWER", Toast.LENGTH_LONG);
                        }
                    }
                });

            }
        });



        /*

        // Get directory for file io
        store_directory = Environment.getExternalStorageDirectory().getAbsolutePath()+ "/MediaProjection/";
        File storeDirectory = new File(store_directory);
        if(!storeDirectory.exists()) {
            storeDirectory.mkdirs();
            if(!storeDirectory.isDirectory()) {
                Log.e(TAG, "failed to create file storage directory.");
            }
        }

        // Create RFB Server
        sharedServer = RFBServer.getSharedInstance();
        

        // Instantiate VNC Projection Manager
        mVNCProjection = new VNCProjection(this, mVirtualDisplayCallback, new VNCProjection.VNCProjectionImageCallback() {
            @Override
            public void didGetImage(ByteBuffer buffer, int storedBitmapWidth, int storedBitmapHeight) {

                if(sharedServer.clientSocket == null){
                    isConnectedToAViewer = false;
                    mImageView.setImageBitmap(BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher));
                }


                if(isConnectedToAViewer){
                    // Create Bitmap
                    if(mBmp == null || sharedBitmap == null){
                        mBmp = Bitmap.createBitmap(storedBitmapWidth, storedBitmapHeight, Bitmap.Config.ARGB_8888);
                        sharedBitmap = Bitmap.createBitmap(storedBitmapWidth, storedBitmapHeight, Bitmap.Config.ARGB_8888);
                    }
                    byte[] bytes = new byte[buffer.remaining()];
                    buffer.get(bytes);
                    sharedByteArray = bytes;

                    buffer.rewind();
                    mBmp.copyPixelsFromBuffer(buffer);
                    buffer.rewind();
                    //sharedBitmap.copyPixelsFromBuffer(buffer);
                    sharedBitmap = mBmp;

                    // Send to VNC server in background service which can run in background
                    //VNCUtility.saveBitmapToFile(mBmp, "vncbitmap.png", store_directory, false);
                    Intent i= new Intent(mActivity, MyService.class);
                    //i.putExtra("photoPath", store_directory + "/" + "vncbitmap.png");
                    mActivity.startService(i);

                    // Update image view to show what is being captured
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mImageView.setImageBitmap(mBmp);
                        }
                    });
                }





            }
        });


        // Get Image View
        mImageView = (ImageView)findViewById(R.id.serverImageView);

        // start projection
        Button startButton = (Button)findViewById(R.id.startButton);
        startButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mVNCProjection.startProjection();
                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {

                        final boolean success = sharedServer.connect(6880);
                        isConnectedToAViewer = success;;

                        if(success){
                            try {
                                sharedServer.startListeningForMessages();
                                //sharedServer.sendMessage("HABHABHAB");
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                        Log.d(TAG, "SERVER LISTENING RESULT: " + success);
                    }
                });
                t.start();
            }
        });

        // stop projection
        Button stopButton = (Button)findViewById(R.id.stopButton);
        stopButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mVNCProjection.stopProjection();
            }
        });
        */
    }




    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        mAndroidServer.handleActivityResult(requestCode, resultCode, data);
    }



    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_server, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }


    ////////////////////////////////////////// UTILITY METHODS /////////////////////////////////////

    //region Utility Methods


    private void manuallyPushImageToImageReader(){
        mImageReader = VNCUtility.createImageReader(this);
        mImageReader.setOnImageAvailableListener(new ImageReader.OnImageAvailableListener() {
            @Override
            public void onImageAvailable(ImageReader reader) {
                Log.d(TAG, "NEW IMAGE AVAILABLE");

                DisplayMetrics metrics = getResources().getDisplayMetrics();


                Image image = reader.acquireLatestImage();
                VNCUtility.printReaderFormat(image.getFormat());
                Log.d(TAG, "IMAGE FORMAT: " + image.getFormat());
                final Image.Plane[] planes = image.getPlanes();
                final ByteBuffer buffer = planes[0].getBuffer();
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * mWidth;
                int w = mWidth + rowPadding / pixelStride;
                Bitmap bmp = Bitmap.createBitmap(mWidth, mHeight, Bitmap.Config.RGB_565);
                bmp.copyPixelsFromBuffer(buffer);
                mBmp = bmp;

                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            //saveScreen(mBmp);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }

                    }
                });
                t.start();

                image.close();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mImageView.setImageBitmap(mBmp);
                    }
                });


                /*
                Image img = reader.acquireLatestImage();
                Image.Plane[] planes = img.getPlanes();
                int width = img.getWidth();
                int height = img.getHeight();
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * width;
                ByteBuffer buffer = planes[0].getBuffer();
                byte[] data = new byte[buffer.capacity()];
                buffer.get(data);

                for(int i = 10000; i < 100; i++){
                    Log.d(TAG, getColourForInt(data[i]));
                }

                int offset = 0;
                Bitmap bitmap = Bitmap.createBitmap(metrics, width, height, Bitmap.Config.ARGB_8888);
                for (int i = 0; i < height; ++i) {
                    for (int j = 0; j < width; ++j) {
                        int pixel = 0;
                        pixel |= (buffer.get(offset) & 0xff) << 16;     // R
                        pixel |= (buffer.get(offset + 1) & 0xff) << 8;  // G
                        pixel |= (buffer.get(offset + 2) & 0xff);       // B
                        pixel |= (buffer.get(offset + 3) & 0xff) << 24; // A
                        bitmap.setPixel(j, i, pixel);
                        offset += pixelStride;
                    }
                    offset += rowPadding;
                }
                mBmp = bitmap;
                img.close();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mImageView.setImageBitmap(mBmp);
                    }
                });
                */
            }
        }, null);
        Surface surface = mImageReader.getSurface();
        if(surface == null){
            Log.d(TAG, "SURFACE IS NULL");
        }
        else{
            Log.d(TAG, "SURFACE IS NOT NULL");
            Canvas canvas = surface.lockCanvas(null);

            int[] ints = new int[mWidth * mHeight];
            for (int i = 0; i < mWidth * mHeight; i++) {
                ints[i] = Color.MAGENTA;//0x00FF0000;
            }

            //final Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, ints.length);
            Bitmap bitmap = Bitmap.createBitmap(ints, mWidth, mHeight, Bitmap.Config.RGB_565);
            //canvas.drawARGB(255, 255, 0, 255);
            canvas.drawBitmap(bitmap,0,0,null);
            surface.unlockCanvasAndPost(canvas);
        }
    }



    //endregion

    static {
        System.loadLibrary("JNIDummy");
    }
}
