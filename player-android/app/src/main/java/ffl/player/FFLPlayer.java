package ffl.player;

import android.view.Surface;

import java.lang.ref.WeakReference;

/**
 * Created by zhufeifei on 2018/7/2.
 */

public class FFLPlayer {
    static{
        System.loadLibrary("FFLPlayer");
        System.loadLibrary("ffmpeg");
    }
    public FFLPlayer(){
        native_setup(new WeakReference(this));
    }


    private static void postEventFromNative(Object weakThiz, int what, int arg1, int arg2, Object obj){
        FFLPlayer player = (FFLPlayer) ((WeakReference) weakThiz).get();
        if (player == null) {
            return;
        }
    }

    public native void native_setup(Object weakthis);
    public native void native_release();
    public native void native_finalize();
    public native int native_play(String url);
    public native int native_setUrl(String url);
    public native int native_setSurface(Surface surface);
    public native int native_setSurfaceSize(int widht,int height);
    public native int native_prepare();
    public native int native_start();
    public native int native_pause(int pause);
    public native int native_stop();
    public native int native_seekTo(long us);
    public native long native_getCurrentPosition();
    public native long native_getDuration();
    public native void native_setSpeed(int speed);
    public native int native_getSpeed();
    public native void native_setVolume(int vol);
    public native int native_getVolume();
    public native void native_setLoop(int num);
    public native int native_getLoop();

    private long mNativePlayer;
}
