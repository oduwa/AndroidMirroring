package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

import com.vnc.oeo.mediaprojectiontest1.R;

import org.apache.http.conn.util.InetAddressUtils;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URL;
import java.util.Collections;
import java.util.List;

import main.Networking.RFBClient;
import main.Networking.TCPClient;
import main.Networking.TCPServer;


public class ViewerActivity extends Activity {

    private static final String TAG = ViewerActivity.class.getName();

    private Activity mActivity;
    private ImageView mImageView;
    private boolean isViewing = false;
    private AndroidViewer viewer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_viewer);



        // Store Context
        mActivity = this;

        // Get Image View
        mImageView = (ImageView)findViewById(R.id.viewerImageView);

        // Create viewer
        viewer = new AndroidViewer();


        // Start Button
        final RFBClient rfbClient = new RFBClient();
        Button startViewerButton = (Button)findViewById(R.id.startViewerButton);
        startViewerButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                viewer.connect("10.10.19.228", 6881, new AndroidViewer.ViewerCallback() {
                    @Override
                    public void connectionResult(boolean success) {

                    }

                    @Override
                    public void didGetFrame(byte[] frame) {
                        Log.d(TAG, "LOADED NEW IMAGE");
                        final Bitmap bmp = BitmapFactory.decodeByteArray(frame, 0, frame.length);
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mImageView.setImageBitmap(bmp);
                            }
                        });
                    }
                });



//
//                isViewing = true;
//                //getScreen();
//                /*
//                Thread t = new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        TCPClient.start(new TCPClient.SocketImageCallback() {
//                            @Override
//                            public void didReceiveMessage(byte[] frameMessage) {
//                                final Bitmap bmp = BitmapFactory.decodeByteArray(frameMessage, 0, frameMessage.length);
//                                runOnUiThread(new Runnable() {
//                                    @Override
//                                    public void run() {
//                                        runOnUiThread(new Runnable() {
//                                            @Override
//                                            public void run() {
//                                                mImageView.setImageBitmap(bmp);
//                                            }
//                                        });
//                                    }
//                                });
//                            }
//                        });
//                    }
//                });
//                t.start();
//                */
//
//                Thread t = new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        String ip = "10.10.19.228";//"10.10.19.196";//"10.10.19.228"
//                        final boolean success = rfbClient.connect(ip, 6881);
//
//
//                        if(success){
//                            boolean processedFrameResponse = false;
//                            boolean shouldRequestNewFrame = true;
//                            Log.d(TAG, "SENT FRAME REQUEST MESSAGE");
//                            try {
//                                rfbClient.requestFrameBufferUpdate();
//                            } catch (IOException e) {
//                                e.printStackTrace();
//                            }
//                            while(isViewing){
//                                try {
////                                    if(shouldRequestNewFrame == true){
////                                        shouldRequestNewFrame = false;
////                                        Log.d(TAG, "SENT FRAME REQUEST MESSAGE");
////                                        rfbClient.requestFrameBufferUpdate();
////                                        //Thread.sleep(3000);
////                                    }
//
//                                    try {
//                                        /*
//                                        String x = rfbClient.receiveMessage();
//                                        System.out.println("GET SUMN: " + x);
//                                        if(x == null){
//                                            isViewing = true;
//                                        }
//                                        */
//                                        byte[] screen = rfbClient.getFrameBuffer();
//                                        if(screen == null){
//                                            shouldRequestNewFrame = true;
//                                            Log.d(TAG, "SENT FRAME REQUEST MESSAGE");
//                                            rfbClient.requestFrameBufferUpdate();
//                                        }
//                                        else{
//                                            Log.d(TAG, "LOADED NEW IMAGE");
//                                            final Bitmap bmp = BitmapFactory.decodeByteArray(screen, 0, screen.length);
//                                            shouldRequestNewFrame = true;
//                                            runOnUiThread(new Runnable() {
//                                                @Override
//                                                public void run() {
//                                                    mImageView.setImageBitmap(bmp);
//                                                }
//                                            });
//                                        }
//                                    } catch (IOException e) {
//                                        e.printStackTrace();
//                                    }
//
//
//
//                                } catch (Exception e) {
//                                    e.printStackTrace();
//                                }
//
//                            }




//                        }
//                        else{
//                            runOnUiThread(new Runnable() {
//                                @Override
//                                public void run() {
//                                    Toast.makeText(mActivity, "CONNECTION RESULT: "+success, Toast.LENGTH_LONG);
//                                }
//                            });
//                        }
//                    }
//                });
//                t.start();
            }
        });

        // Stop Button
        Button stopViewerButton = (Button)findViewById(R.id.stopViewerButton);
        stopViewerButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                viewer.pauseViewing();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_viewer, menu);
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
                    mImageView.setImageBitmap(result);
                }

                if(isViewing == true){
                    getScreen();
                }
            }
        });
        task.execute();
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
}
