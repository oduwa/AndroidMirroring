package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.hardware.display.DisplayManager;
import android.media.ImageReader;
import android.os.Build;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;

import org.apache.http.conn.util.InetAddressUtils;

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

/**
 * Created by oeo on 31/07/2015.
 */
public class VNCUtility {

    public static final String TAG = VNCUtility.class.getName();

    public static int getIntFromColor(int Red, int Green, int Blue){
        Red = (Red << 16) & 0x00FF0000; //Shift red 16-bits and mask out other stuff
        Green = (Green << 8) & 0x0000FF00; //Shift Green 8-bits and mask out other stuff
        Blue = Blue & 0x000000FF; //Mask out anything not blue.

        return 0xFF000000 | Red | Green | Blue; //0xFF000000 for 100% Alpha. Bitwise OR everything together.
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static ImageReader createImageReader(Activity c){
        DisplayMetrics metrics = c.getResources().getDisplayMetrics();
        int density = metrics.densityDpi;
        int flags = DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC; //DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY | DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC;
        Display display = c.getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);

        return ImageReader.newInstance(size.x, size.y, PixelFormat.RGB_565, 2);
    }

    public static String getColourForInt(int pixel){
        int a = Color.alpha(pixel);
        int r = Color.red(pixel);
        int g = Color.green(pixel);
        int b = Color.blue(pixel);

        return String.format("#%02X%02X%02X%02X", a, r, g, b);
    }

    public static void printReaderFormat(int format){
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

    public static byte[] bitmapToArray(Bitmap b){
        int bytes = b.getByteCount();

        ByteBuffer buffer = ByteBuffer.allocate(bytes); //Create a new buffer
        b.copyPixelsToBuffer(buffer); //Move the byte data to the buffer

        byte[] array = buffer.array(); //Get the underlying array containing the data.

        return  array;
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

    public static File saveBitmapToFile(Bitmap bitmap, String filename, String path, boolean recycle) {
        FileOutputStream out=null;
        try {
            File f = new File(path,filename);
            if(!f.exists()) {
                f.createNewFile();
            }
            out = new FileOutputStream(f);
            if(bitmap.compress(Bitmap.CompressFormat.PNG, 100, out)) {
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

}
