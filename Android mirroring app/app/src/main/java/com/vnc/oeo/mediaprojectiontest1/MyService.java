package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.app.ActivityManager;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;

import org.apache.http.conn.util.InetAddressUtils;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URL;
import java.util.Collections;
import java.util.List;

import main.Networking.RFBServer;
import main.Networking.TCPServer;
import main.VNCServerJNIWrapper.VNCAndroidServer;

/**
 * Created by oeo on 31/07/2015.
 */
public class MyService extends Service {

    public static final String TAG = "OdiesAmazingService";
    public boolean saved = false;
    private SaveScreenTask task;
    Bitmap bitmap;
    public static Service mService;
    public static RFBServer sharedServer;


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        //Log.d(TAG, "SERVICE CALLED");

        mService = this;

        if(sharedServer == null){
            sharedServer = RFBServer.getSharedInstance();
        }


        if(isApplicationBroughtToBackground() == true){
            saved = false;
        }

        if (!saved /*|| task == null*/) {
            saved = true;
            /*
            Thread t = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Log.d(TAG, "SERVICE CALLED");
                    } catch (Exception e) {
                        e.printStackTrace();
                        Log.d(TAG, "IMAGE SAVE FAILED!!!");
                    } finally {
                        saved = false;
                    }
                    //Log.e(TAG, "GOT EEM: " + ++images_produced);
                }
            });
            t.start();
            */

//            String photoPath = intent.getStringExtra("photoPath");
//            Log.d(TAG, photoPath);
//            BitmapFactory.Options options = new BitmapFactory.Options();
//            options.inPreferredConfig = Bitmap.Config.ARGB_8888;
//            bitmap = BitmapFactory.decodeFile(photoPath, options);
//
//            if(bitmap == null){
//                Log.e(TAG, "BITMAP TO SAVE IS NULL");
//                return Service.START_REDELIVER_INTENT;
//            }

            /*
            task = new SaveScreenTask(null, bitmap, new SaveScreenTask.TaskCallback() {
                @Override
                public void onComplete(String result) {
                    Log.d(TAG, result);
                    saved = false;
                    synchronized (this){
                        bitmap.recycle();
                    }
                }
            });
            task.execute();
            saved = true;
            */

            //Log.d(TAG, "SENDING VIA TCP");

            Thread t = new Thread(new Runnable() {
                @Override
                public void run() {
                    synchronized (mService){
                        if(ServerActivity.mBmp == null){
                            //Log.d(TAG, "NUUUULLLLLLL!!!");
                        }
                        else{
                            //Log.d(TAG, "COOOL!!!!!");
                        }
                        try{
//                            if(rfbServer.updatePending){
//                                rfbServer.updatePending = false;
//                                ByteArrayOutputStream stream = new ByteArrayOutputStream();
//                                ServerActivity.mBmp.compress(Bitmap.CompressFormat.JPEG, 0, stream);
//                                byte[] byteArray = stream.toByteArray();
//                                rfbServer.screenPixels = byteArray;
//                                rfbServer.sendFrameBufferUpdate(byteArray);
//                                //rfbServer.send(byteArray);
//                            }

                            if(!sharedServer.updateInProgress ||  sharedServer.screenPixels == null){

                            }

                            synchronized (mService){

//                                ByteArrayOutputStream stream = new ByteArrayOutputStream();
//                                ServerActivity.mBmp.compress(Bitmap.CompressFormat.JPEG, 0, stream);
//                                byte[] byteArray = stream.toByteArray();
//                                sharedServer.screenPixels = byteArray;

                                //byte[] byteArray = VNCUtility.bitmapToArray(AndroidServer.bitmap);
                                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                                AndroidServer.bitmap.compress(Bitmap.CompressFormat.JPEG, 70, stream);
                                byte[] byteArray = stream.toByteArray();
                                //Log.d("PKKKKK", "(" + AndroidServer.bitmap.getWidth() + ", " + AndroidServer.bitmap.getHeight() + ")");
                                VNCAndroidServer.setNewScreen(byteArray, byteArray.length);


                                //sharedServer.screenPixels = ServerActivity.sharedByteArray;
                            }


                            //TCPServer.start(byteArray);
                            //if(rfbServer.updatePending){

                            //}
                            saved = false;
                        }
                        catch(Exception e){
                            Log.e(TAG, e.getMessage());
                            //e.printStackTrace();
                            saved = false;
                        }
                    }
                }
            });
            t.start();
        }


        return Service.START_REDELIVER_INTENT;
    }

    public static String getCacheDir(Context ctx) {
        return Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()) ||!Environment.isExternalStorageRemovable() ?
                ctx.getExternalCacheDir().getPath() : ctx.getCacheDir().getPath();
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    /**
     * AsyncTask used to asynchronously fetch and parse an json file.
     */
    public static class SaveScreenTask extends AsyncTask<String, Void, String> {

        // Callback to allow user to specify what happens once task completes
        public interface TaskCallback {
            void onComplete(String result);
        }
        private TaskCallback delegate;
        private String urlString;
        private Bitmap bitmap;

        // AsyncTask Constructor
        SaveScreenTask(String URLString, Bitmap bitmap, TaskCallback callback) {
            this.delegate = callback;
            this.urlString = URLString;
            this.bitmap = bitmap;
        }

        // Work to be done in the background ie. json parsing.
        @Override
        protected String doInBackground(String... params) {
            try {
                return saveScreen(bitmap);

            } catch (Exception e) {
                e.printStackTrace();
                return "EXCEPTION HAPPENED SAVING SCREEN";
            }
        }

        // code to be fired once background task completes. callbacks are called here
        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);
            delegate.onComplete(result);
        }

        // HELPERS
        public String saveScreen(Bitmap bitmap) throws Exception{
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

            return response;
        }

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
    }

    private boolean isApplicationBroughtToBackground() {
        ActivityManager am = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningTaskInfo> tasks = am.getRunningTasks(1);
        if (!tasks.isEmpty()) {
            ComponentName topActivity = tasks.get(0).topActivity;
            if (!topActivity.getPackageName().equals(getPackageName())) {
                return true;
            }
        }

        return false;
    }

    static {
        System.loadLibrary("JNIDummy");
    }
}
