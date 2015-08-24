package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import com.vnc.oeo.mediaprojectiontest1.R;

import java.io.ByteArrayOutputStream;

import main.Networking.TCPClient;
import main.Networking.TCPServer;
import main.VNCServerJNIWrapper.VNCAndroidServer;
import main.Bindings.Server;

import static main.java.com.vnc.oeo.mediaprojectiontest1.VNCUtility.getIntFromColor;


public class HomeActivity extends Activity {

    private static final String TAG = ServerActivity.class.getName();
    private Activity mActivity;
    private ImageView mImageView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);

        // Store Context
        mActivity = this;

        //VNCAndroidServer.start();




        // Server
        final VNCAndroidServer server = new VNCAndroidServer();

        // Image View
        mImageView = (ImageView) findViewById(R.id.tcpImageView);

        // Viewer Button
        Button startViewerButton = (Button)findViewById(R.id.viewerButton);
        startViewerButton.setText("Viewer"/*server.getStringFromJNI()*/);
        startViewerButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                //  Navigate to viewer activity
                Intent viewerIntent = new Intent(mActivity, ViewerActivity.class);
                viewerIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                //startActivity(viewerIntent);

//                byte[] bytes = new byte[50*50];
//                int[] ints = new int[50*50];
//
//                for (int i = 0; i < 50 * 50; i++) {
//                    ints[i] = Color.RED;
//                    bytes[i] = (byte) Color.RED;
//                }
//
//                //Bitmap bmp = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
//                Bitmap bmp = Bitmap.createBitmap(ints, 50, 50, Bitmap.Config.ARGB_8888);
//                //Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.drawable.image);
//                if (bmp == null)
//                    Log.e(TAG, "bitmap is null");
//                mImageView.setImageBitmap(bmp);
//                ByteArrayOutputStream stream = new ByteArrayOutputStream();
//                bmp.compress(Bitmap.CompressFormat.JPEG, 0, stream);
//                bytes = stream.toByteArray();
//                bytes = VNCUtility.bitmapToArray(bmp);
//                Log.d("PKKKKK", "(" + bmp.getWidth() + ", " + bmp.getHeight() + ")");
//                //bmp = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
//                //mImageView.setImageBitmap(bmp);
//
//                String encoded = Base64.encodeToString(bytes, Base64.NO_WRAP | Base64.NO_PADDING);
//                //server.newScreenAvailable(null);
//                server.newFrameAvailable(bytes, /*184 * 768*/bytes.length);
//                String s = "A TEXT MESSAGE";
//                server.sendTextMessage(encoded, encoded.length());

                VNCAndroidServer.startSharedMemServer();

            }
        });

        // Server Button
        Button startServerButton = (Button)findViewById(R.id.serverButton);
        startServerButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                //  Navigate to server activity
                Intent serverIntent = new Intent(mActivity, ServerActivity.class);
                serverIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(serverIntent);
                //VNCAndroidServer.startSharedMemClient();
            }
        });


//        final byte[] data = new byte[1184 * 768];
//        final int[] ints = new int[1184 * 768];
//
//        for (int i = 0; i < 1184 * 768; i++) {
//            data[i] = (byte) Color.RED;
//            ints[i] = data[i];//Color.RED; //0x00FF0000;
//        }

        //Bitmap bmp = BitmapFactory.decodeByteArray(ints, 0, ints.length);
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                //Bitmap bmp = Bitmap.createBitmap(ints, 768, 1184, Bitmap.Config.ARGB_8888);
//                final Bitmap bmp = VNCUtility.getBitmapFromURL("http://images.fashionmodeldirectory.com/images/agencies/q-model-management-los-angeles-vector-11-alternative.png");
//                ByteArrayOutputStream stream = new ByteArrayOutputStream();
//                bmp.compress(Bitmap.CompressFormat.PNG, 100, stream);
//                byte[] byteArray = stream.toByteArray();
//                final Bitmap bmp2 = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
//                runOnUiThread(new Runnable() {
//                    @Override
//                    public void run() {
//                        mImageView.setImageBitmap(bmp2);
//                    }
//                });
                Log.d("PKK", VNCUtility.getIPAddress(true));
                Log.d(TAG, "T2 STARTED");
                VNCAndroidServer.newScreenAvailable2(null, 0);
            }
        });
        t.start();





    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_home, menu);
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

    // tell java which library to load

}
