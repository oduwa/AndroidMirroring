/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package main.Networking;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import static java.lang.Thread.sleep;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Odie
 */
public class RFBClient {

    private class MessageCode {
        public static final int GENERAL_MESSAGE = 0;
        public static final int FRAME_BUFFER_REQUEST_MESSAGE = 1;
        public static final int CLOSE_CONNECTION_MESSAGE = 2;
    }

    public Socket socket;
    public DataInputStream input;
    public DataOutputStream output;
    PixelFormat serverPixelFormat;
    String serverName;
    int width;
    int height;
    static String protocolVersion = "RFB 003.004\n";
    byte[] screen;



    public RFBClient(){}
    public RFBClient(String ip, int port) {
        connect(ip, port);
    }

    /**
     * Create a connection to another device.
     *
     * @param ip IP Address of device.
     * @param port number of port to create connection on.
     *
     * @return TRUE if connection was successfully created.
     */
    public boolean connect(String ip, int port){
        try {

            // Create socket and io streams for communication
            socket = new Socket(ip, port);
            input = new DataInputStream(socket.getInputStream());
            output = new DataOutputStream(socket.getOutputStream());


            // Perform handshake as per RFB protocol
            int authCode = performHandshakeWithServer();

            // Connection failed
            if(authCode == 0){
                System.out.println(getConnectionFailedReason());
                return false;
            }

            // Server does not require authentication
            if(authCode == 1){
                System.out.println("AUTHENTICATION NOT REQUIRED");
            }

            // Authentication is not supported at the moment.

            // Client Init
            System.out.println("CLIENT INIT");
            sendClientInitMessage(true);

            // Server Init
            System.out.println("SERVER INIT");
            readServerInitMessage();

            System.out.println("SUCCESSFULLY CONNECTED TO SERVER WITH NAME: " + serverName);
            return true;

        } catch (UnknownHostException e) {
            System.out.println("Sock:" + e.getMessage());
            return false;
        } catch (EOFException e) {
            System.out.println("EOF:" + e.getMessage());
            return false;
        } catch (IOException e) {
            System.out.println("IO:" + e.getMessage());
            return false;
        }

    }


    /**
     * Perform handshake as per RFB protocol.
     *
     * First, the server sends the client what FB version it is running, then the client replies
     * with its own version. The server then replies with an authentication code, which
     * determines what kind of authentication (if any), is to be used.
     *
     * @return authenticationCode code
     * @throws IOException
     */
    private int performHandshakeWithServer() throws IOException {
        String serverVersion = receiveServerProtocolVersionMessage();
        System.out.println(serverVersion);

        sendClientProtocolVersionMessage();
        
        int authCode = input.read();
        return authCode;
    }


    /**
     * If the handshake process fails, this method returns the reason for the failure provided by
     * the server.
     *
     * @return reason The reason for the connection failure (provided by the server).
     * @throws IOException
     */
    private String getConnectionFailedReason() throws IOException{
        byte[] reason;
        reason = new byte[input.read()];
        input.readFully(reason);
        return new String(reason);
    }


