package main.Networking;

// TCPServer.java
// A server program implementing TCP socket
import android.graphics.Color;

import java.net.*;
import java.io.*;

public class TCPServer {

    public static void start(byte[] d) {// arguments supply message and hostname of destination
        Socket s = null;
        try {
            //htc 10.10.19.196
            int serverPort = 6880;
            String ip = "10.10.19.196"; //"10.0.2.2";//"192.168.5.5";//"10.10.19.228"; //"localhost";
            //String data = "Hello, How are you?";
//            byte[] data;
//            byte[] ints = new byte[1184 * 768];
//
//            for (int i = 0; i < 1184 * 768; i++) {
//                ints[i] = (byte) Color.RED; //0x00FF0000;
//            }
//            data = ints;

            s = new Socket(ip, serverPort);
            DataInputStream input = new DataInputStream(s.getInputStream());
            DataOutputStream output = new DataOutputStream(s.getOutputStream());

            //Step 1 send length
            System.out.println("Length" + d.length);
            output.writeInt(d.length);
            //Step 2 send length
            System.out.println("Writing.......");
            //output.writeBytes(data); // UTF is a string encoding
            output.write(d);
            System.out.println("Written");

            /*
            //Step 1 read length
            int nb = input.readInt();
            byte[] digit = new byte[nb];

            //Step 2 read byte
            for (int i = 0; i < nb; i++) {
                digit[i] = input.readByte();
            }

            String st = new String(digit);
            System.out.println("Received: " + st);
            */
        } catch (UnknownHostException e) {
            System.out.println("Sock:" + e.getMessage());
        } catch (EOFException e) {
            System.out.println("EOF:" + e.getMessage());
        } catch (IOException e) {
            System.out.println("IO:" + e.getMessage());
        } finally {
            if (s != null) {
                try {
                    s.close();
                } catch (IOException e) {/*close failed*/

                }
            }
        }
    }
}