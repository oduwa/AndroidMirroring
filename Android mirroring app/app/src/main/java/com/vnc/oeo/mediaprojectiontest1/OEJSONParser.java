package main.java.com.vnc.oeo.mediaprojectiontest1;

import android.os.AsyncTask;

import org.json.JSONObject;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by Odie on 20/02/15.
 */
public class OEJSONParser {

    private static final String TAG = OEJSONParser.class.getSimpleName();

    private String urlString;
    public volatile boolean parsingComplete = true;
    public JSONObject jsonResult;

    public OEJSONParser(String url) {
        urlString = url;
    }



    /**
     * Asynchronously parse JSON and execute some specified block of code once parsing is complete.2
     * @param callback block of code to be called once parsing is complete.
     */
    public void fetchJSON(JSONTask.JSONCallback callback) {
        JSONTask task = new JSONTask(urlString, callback);
        task.execute();
    }


    /**
     * Convert binary stream data to a String representation.
     * @param inputStream binary stream data to be converted to a String representation.
     * @return String representation of binary stream data.
     */
    private String getStringForJSONData(InputStream inputStream){
        String result = "";

        try {
            BufferedReader input = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"));
            StringBuilder strB = new StringBuilder();
            String str;
            while (null != (str = input.readLine())) {
                strB.append(str).append("\r\n");
            }
            input.close();
            result = strB.toString();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return result;
    }

    /**
     * Replaces html escape characters wih their natural values.
     * @param string string containing html to unescape.
     * @return unescaped string.
     */
    private String removeHTMLEntities(String string){
        return  string.replaceAll("&.{0,}?;", "");
    }



    /**************************** Async Tasks *********************************/

    /**
     * AsyncTask used to asynchronously fetch and parse an json file.
     */
    public static class JSONTask extends AsyncTask<String, Void, JSONObject> {

        // Callback to allow user to specify what happens once task completes
        public interface JSONCallback {
            void onComplete(JSONObject result);
        }
        private JSONCallback delegate;
        private String urlString;

        // AsyncTask Constructor
        JSONTask(String URLString, JSONCallback callback) {
            this.delegate = callback;
            this.urlString = URLString;
        }

        // Work to be done in the background ie. json parsing.
        @Override
        protected JSONObject doInBackground(String... params) {
            try {
                URL url = new URL(urlString);
                HttpURLConnection conn = (HttpURLConnection)
                        url.openConnection();
                conn.setReadTimeout(20000 /* milliseconds */);
                conn.setConnectTimeout(5000 /* milliseconds */);
                conn.setRequestMethod("GET");
                conn.setDoInput(true);
                conn.connect();
                InputStream stream = conn.getInputStream();

                // Get JSON string and escape it
                String jsonString = getStringForJSONData(stream);
                String escapedJSONString = removeHTMLEntities(jsonString);

                if(escapedJSONString.equalsIgnoreCase("No results")){
                    return null;
                }

                char g = escapedJSONString.charAt(escapedJSONString.length()-1);
                if(g == '\n'){
                    escapedJSONString = escapedJSONString.substring(0,escapedJSONString.length()-2);
                    escapedJSONString = "{ array = "  + escapedJSONString + "}";
                }

                stream.close();
                JSONObject jsonResult = new JSONObject(escapedJSONString);

                return jsonResult;

            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        }

        // code to be fired once background task completes. callbacks are called here
        @Override
        protected void onPostExecute(JSONObject result) {
            super.onPostExecute(result);
            delegate.onComplete(result);
        }


        /** HELPERS **/

        /**
         * Convert binary stream data to a String representation.
         * @param inputStream binary stream data to be converted to a String representation.
         * @return String representation of binary stream data.
         */
        private String getStringForJSONData(InputStream inputStream){
            String result = "";

            try {
                BufferedReader input = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"));
                StringBuilder strB = new StringBuilder();
                String str;
                while (null != (str = input.readLine())) {
                    strB.append(str).append("\r\n");
                }
                input.close();
                result = strB.toString();
            } catch (IOException e) {
                e.printStackTrace();
            }

            return result;
        }


        /**
         * Replaces html escape characters wih their natural values.
         * @param string string containing html to unescape.
         * @return unescaped string.
         */
        private String removeHTMLEntities(String string){
            return  string.replaceAll("&.{0,}?;", "");
        }

    }



}