    /**
     * Sends a server the version of RFB the calling client wants to run in reply to the servers
     * version message.
     */
    private void sendClientProtocolVersionMessage(){
        System.out.println("REPLYING TO PROTOCOL MESSAGE");
        try {
            this.output.write(protocolVersion.length());
            this.output.writeBytes(protocolVersion);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }


    /**
     * Fetches the RFB version that the server to connect to supports.
     *
     * @return version The RFB version that the server to connect to is based on.
     */
    private String receiveServerProtocolVersionMessage(){
        System.out.println("RECEIVING SERVER PROTOCOL MESSAGE");
        
        byte[] serverVersion;
        try {
            serverVersion = new byte[input.read()];
            input.readFully(serverVersion);
            return new String(serverVersion);
        } catch (IOException ex) {
            Logger.getLogger(RFBClient.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return null;  
    }


    /**
     * Sends ClientInit message to the server.
     *
     * The ClientInit message contains a boolean flag which denotes whether the client wants the
     * server to only connect to one client, or whether it may have multiple clients.
     *
     * @param sharedFlag boolean flag which denotes whether the client wants the
     * server to only connect to one client, or whether it may have multiple clients.
     * @throws IOException
     */
    private void sendClientInitMessage(boolean sharedFlag) throws IOException{
        
        // write share flag
        output.writeBoolean(sharedFlag);

    }


    /**
     * Fetches ServerInit message.
     *
     * After receiving the ClientInit message, the server sends a ServerInit message. This tells
     * the client the width and height of the server’s framebuffer, its pixel format and the name
     * associated with its display.
     *
     * @throws IOException
     */
    private void readServerInitMessage() throws IOException{
        // Get width and height of the server display
        width = input.read();
        height = input.read();

        // Get pixel format of server display
        int bpp = input.read();
        int depth = input.read();
        boolean isBigeEndian = input.readBoolean();
        boolean isTrueColour = input.readBoolean();
        int rMax = input.read();
        int gMax = input.read();
        int bMax = input.read();
        int rShift = input.read();
        int gShift = input.read();
        int bShift = input.read();
        serverPixelFormat = new PixelFormat(bpp, depth, isBigeEndian, isTrueColour,
        rMax, gMax, bMax, rShift, gShift, bShift);
        
        // Remove padding
        input.read();input.read();input.read();

        // Get name of server display
        byte[] serverNameBytes = new byte[input.read()];
        input.readFully(serverNameBytes);
        serverName = new String(serverNameBytes);
    }


    /**
     * Notifies the server that it is interested in capturing the screen data of the server.
     *
     * @throws IOException
     */
    public void requestFrameBufferUpdate() throws IOException {
        output.write(1);
    }


    /**
     * Gets the pixels of the server display.
     *
     * Must first call requestFrameBufferUpdate().
     *
     * @return updated frame buffer
     */
    public byte[] getFrameBuffer() throws IOException {
        // Check that server has started sending frame. (It does not necessarily send when
        // the client requests it)
        if(input.available() > 0){

            // Do nothing until server has finished sending frame
            int size = input.readInt();
            System.out.println("FRAME SIZE: " + size);
            while(input.available() < size){
                // do nothing
            }

            // Get received frame
            screen = new byte[size];
            input.readFully(screen);
            return screen;
        }
        else{
            return null;
        }
    }


    /**
     * Sends a message to the server.
     *
     * @param msg The message to be sent to the server.
     *
     * @throws IOException
     */
    public void sendRequest(String msg) throws IOException{
        output.write(MessageCode.GENERAL_MESSAGE);
        output.write(msg.length());
        output.writeBytes(msg);
    }

    /**
     * Sends a message to the server, along with a code that specifies how the server should deal
     * with the message.
     *
     * @param msg The message to be sent to the server.
     * @param msgCode a code that specifies how the server should deal
     * with the message. See RFBClient.MessageCode for more info.
     *
     * @throws IOException
     */
    private void sendRequest(String msg, int msgCode) throws IOException{
        output.write(msgCode);
        output.write(msg.length());
        output.writeBytes(msg);
    }


    /**
     * Fetches a textual message from the server.
     *
     * @return msg A textual message from the server.
     *
     * @throws IOException
     */
    public String receiveMessage() throws IOException{
        if(input.available() > 0){
            byte[] msg = new byte[input.read()];
            input.readFully(msg);
            return new String(msg);
        }
        else{
            return null;
        }
    }


    /**
     * Close connection to server.
     *
     * @throws IOException
     */
    public void closeConnection() throws IOException{
        sendRequest("CLOSE", 2);
        socket.close();
    }


    //region Getters and Setters
    public Socket getSocket() {
        return socket;
    }

    public void setSocket(Socket socket) {
        this.socket = socket;
    }

    public DataInputStream getInput() {
        return input;
    }

    public void setInput(DataInputStream input) {
        this.input = input;
    }

    public DataOutputStream getOutput() {
        return output;
    }

    public void setOutput(DataOutputStream output) {
        this.output = output;
    }

    public PixelFormat getServerPixelFormat() {
        return serverPixelFormat;
    }

    public void setServerPixelFormat(PixelFormat serverPixelFormat) {
        this.serverPixelFormat = serverPixelFormat;
    }

    public String getServerName() {
        return serverName;
    }

    public void setServerName(String serverName) {
        this.serverName = serverName;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public static String getProtocolVersion() {
        return protocolVersion;
    }

    public static void setProtocolVersion(String protocolVersion) {
        RFBClient.protocolVersion = protocolVersion;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public byte[] getScreen() {
        return screen;
    }

    public void setScreen(byte[] screen) {
        this.screen = screen;
    }
    //endregion

}

