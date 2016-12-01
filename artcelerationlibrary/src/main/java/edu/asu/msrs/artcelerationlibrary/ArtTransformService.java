/*This class describes the functionality of the Art Transform Service
* The Important member fields are
*   1. String TAG = "ArtTransformService" - Tag used while displaying log messages
    2. final Messenger mMessenger - This Objectis bound to the ArtTransformHandler- process messages
                                    fromlibrary
    3.private Messenger mmMessenger - This object is bound to te service and is used to send responses
                                    to the Library
    4.private boolean mBound - Thsi is used to detected if the service is connected or not.
    5.static final int GAUSSIAN_BLUR, UNSHARP_MASK,SOBEL_EDGE, NEON_EDGE, MOTION_BLUR
        - These are flags that are used to classify the image transforms.
    6. ServiceConnection mServiceConnection - used to establish a connection to the service
Each member function has a description within the class.
*/
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
    String TAG = "ArtTransformService";
    static final int GAUSSIAN_BLUR = 0;
    static final int UNSHARP_MASK = 1;
    static final int SOBEL_EDGE =2;
    static final int NEON_EDGE=3;
    static final int MOTION_BLUR=4;
    /*The following function loads the Native Library built by linking the native source files*/
    static {
        System.loadLibrary("ImageTransforms");
    }
    /*Declaring each of the transforms with native keywork - to link to the corresponding JNI call*/
    public native void gaussian_blur(Bitmap img, int[] intArgs, float[] floatArgs);
    public native void unsharp_mask(Bitmap img,float[] floatArgs);
    public native void sobel_edge_filter(Bitmap img,int[] a);
    public native void neon_edge(Bitmap img, float[] floatArgs);
    public native void motion_blur(Bitmap img, int[] intArgs);

    /*This class handles the requests from the Library
    * The class extends the Handler subclass and overrides the handleMessage(Message) to implement
    * the following functionality
    * 1. Set mmMessenger as the replyto Parameter to be able to reply to the Library calls
    * 2. Identify the appropriate Transform to be applied using msg.what parameter
    * 3. Launch a thread that makes a call to the appropriate transform in the Backgroun so as to
    *    not block mai UI Thread
    * 4. rename the thread so that launching a consequent request doesnt crash(i.e trying to start
    *    the same thread twice)unction once the
    * 5. Call the respondTransform transform is processed*/
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
                    unsharpMask.setName(String.valueOf(dataBundle.getInt("ID")));
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
                    gaussianBlur.setName(String.valueOf(dataBundle.getInt("ID")));
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
                    sobelEdge.setName(String.valueOf(dataBundle.getInt("ID")));
                    break;
                case NEON_EDGE:
                    final Thread neonEdge =new Thread(new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            neon_edge(bmp,dataBundle.getFloatArray("FloatArgs"));
                            respondTransform(bmp,UNSHARP_MASK,dataBundle.getInt("ID"));}
                    });
                    neonEdge.start();
                    neonEdge.setName(String.valueOf(dataBundle.getInt("ID")));
                    break;
                case MOTION_BLUR:
                    final Thread motionBlur =new Thread(new Runnable() {
                        @Override
                        public void run() {
                            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
                            motion_blur(bmp,dataBundle.getIntArray("IntArgs"));
                            respondTransform(bmp,UNSHARP_MASK,dataBundle.getInt("ID"));
                        }
                    });
                    motionBlur.start();
                    motionBlur.setName(String.valueOf(dataBundle.getInt("ID")));
                    break;
                default:
                    break;

            }
        }

    }
    /*This Object is bound to the ArtTransform Handler- which responds when the a request is received
     from Library*/
    final Messenger mMessenger = new Messenger(new ArtTransformHandler());
    @Override
    public IBinder onBind(Intent intent) {
        return mMessenger.getBinder();
    }

    private Messenger mmMessenger;
    private boolean mBound;
    /*Establishing connection to the service through an IBinder object and setting up a messenger
    * with the service*/
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
    /*This function is called after each transform is  processed, the bitmap is compressed and the
    ID and image are sent back to the Library*/
    public boolean respondTransform(Bitmap img, int index, int ID){
        Log.i(TAG,"Respond Transform called");

        int what = index;
        Bundle dataBundle = new Bundle();
        Message msg = Message.obtain(null,what,4,5);
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        //Compressing the image into a ByteArray
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
