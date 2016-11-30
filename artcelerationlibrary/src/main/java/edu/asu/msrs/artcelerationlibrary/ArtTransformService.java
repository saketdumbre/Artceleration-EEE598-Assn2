package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

import java.io.ByteArrayOutputStream;

public class ArtTransformService extends Service {

    public ArtTransformService() {
    }
    String TAG = "AtTransformService";
    static final int GAUSSIAN_BLUR = 0;
    static final int UNSHARP_MASK = 1;
    static final int SOBEL_EDGE =2;
    static final int NEON_EDGE=3;

    static {
        System.loadLibrary("ImageTransforms");
    }
    public native void gaussian_blur(Bitmap img, int[] intArgs, float[] floatArgs);
    public native void unsharp_mask(Bitmap img,float[] floatArgs);
    public native void sobel_edge_filter(Bitmap img,int[] a);
    public native void neon_edge(Bitmap img, float[] floatArgs);

    class ArtTransformHandler extends Handler{
        @Override
        public void handleMessage(Message msg){
            Log.i(TAG,"IN Thread");
            final Bundle dataBundle = msg.getData();
            mmMessenger=msg.replyTo;
            byte[] byteArray = dataBundle.getByteArray("Image");
            final Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
            Log.d(TAG, "handleMessage(msg):" + msg.what);
            switch(msg.what){
                case UNSHARP_MASK:
                    Thread unsharpMask = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            unsharp_mask(bmp,dataBundle.getFloatArray("FloatArgs"));
                            respondTransform(bmp,UNSHARP_MASK,dataBundle.getInt("ID"));
                        }
                    });
                    unsharpMask.start();
                    break;
                case GAUSSIAN_BLUR:
                    Thread gaussianBlur = new Thread((new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            gaussian_blur(bmp,dataBundle.getIntArray("IntArgs"),dataBundle.getFloatArray("FloatArgs"));
                            respondTransform(bmp,GAUSSIAN_BLUR,dataBundle.getInt("ID"));
                        }
                    }));
                    gaussianBlur.start();
                    break;
                case SOBEL_EDGE:
                    Thread sobelEdge = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            sobel_edge_filter(bmp,dataBundle.getIntArray("IntArgs"));
                            respondTransform(bmp,SOBEL_EDGE,dataBundle.getInt("ID"));
                        }
                    });
                    sobelEdge.start();
                    break;
                case NEON_EDGE:
                    final Thread neonEdge =new Thread(new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            neon_edge(bmp,dataBundle.getFloatArray("FloatArgs"));
                            respondTransform(bmp,UNSHARP_MASK,dataBundle.getInt("ID"));
                        }
                    });
                    neonEdge.start();
                default:
                    break;

            }
        }

    }
    final Messenger mMessenger = new Messenger(new ArtTransformHandler());
    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }

    private Messenger mmMessenger;
    private boolean mBound;
    ServiceConnection mServiceConnection = new ServiceConnection(){

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mmMessenger = new Messenger(service);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

            mmMessenger = null;
            mBound = false;
        }
    };

    public boolean respondTransform(Bitmap img, int index, int ID){
        Log.i(TAG,"Respond Transform called");
        //            MemoryFile memFile = new MemoryFile("someone",30);
//            ParcelFileDescriptor pfd =  MemoryFileUtil.getParcelFileDescriptor(memFile);

        int what = index;
        Bundle dataBundle = new Bundle();
//            dataBundle.putParcelable("pfd", pfd);
        Message msg = Message.obtain(null,what,4,5);
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        img.compress(Bitmap.CompressFormat.PNG, 100, stream);
        byte[] byteArray = stream.toByteArray();
        dataBundle.putByteArray("Image",byteArray);
        dataBundle.putInt("ID",ID);
        msg.setData(dataBundle);

        try {
            mmMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        return true;
    }
}
