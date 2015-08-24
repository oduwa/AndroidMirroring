/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package main.Networking;

import android.graphics.Bitmap;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

import main.java.com.vnc.oeo.mediaprojectiontest1.ServerActivity;
import main.java.com.vnc.oeo.mediaprojectiontest1.VNCUtility;

/**
 *
 * @author Odie
 */
public class RFBServer {

    private static final String FRAME_BUFFER_REQUEST_MESSAGE = "FrameBufferRequest";
    private static final String CLOSE_CONNECTION_REQUEST_MESSAGE = "CloseConnection";
    public static RFBServer singleton = null;

    ServerSocket listenSocket;
    public DataInputStream input;
    public DataOutputStream output;
    public Socket clientSocket;
    ArrayList<String> connectedClients;
    boolean shouldListenForMessages;
    Thread listenThread;
    public byte[] screenPixels;
    public boolean updateInProgress;

    /**
     * Method for retrieving a singleton instance of the RFBServer class.
     * @return singleton instance of the RFBServer class.
     */
    public static RFBServer getSharedInstance(){
        if(singleton == null){
            singleton = new RFBServer();
            return singleton;
        }
        else{
            return singleton;
        }
    }

    /**
     * Default Cnstructor.
     *
     * Creates and initializes a server object.
     */
    public RFBServer(){
        this.connectedClients = new ArrayList<>();
        this.shouldListenForMessages = false;
    }

    /**
     * Constructor.
     *
     * Creates and initializes a server object and begins listening for connections on the
     * specified port.
     *
     * @param port number of port to listen to connections on.
     */
    public RFBServer(int port){
        this.connectedClients = new ArrayList<>();
        connect(port);
    }

    /**
     * Listen for connections on a specified port.
     * @param port number of port to listen to connections on.
     * @return TRUE if a successful connection is made.
     */
    public boolean connect(int port){
        try {
            // Create a socket to listen to connections on
            listenSocket = new ServerSocket(port);

            // Start listening to connections. Note that this is a blocking operation.
            System.out.println("server start listening... ... ...");
            clientSocket = listenSocket.accept();

            // Create io streams for communication if a connection is made.
            input = new DataInputStream(clientSocket.getInputStream());
            output = new DataOutputStream(clientSocket.getOutputStream());

            // Perform handshake as per RFB protocol
            return performHandshake();

        } catch(Exception e){
            e.printStackTrace();
            return false;
        }
    }

    /**
     * Send a chunk of data to the client.
     *
     * @param data Data to be sent to the client.
     *
     * @throws IOException
     */
    public void send(byte[] data) throws IOException {
        output.writeInt(data.length);
        output.write(data);
    }


    /**
     * Calling this method causes the server to listen and respond to messages from the client(s).
     *
     * Note that this is only possible after successfully connecting to a client with a successful
     * handshake process.
     *
     * @throws IOException
     */
    public void startListeningForMessages() throws IOException {
        shouldListenForMessages = true;

        // Create thread to listen on, so as to not block.
        listenThread = new Thread(new Runnable() {

            @Override
            public void run() {
                // listen loop
                while(shouldListenForMessages){
                    try {
                        // Check if message available
                        if(input.available() > 0){
                            // Get message
                            String msg = receiveMessage();
                            System.out.println("RECEIVED MSG FROM CLIENT: " + msg);

                            // Respond to message
                            if(msg.equalsIgnoreCase(FRAME_BUFFER_REQUEST_MESSAGE)){
                                // Send frame buffer update if pixels available
                                if(RFBServer.singleton.screenPixels != null){
                                    System.out.println("SENDING FRAME BUFFER UPDATE");
                                    updateInProgress = true;
                                    sendFrameBufferUpdate();
                                    updateInProgress = false;
                                }
                            }
                            else if(msg.equalsIgnoreCase(CLOSE_CONNECTION_REQUEST_MESSAGE)){
                                // Close connection
                                shouldListenForMessages = false;
                                clientSocket.close();
                            }

                        }
                    } catch(Exception e){
                        e.printStackTrace();
                    }
                }
            }
        });

        listenThread.start();
    }


    /**
     * Stop listening for messages from client(s).
     */
    public void stopListeningForMessages(){
        shouldListenForMessages = false;
        listenThread = null;
    }


