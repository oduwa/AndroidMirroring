/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package main.Networking;

/**
 *
 * @author Odie
 */
public class PixelFormat {
    
    int bitsPerPixel;
    int depth;
    boolean isBigEndian;
    boolean isTrueColour;
    int redMax;
    int greenMax;
    int blueMax;
    int redShift;
    int greenShift;
    int blueShift;
    
    public PixelFormat(){
        bitsPerPixel = 32;
        depth = 8; // i think
        isBigEndian = true;
        isTrueColour = true;
        redMax = 255;
        greenMax = 255;
        blueMax = 255;
        redShift = 0; // TODO: FIll in later
        greenShift = 0; // TODO: FIll in later
        blueShift = 0; // TODO: FIll in later
    }
    
    public PixelFormat(int bpp, int d, boolean be, boolean tc, int rm, int gm, int bm, int rs, int gs, int bs){
        bitsPerPixel = bpp;
        depth = d; 
        isBigEndian = be;
        isTrueColour = tc;
        redMax = rm;
        greenMax = gm;
        blueMax = bm;
        redShift = rs; 
        greenShift = gs;
        blueShift = bs;
    }
    
}
