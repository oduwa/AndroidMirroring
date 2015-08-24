package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceView;
import android.widget.ImageView;

import java.io.File;
import java.nio.ByteBuffer;

/**
 * Created by oeo on 31/07/2015.
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class VNCProjection {

    private static final String TAG = VNCProjection.class.getName();
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

    private boolean saved = false;
    private boolean isViewing = false;
    private Activity mManagingActivity;
    private VirtualDisplay.Callback mVirtualDisplayCallback;
    private ImageReader.OnImageAvailableListener mImageAvailableListener;

    public interface VNCProjectionImageCallback {
        void didGetImage(ByteBuffer buffer, int storedBitmapWidth, int storedBitmapHeight, int pixelStride);
    }
    private VNCProjectionImageCallback mImageCallback;


    // CONSTRUCTOR

    /**
     * Instantiates a VNCProjection object.
     *
     * @param activity The activity within which the VNCProjection object will be created.
     * @param virtualDisplayCallback Callback for onPause, onResume and onStopped states of
     *                               screen capture.
     * @param callback Block of code to be called whenever a new image of the screen capture is
     *                 available. Provides access to the image in the form of its byte buffer
     */
    public VNCProjection(Activity activity, VirtualDisplay.Callback virtualDisplayCallback, VNCProjectionImageCallback callback){
        mManagingActivity = activity;
        mVirtualDisplayCallback = virtualDisplayCallback;
        mImageAvailableListener = new ImageAvailableListener();
        mImageCallback = callback;

        // Get Media Projection Manager
        mProjectionManager = (MediaProjectionManager) mManagingActivity.getSystemService(Context.MEDIA_PROJECTION_SERVICE);
    }

    /**
     * Start Screen Capture.
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void startProjection() {
        if(mManagingActivity != null) {
            mManagingActivity.startActivityForResult(mProjectionManager.createScreenCaptureIntent(), REQUEST_CODE);
        }
    }

    /**
     * Stop Screen Capture.
     */
    public void stopProjection() {
        if (mMediaProjection != null) mMediaProjection.stop();
        if (mVirtualDisplay != null) mVirtualDisplay.release();
    }

    /**
     * Handle permission request and response of screen capture.<br><br/><br><br/>
     *
     * The managing activity must implement the onActivityResultMethod. Within that method, call
     * this method on your VNCProjection instance to handle it.<br><br/>
     *
     * This method checks the users decision on whether to enable screen capture and proceeds
     * depending on that decision.
     *
     * @param requestCode The request code for the activity request.
     * @param resultCode The result code for the activity request.
     * @param data Data that may be contained in the result of the request.
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void handleActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE) {
            mMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);

            if (mMediaProjection != null) {
                store_directory = Environment.getExternalStorageDirectory().getAbsolutePath()+ "/MediaProjection/";
                File storeDirectory = new File(store_directory);
                if(!storeDirectory.exists()) {
                    storeDirectory.mkdirs();
                    if(!storeDirectory.isDirectory()) {
                        Log.e(TAG, "failed to create file storage directory.");
                        //return;
                    }
                }


                /* Get screen metrics and data */
                DisplayMetrics metrics = mManagingActivity.getResources().getDisplayMetrics();
                int density = metrics.densityDpi;
                int flags = DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC;
                Display display = mManagingActivity.getWindowManager().getDefaultDisplay();
                Point size = new Point();
                display.getSize(size);
                mWidth = size.x;
                mHeight = size.y;
                Log.d(TAG, "("+mWidth+" , " + mHeight+")");
                mWidth =640;mHeight=960;
                /*
                 * Create ImageReader which will provide a surface for media projection to draw on.
                 * ImageReader also serves as a point of access to Image data of captured screens.
                 */
                mImageReader = ImageReader.newInstance(mWidth, mHeight, PixelFormat.RGBA_8888 /*ImageFormat.JPEG*/, 2);
                mImageReader.setOnImageAvailableListener(mImageAvailableListener, null);

                /* Create virtual display (where MediaProjection api projects screen captures to) */
                mVirtualDisplay = mMediaProjection.createVirtualDisplay("screencap", mWidth, mHeight, density, flags, mImageReader.getSurface(), mVirtualDisplayCallback, null);
            }
        }
    }

    private class ImageAvailableListener implements ImageReader.OnImageAvailableListener {
        @Override
        public void onImageAvailable(ImageReader reader){
            // Get Image
            Image image = reader.acquireLatestImage();

            if(image != null){
                // Get byte buffer for image
                final Image.Plane[] planes = image.getPlanes();
                final ByteBuffer buffer = planes[0].getBuffer();

                // Calculate bitmap size taking padding in the buffer into account
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * mWidth;
                int effectiveWidth = mWidth+rowPadding/pixelStride;

                // User's callback
                mImageCallback.didGetImage(buffer, effectiveWidth, mHeight, rowPadding);

                // Close image in preparation to receive next one
                image.close();
            }


        }
    }


}
