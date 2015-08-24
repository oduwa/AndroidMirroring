package main.Networking;

// TCPClient.java
// A client program implementing TCP socket
import android.graphics.Color;

import java.net.*;
import java.io.*;

import main.java.com.vnc.oeo.mediaprojectiontest1.VNCUtility;

public class TCPClient {

    public interface SocketImageCallback {
        void didReceiveMessage(byte[] frameMessage);
    }

    static boolean shouldFetch = true;


    public static void start(SocketImageCallback socketImageCallback) {
        try {
            int serverPort = 6880;
            ServerSocket listenSocket = new ServerSocket(serverPort);

            System.out.println("server start listening... ... ...");

            while (true) {
                Socket clientSocket = listenSocket.accept();
                if(shouldFetch == true){
                    Connection c = new Connection(clientSocket, socketImageCallback);
                }
            }
        } catch (IOException e) {
            System.out.println("Listen :" + e.getMessage());
        }
    }
}

class Connection extends Thread {

    DataInputStream input;
    DataOutputStream output;
    Socket clientSocket;
    TCPClient.SocketImageCallback mImageCallback;

    public Connection(Socket aClientSocket, TCPClient.SocketImageCallback socketImageCallback) {
        try {
            clientSocket = aClientSocket;
            input = new DataInputStream(clientSocket.getInputStream());
            output = new DataOutputStream(clientSocket.getOutputStream());
            mImageCallback = socketImageCallback;
            this.start();
        } catch (IOException e) {
            System.out.println("Connection:" + e.getMessage());
        }
    }

    public void run() {
        try { // an echo server
            //  String data = input.readUTF();

            //FileWriter out = new FileWriter("test.txt");
            //BufferedWriter bufWriter = new BufferedWriter(out);

            TCPClient.shouldFetch = false;

            //Step 1 read length
            int length = input.readInt();
            System.out.println("Read Length" + length);
            byte[] digit = new byte[length];
            //int[] ints = new int[length];

            //Step 2 read byte
            System.out.println("Reading.......");
//            for (int i = 0; i < length; i++) {
//                digit[i] = input.readByte();
//                //ints[i] = (int) input.readByte();
//            }
            input.readFully(digit);
            System.out.println("Read. Starting callback");
            mImageCallback.didReceiveMessage(digit);
            System.out.println("Callback done");

            //System.out.println("Read bitmap with colour " + ints[0]);

            /*
            String st = new String(digit);
            //bufWriter.append(st);
            //bufWriter.close();
            System.out.println("receive from : "
                    + clientSocket.getInetAddress() + ":"
                    + clientSocket.getPort() + " message - " + st);

            //Step 1 send length
            output.writeInt(st.length());
            //Step 2 send length
            output.writeBytes(st); // UTF is a string encoding
            //  output.writeUTF(data);
            */
        } catch (EOFException e) {
            System.out.println("EOF:" + e.getMessage());
        } catch (IOException e) {
            System.out.println("IO:" + e.getMessage());
        } finally {
            try {
                clientSocket.close();
            } catch (IOException e) {/*close failed*/

            }

            TCPClient.shouldFetch = true;
        }
    }
}
