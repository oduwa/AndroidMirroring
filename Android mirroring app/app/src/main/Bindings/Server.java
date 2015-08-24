package main.Bindings;

/**
 * Created by oeo on 19/08/2015.
 */
public class Server {

    public static native long getInstance();

    static {
        System.loadLibrary("j");
    }

}
