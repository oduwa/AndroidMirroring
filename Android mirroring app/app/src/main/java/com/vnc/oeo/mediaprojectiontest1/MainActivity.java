package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.app.Activity;
import android.app.Service;
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
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.util.Base64;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import com.vnc.oeo.mediaprojectiontest1.R;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.conn.util.InetAddressUtils;
import org.apache.http.entity.mime.HttpMultipartMode;
import org.apache.http.entity.mime.MultipartEntity;
import org.apache.http.entity.mime.content.ByteArrayBody;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URL;
import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.List;

public class MainActivity extends Activity {

    private static final String TAG = MainActivity.class.getName();
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

    private ImageView mImageView;
    private ImageView mViewerImageView;
    private SurfaceView mSurfaceView;
    private Surface mSurface;
    public static Bitmap mBmp;
    private Activity mActivity;
    private ByteBuffer mByteBuffer;
    private boolean saved = false;
    private boolean isViewing = false;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mActivity = this;

        // call for the projection manager
        mProjectionManager = (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);


        // start projection
        Button startButton = (Button)findViewById(R.id.startButton);
        startButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                startProjection();
                //manuallyPushImageToImageReader();
                /*
                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try{
                            if(saved == false){
                                saved = true;
                                mBmp = BitmapFactory.decodeResource(mActivity.getResources(), R.mipmap.ic_launcher); //getBitmapFromURL("http://10.0.2.2:80/AndroidVNCWebServer/FileUploads/x.png");
                                saveScreen(mBmp);
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(mActivity, "DONE", Toast.LENGTH_LONG).show();
                                        mImageView.setImageBitmap(mBmp);
                                    }
                                });

                            }
                        }
                        catch (Exception e){
                            e.printStackTrace();
                        }

                    }
                });
                t.start();
                */
            }
        });

        // stop projection
        Button stopButton = (Button)findViewById(R.id.stopButton);
        stopButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                stopProjection();
            }
        });

        // start viewing
        Button startViewButton = (Button)findViewById(R.id.startViewButton);
        startViewButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                isViewing = true;
                getScreen();
            }
        });

        // stop viewing
        Button stopViewButton = (Button)findViewById(R.id.stopViewButton);
        stopViewButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                isViewing = false;
            }
        });


        // Get Image View
        mImageView = (ImageView)findViewById(R.id.imageView);
        mViewerImageView = (ImageView)findViewById(R.id.imageView2);
        mImageView.setImageResource(R.mipmap.ic_launcher);


        // Surface View
        mSurfaceView = (SurfaceView)findViewById(R.id.surfaceView);
        mSurface = mSurfaceView.getHolder().getSurface();

        /*
        String url = "";
        String image = "x";
        boolean isRunningOnEmulator = Build.BRAND.toLowerCase().startsWith("generic");
        if(isRunningOnEmulator){
            url = "http://10.0.2.2:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + getIPAddress(true) + "&img=" + image;
        }
        else{
            url = "http://192.168.5.5:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + getIPAddress(true) + "&img=" + image;
        }

        OEJSONParser parser = new OEJSONParser(url);
        parser.fetchJSON(new OEJSONParser.JSONTask.JSONCallback() {
            @Override
            public void onComplete(JSONObject result) {
                Toast.makeText(mActivity, result.toString(), Toast.LENGTH_LONG).show();
            }
        });
        */

    }

    public static class FetchScreenTask extends AsyncTask<String, Void, Bitmap> {
        // Callback to allow user to specify what happens once task completes
        public interface TaskCallback {
            void onComplete(Bitmap result);
        }
        private TaskCallback delegate;
        private String urlString;
        private Bitmap bitmap;

        // AsyncTask Constructor
        FetchScreenTask(String URLString, TaskCallback callback) {
            this.delegate = callback;
            this.urlString = URLString;
        }

        // Work to be done in the background ie. json parsing.
        @Override
        protected Bitmap doInBackground(String... params) {
            try {
                URL url = new URL(urlString);
                HttpURLConnection connection = (HttpURLConnection) url.openConnection();
                connection.setDoInput(true);
                connection.connect();
                InputStream input = connection.getInputStream();
                Bitmap myBitmap = BitmapFactory.decodeStream(input);
                return myBitmap;

            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        }

        // code to be fired once background task completes. callbacks are called here
        @Override
        protected void onPostExecute(Bitmap result) {
            super.onPostExecute(result);
            delegate.onComplete(result);
        }
    }

    public void getScreen(){
        String urlString = "";
        String serverId = getIPAddress(true);
        boolean isRunningOnEmulator = Build.BRAND.toLowerCase().startsWith("generic");
        if(isRunningOnEmulator){
            urlString = "http://10.0.2.2:80/AndroidVNCWebServer/FileUploads/10.10.19.228.png";
        }
        else{
            urlString = "http://192.168.5.1:80/AndroidVNCWebServer/FileUploads/10.10.19.228.png";
        }

        FetchScreenTask task = new FetchScreenTask(urlString, new FetchScreenTask.TaskCallback() {
            @Override
            public void onComplete(Bitmap result) {
                if(result != null){
                    mViewerImageView.setImageBitmap(result);
                }

                if(isViewing == true){
                    getScreen();
                }
            }
        });
        task.execute();
    }

    public void saveScreen(Bitmap bitmap) throws Exception{
        String urlString = "";
        String serverId = getIPAddress(true);
        boolean isRunningOnEmulator = Build.BRAND.toLowerCase().startsWith("generic");
        if(isRunningOnEmulator){
            urlString = "http://10.0.2.2:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + serverId;
        }
        else{
            urlString = "http://192.168.5.1:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + serverId;
        }

        //bitmap = /*getBitmapFromURL("https://yt3.ggpht.com/-cnldRsClRh0/AAAAAAAAAAI/AAAAAAAAAAA/-o9U-7bqgeY/s88-c-k-no/photo.jpg");*/ //BitmapFactory.decodeResource(mActivity.getResources(), R.mipmap.ic_launcher);
        String attachmentName = "bitmap";
        String attachmentFileName = "bitmap.bmp";
        String crlf = "\r\n";
        String twoHyphens = "--";
        String boundary =  "*****";


        // Setup the request
        HttpURLConnection httpUrlConnection = null;
        URL url = new URL(urlString);
        httpUrlConnection = (HttpURLConnection) url.openConnection();
        httpUrlConnection.setUseCaches(false);
        httpUrlConnection.setDoOutput(true);

        httpUrlConnection.setRequestMethod("POST");
        httpUrlConnection.setRequestProperty("Connection", "Keep-Alive");
        httpUrlConnection.setRequestProperty("Cache-Control", "no-cache");
        httpUrlConnection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);

        // Send server id. Start content wrapper
        DataOutputStream request = new DataOutputStream(httpUrlConnection.getOutputStream());

        request.writeBytes(twoHyphens + boundary + crlf);
        request.writeBytes("Content-Disposition: form-data; name=\"serverID\"" + crlf);
        request.writeBytes(crlf);

        //  Write serverID
        request.write(serverId.getBytes());

        // End content wrapper
        request.writeBytes(crlf);
        request.writeBytes(twoHyphens + boundary + twoHyphens + crlf);

        // Start content wrapper for image
        request.writeBytes(twoHyphens + boundary + crlf);
        request.writeBytes("Content-Disposition: form-data; name=\"" + attachmentName + "\";filename=\"" + attachmentFileName + "\"" + crlf);
        request.writeBytes(crlf);

        // Convert bitmap to byte buffer
        ByteArrayOutputStream output = new ByteArrayOutputStream(bitmap.getByteCount());
        bitmap.compress(Bitmap.CompressFormat.JPEG, 0, output);
        byte[] pixels = output.toByteArray();
        request.write(pixels);

        // End content wrapper
        request.writeBytes(crlf);
        request.writeBytes(twoHyphens + boundary + twoHyphens + crlf);

        // Flush output buffer
        request.flush();
        request.close();

        // Get Response
        InputStream responseStream = new BufferedInputStream(httpUrlConnection.getInputStream());

        BufferedReader responseStreamReader = new BufferedReader(new InputStreamReader(responseStream));
        String line = "";
        StringBuilder stringBuilder = new StringBuilder();
        while ((line = responseStreamReader.readLine()) != null)
        {
            stringBuilder.append(line).append("\n");
        }
        responseStreamReader.close();

        String response = stringBuilder.toString();
        Log.e(TAG, "HTTPRESPONSE: " + response);

        // Close response stream
        responseStream.close();

        // Close the connection
        httpUrlConnection.disconnect();
    }

    public void saveScreen2(ByteBuffer buffer, int width, int height) throws Exception{
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(buffer);
        String urlString = "";
        String serverId = getIPAddress(true);
        boolean isRunningOnEmulator = Build.BRAND.toLowerCase().startsWith("generic");
        if(isRunningOnEmulator){
            urlString = "http://10.0.2.2:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + serverId;
        }
        else{
            urlString = "http://192.168.5.1:80/AndroidVNCWebServer/UploadScreen.php?serverID=" + serverId;
        }

        //bitmap = /*getBitmapFromURL("https://yt3.ggpht.com/-cnldRsClRh0/AAAAAAAAAAI/AAAAAAAAAAA/-o9U-7bqgeY/s88-c-k-no/photo.jpg");*/ //BitmapFactory.decodeResource(mActivity.getResources(), R.mipmap.ic_launcher);
        String attachmentName = "bitmap";
        String attachmentFileName = "bitmap.bmp";
        String crlf = "\r\n";
        String twoHyphens = "--";
        String boundary =  "*****";


        // Setup the request
        HttpURLConnection httpUrlConnection = null;
        URL url = new URL(urlString);
        httpUrlConnection = (HttpURLConnection) url.openConnection();
        httpUrlConnection.setUseCaches(false);
        httpUrlConnection.setDoOutput(true);

        httpUrlConnection.setRequestMethod("POST");
        httpUrlConnection.setRequestProperty("Connection", "Keep-Alive");
        httpUrlConnection.setRequestProperty("Cache-Control", "no-cache");
        httpUrlConnection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);

        // Send server id. Start content wrapper
        DataOutputStream request = new DataOutputStream(httpUrlConnection.getOutputStream());

        request.writeBytes(twoHyphens + boundary + crlf);
        request.writeBytes("Content-Disposition: form-data; name=\"serverID\"" + crlf);
        request.writeBytes(crlf);

        //  Write serverID
        request.write(serverId.getBytes());

        // End content wrapper
        request.writeBytes(crlf);
        request.writeBytes(twoHyphens + boundary + twoHyphens + crlf);

        // Start content wrapper for image
        request.writeBytes(twoHyphens + boundary + crlf);
        request.writeBytes("Content-Disposition: form-data; name=\"" + attachmentName + "\";filename=\"" + attachmentFileName + "\"" + crlf);
        request.writeBytes(crlf);

        // Convert bitmap to byte buffer
        ByteArrayOutputStream output = new ByteArrayOutputStream(bitmap.getByteCount());
        bitmap.compress(Bitmap.CompressFormat.JPEG, 0, output);
        byte[] pixels = output.toByteArray();
        request.write(pixels);

        // End content wrapper
        request.writeBytes(crlf);
        request.writeBytes(twoHyphens + boundary + twoHyphens + crlf);

        // Flush output buffer
        request.flush();
        request.close();

        // Get Response
        InputStream responseStream = new BufferedInputStream(httpUrlConnection.getInputStream());

        BufferedReader responseStreamReader = new BufferedReader(new InputStreamReader(responseStream));
        String line = "";
        StringBuilder stringBuilder = new StringBuilder();
        while ((line = responseStreamReader.readLine()) != null)
        {
            stringBuilder.append(line).append("\n");
        }
        responseStreamReader.close();

        String response = stringBuilder.toString();
        Log.e(TAG, "HTTPRESPONSE: " + response);

        // Close response stream
        responseStream.close();

        // Close the connection
        httpUrlConnection.disconnect();
    }


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

    private class TestImageAvailableListener implements ImageReader.OnImageAvailableListener {
        @Override
        public void onImageAvailable(ImageReader reader){
            Image image = reader.acquireLatestImage();
            Log.e(TAG, "FOUND IMAGE");

            final Image.Plane[] planes = image.getPlanes();
            final ByteBuffer buffer = planes[0].getBuffer();
            int pixelStride = planes[0].getPixelStride();
            int rowStride = planes[0].getRowStride();
            int rowPadding = rowStride - pixelStride * mWidth;
            int w = mWidth+rowPadding/pixelStride;
            mBWidth = w;
            mByteBuffer = buffer;

            // Create Bitmap
            if(mBmp == null){
                mBmp = Bitmap.createBitmap(w, mHeight, Bitmap.Config.ARGB_8888);
            }
            buffer.rewind();
            mBmp.copyPixelsFromBuffer(buffer);


            //Log.d(TAG, "STARTED SERVICE");

            saveBitmapToFile(mBmp, "vncbitmap.png", store_directory, false);
            Intent i= new Intent(mActivity, MyService.class);
            i.putExtra("photoPath", store_directory + "/" + "vncbitmap.png");
            mActivity.startService(i);


            image.close();
        }
    }

    public static String getCacheDir(Context ctx) {
        return Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) ||!Environment.isExternalStorageRemovable() ?
                ctx.getExternalCacheDir().getPath() : ctx.getCacheDir().getPath();
    }

    public static File saveBitmapToFile(Bitmap bitmap, String filename, String path, boolean recycle) {
        FileOutputStream out=null;
        try {
            File f = new File(path,filename);
            if(!f.exists()) {
                f.createNewFile();
            }
            out = new FileOutputStream(f);
            if(bitmap.compress(Bitmap.CompressFormat.PNG, 90, out)) {
                return f;
            }
        } catch (Exception e) {
            Log.e(TAG, "Could not save bitmap", e);
        } finally {
            try{
                out.close();
            } catch(Throwable ignore) {}
            if(recycle) {
                bitmap.recycle();
            }
        }
        return null;
    }





    private class ImageAvailableListener implements ImageReader.OnImageAvailableListener {

        @Override
        public void onImageAvailable(ImageReader reader){
            Image image = reader.acquireLatestImage();
            FileOutputStream fos = null;

            if(image != null){
                final Image.Plane[] planes = image.getPlanes();
                final ByteBuffer buffer = planes[0].getBuffer();
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * mWidth;
                int w = mWidth+rowPadding/pixelStride;

                // Create Bitmap
                mBmp = Bitmap.createBitmap(w, mHeight, Bitmap.Config.ARGB_8888);
                mBmp.copyPixelsFromBuffer(buffer);
                //mBmp = bmp;

                if(!saved){
                    saved = true;
                    Thread t = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try{
                                try{
                                    saveScreen(mBmp);
                                    Log.d(TAG, "SAVED SCREEN");
                                }
                                catch (Exception e){
                                    e.printStackTrace();
                                }
                                finally {
                                    saved = false;
                                }
                            }
                            catch (Exception e){
                                e.printStackTrace();
                            }

                        }
                    });
                    t.start();
                }



                /*
                // Convert bitmap to byte[]
                Bitmap b = bmp;
                ByteArrayOutputStream output = new ByteArrayOutputStream(b.getByteCount());
                b.compress(Bitmap.CompressFormat.PNG, 100, output);
                byte[] imageBytes = output.toByteArray();
                String encodedImage = Base64.encodeToString(imageBytes, Base64.NO_WRAP);
                Log.d(TAG, "xXx" + encodedImage + "xXx");

                // Convert string to bitmap
                String encodedString = encodedImage;
                byte[] newImageBytes = Base64.decode(encodedString, Base64.NO_WRAP);
                mBmp = BitmapFactory.decodeByteArray(newImageBytes, 0, newImageBytes.length);
                */

                //saveScreen(encodedImage);



                /*
                String encodedString = "dd";
                byte[] newImageBytes = Base64.decode(encodedString, Base64.NO_WRAP);
                mBmp = BitmapFactory.decodeByteArray(newImageBytes, 0, newImageBytes.length);
                */

                /* Uncomment at will
                // Save bitmap as image on phone
                try{
                    fos = new FileOutputStream(store_directory + "/screencap" + images_produced + ".png");
                    mBmp.compress(CompressFormat.JPEG, 100, fos);
                    images_produced++;
                    Log.e(TAG, "captured image: " + images_produced);
                }
                catch (Exception e){

                }
                finally {
                    if (fos!=null) {
                        try {
                            fos.close();
                        } catch (IOException ioe) {
                            ioe.printStackTrace();
                        }
                    }
                }
                */

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mImageView.setImageBitmap(mBmp);
                    }
                });
            }

            // Close image in preparation to receive next image
            if(image != null){
                image.close();
            }

        }

    }
    //endregion


    ///////////////////////////////////////// MEDIA PROJECTION /////////////////////////////////////


    //region Media Projection
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE) {
            mMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);

            if (mMediaProjection != null) {
                store_directory = Environment.getExternalStorageDirectory().getAbsolutePath()+ "/MediaProjection/";
                File storeDirectory = new File(store_directory);
                if(!storeDirectory.exists()) {
                    storeDirectory.mkdirs();
                    if(!storeDirectory.isDirectory()) {
                        Log.e(TAG, "failed to create file storage directory.");
                        return;
                    }
                }


                DisplayMetrics metrics = getResources().getDisplayMetrics();
                int density = metrics.densityDpi;
                int flags = DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC; //DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC;
                Display display = getWindowManager().getDefaultDisplay();
                Point size = new Point();
                display.getSize(size);
                mWidth = size.x;
                mHeight = size.y;

                mImageReader = ImageReader.newInstance(mWidth, mHeight, PixelFormat.RGBA_8888 /*ImageFormat.JPEG*/, 2);
                mImageReader.setOnImageAvailableListener(new TestImageAvailableListener(), null);
                mVirtualDisplay = mMediaProjection.createVirtualDisplay("screencap", mWidth, mHeight, density, flags, mImageReader.getSurface(), mVirtualDisplayCallback, null);
            }
        }
    }

    private void startProjection() {
        startActivityForResult(mProjectionManager.createScreenCaptureIntent(), REQUEST_CODE);
    }

    private void stopProjection() {
        if (mMediaProjection != null) mMediaProjection.stop();
        if (mVirtualDisplay != null) mVirtualDisplay.release();
        mImageView.setImageResource(R.mipmap.ic_launcher);
    }
    //endregion





    ////////////////////////////////////////// UTILITY METHODS /////////////////////////////////////

    //region Utility Methods
    public int getIntFromColor(int Red, int Green, int Blue){
        Red = (Red << 16) & 0x00FF0000; //Shift red 16-bits and mask out other stuff
        Green = (Green << 8) & 0x0000FF00; //Shift Green 8-bits and mask out other stuff
        Blue = Blue & 0x000000FF; //Mask out anything not blue.

        return 0xFF000000 | Red | Green | Blue; //0xFF000000 for 100% Alpha. Bitwise OR everything together.
    }

    private ImageReader createImageReader(){
        DisplayMetrics metrics = getResources().getDisplayMetrics();
        int density = metrics.densityDpi;
        int flags = DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC; //DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC;
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        mWidth = size.x;
        mHeight = size.y;

        return ImageReader.newInstance(mWidth, mHeight, PixelFormat.RGB_565 /*ImageFormat.JPEG*/, 2);
    }

    private String getColourForInt(int pixel){
        int a = Color.alpha(pixel);
        int r = Color.red(pixel);
        int g = Color.green(pixel);
        int b = Color.blue(pixel);

        return String.format("#%02X%02X%02X%02X", a, r, g, b);
    }

    private void printReaderFormat(int format){
        if(format == ImageFormat.JPEG){
            Log.e(TAG, "JPEG");
        }
        else if(format == ImageFormat.NV16){
            Log.e(TAG, "NV16");
        }
        else if(format == ImageFormat.NV21){
            Log.e(TAG, "NV21");
        }
        else if(format == ImageFormat.RAW10){
            Log.e(TAG, "RAW10");
        }
        else if(format== ImageFormat.RAW_SENSOR){
            Log.e(TAG, "RAW_SENSOR");
        }
        else if(format == ImageFormat.RGB_565){
            Log.e(TAG, "RGB_565");
        }
        else if(format == ImageFormat.YUV_420_888){
            Log.e(TAG, "YUV_420_888");
        }
        else if(format == ImageFormat.YUY2){
            Log.e(TAG, "YUY2");
        }
        else if(format == ImageFormat.YV12){
            Log.e(TAG, "YV12");
        }
        else if(format == ImageFormat.UNKNOWN){
            Log.e(TAG, "UNKNOWN");
        }
        else if(format == PixelFormat.RGB_888){
            Log.e(TAG, "RGB_888");
        }
        else if(format == PixelFormat.RGB_565){
            Log.e(TAG, "PIXEL_RGB_565");
        }
        else if(format == PixelFormat.RGBA_8888){
            Log.e(TAG, "RGBA_8888");
        }
        else if(format == PixelFormat.RGBX_8888){
            Log.e(TAG, "RGBX_8888");
        }
        else if(format == PixelFormat.TRANSLUCENT){
            Log.e(TAG, "TRANSLUCENT");
        }
        else if(format == PixelFormat.TRANSPARENT){
            Log.e(TAG, "TRANSPARENT");
        }
        else if(format == PixelFormat.UNKNOWN){
            Log.e(TAG, "PIXEL_UNKNOWN");
        }
        else{
            Log.e(TAG, "THE FORMAT IS GIBBERISH");
        }
    }

    public static Bitmap getBitmapFromURL(String src) {
        try {
            URL url = new URL(src);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setDoInput(true);
            connection.connect();
            InputStream input = connection.getInputStream();
            Bitmap myBitmap = BitmapFactory.decodeStream(input);
            return myBitmap;
        } catch (IOException e) {
            // Log exception
            e.printStackTrace();
            Log.e(TAG, e.getMessage());
            return null;
        }
    }

    private void manuallyPushImageToImageReader(){
        mImageReader = createImageReader();
        mImageReader.setOnImageAvailableListener(new ImageReader.OnImageAvailableListener() {
            @Override
            public void onImageAvailable(ImageReader reader) {
                Log.d(TAG, "NEW IMAGE AVAILABLE");

                DisplayMetrics metrics = getResources().getDisplayMetrics();


                Image image = reader.acquireLatestImage();printReaderFormat(image.getFormat());Log.d(TAG, "IMAGE FORMAT: " + image.getFormat());
                final Image.Plane[] planes = image.getPlanes();
                final ByteBuffer buffer = planes[0].getBuffer();
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * mWidth;
                int w = mWidth+rowPadding/pixelStride;
                Bitmap bmp = Bitmap.createBitmap(mWidth, mHeight, Bitmap.Config.RGB_565);
                bmp.copyPixelsFromBuffer(buffer);
                mBmp = bmp;

                Thread t = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try{
                            saveScreen(mBmp);
                        }
                        catch (Exception e){
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

    /**
     * Get IP address from first non-localhost interface
     * @param useIPv4  true=return ipv4, false=return ipv6
     * @return  address or empty string
     */
    public static String getIPAddress(boolean useIPv4) {
        try {
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                List<InetAddress> addrs = Collections.list(intf.getInetAddresses());
                for (InetAddress addr : addrs) {
                    if (!addr.isLoopbackAddress()) {
                        String sAddr = addr.getHostAddress().toUpperCase();
                        boolean isIPv4 = InetAddressUtils.isIPv4Address(sAddr);
                        if (useIPv4) {
                            if (isIPv4)
                                return sAddr;
                        } else {
                            if (!isIPv4) {
                                int delim = sAddr.indexOf('%'); // drop ip6 port suffix
                                return delim<0 ? sAddr : sAddr.substring(0, delim);
                            }
                        }
                    }
                }
            }
        } catch (Exception ex) { } // for now eat exceptions
        return "";
    }
    //endregion
}