    /**
     * Perform handshake as per RFB protocol.
     *
     * First, the server sends the client what FB version it is running, then the client replies
     * with its own version. The server then replies with an authentication code, which
     * determines what kind of authentication (if any), is to be used.
     *
     * @return authenticationCode
     *
     * @throws IOException
     */
    private boolean performHandshake() throws IOException {
        // Send protocol version to client
        sendServerProtocolVersionMessage();

        // Get reply from client containing client's requested version
        String clientVersion = receiveClientProtocolVersionMessage();
        System.out.println(clientVersion);

        // Get major version number to check if this supports it.
        String majorVersionNumber = clientVersion.split(" ")[1].split("\\.")[0];
        if (Integer.parseInt(majorVersionNumber) != 3) {
            System.out.println("CLIENT NOT USING VERSION 3. SENDING CONNECTION FAILED");
            output.write(0);
            
            String reason = "CONNECTION FAILED: Client version must be RFB 3.xx";
            output.write(reason.length());
            output.writeBytes(reason);

            return false;
        }
        else{
            output.write(1);
        }

        // Read ClientInit message ie. get sharedFlag
        boolean clientInitMsg = input.readBoolean();

        // Send ServerInit message. Contains details about server display
        sendServerInitMessage(320, 480, new PixelFormat(), "MyAwesomeScreen");

        return true;
    }


    /**
     * Sends the RFB protocol version this server is based on.
     */
    private void sendServerProtocolVersionMessage(){
        System.out.println("SENDING SERVER PROTOCOL MESSAGE");
        try {
            String protocolVersion = "RFB 003.003\n";
            this.output.write(protocolVersion.length());
            this.output.writeBytes(protocolVersion);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }


    /**
     * Reads a message containing the version of RFB the calling client is based on, in reply
     * to the server's version message.
     *
     * @return The RFB version that the client to connect to is based on.
     */
    private String receiveClientProtocolVersionMessage(){
        System.out.println("RECEIVING SERVER PROTOCOL MESSAGE");
        
        byte[] clientVersion;
        try {
            clientVersion = new byte[input.read()];
            input.readFully(clientVersion);
            return new String(clientVersion);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        
        return null;  
    }


    /**
     * Send ServerInit message to a client.
     *
     * The ServerInit message contains information about the server's display.
     *
     * @param w width of frame.
     * @param h height of frame.
     * @param format pixel format of server display.
     * @param name name of server display.
     *
     * @throws IOException
     */
    private void sendServerInitMessage(int w, int h, PixelFormat format, String name) throws IOException{
        
        // Write width
        output.write(w);
        
        // Write height
        output.write(h);
        
        // Write PixelFormat
        output.write(format.bitsPerPixel);
        output.write(format.depth);
        output.writeBoolean(format.isBigEndian);
        output.writeBoolean(format.isTrueColour);
        output.write(format.redMax);
        output.write(format.greenMax);
        output.write(format.blueMax);
        output.write(format.redShift);
        output.write(format.greenShift);
        output.write(format.blueShift);
        byte[] pixelFormatPadding = new byte[3];
        output.write(pixelFormatPadding); // padding of 3 bytes
        
        // Write name length
        output.write(name.length());
        
        // Write name
        output.writeBytes(name);
    }


    /**
     * Send a textual message to a client.
     *
     * @param msg a textual message to be sent to a client.
     *
     * @throws IOException
     */
    public void sendMessage(String msg) throws IOException{
        output.write(msg.length());
        output.writeBytes(msg);
    }


    /**
     * Receive message from a client.
     *
     * @return message sent to the server by the client.
     *
     * @throws IOException
     */
    public String receiveMessage() throws IOException{
        try{
            if(input.available() > 0){
                // Get message code
                int msgCode = input.read();

                // Depending on message code, process message differently
                if(msgCode == 0){
                    // General message
                    byte[] msg = new byte[input.read()];
                    input.readFully(msg);
                    return new String(msg);
                }
                else if(msgCode == 1){
                    return FRAME_BUFFER_REQUEST_MESSAGE;
                }
                else if(msgCode == 2){
                    return CLOSE_CONNECTION_REQUEST_MESSAGE;
                }
                else{
                    return null;
                }
            }
            else{
                return null;
            }
        }
        catch(Exception e){
            e.printStackTrace();
            return null;
        }

    }


    /**
     * Send the pixel data of the server's display to a client.
     *
     * @throws IOException
     */
    private void sendFrameBufferUpdate() throws IOException {
        output.writeInt(screenPixels.length);
        output.write(screenPixels);
    }

}


