package edu.asu.msrs.artcelerationlibrary;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.Semaphore;

/**
 * Created by rlikamwa on 10/2/2016.
 */

public class ArtLib {
    private TransformHandler artlistener;
    private Activity mActivity;
    String TAG = "AtTransformService";
    private int requestID;
    private int respondTransformID;
    HashMap<Integer,Bitmap> responses=new HashMap<>();
    Queue<Integer> responseQueue = new LinkedList<Integer>();
    List<Integer> t= new ArrayList<Integer>();
    private final Semaphore sem = new Semaphore(1, true);
    static final int GAUSSIAN_BLUR = 0;
    static final int UNSHARP_MASK = 1;
    static final int SOBEL_EDGE =2;
    static final int NEON_EDGE=3;

    class ArtLibHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Transform:" + msg.what);
            Bundle dataBundle = msg.getData();
            int ID = dataBundle.getInt("ID");
            byte[] byteArray = dataBundle.getByteArray("Image");
            Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
            Log.d(TAG, "Transform:" + msg.what);
            Log.d(TAG,"REQUEST ID = "+ ID+ " RESPOND TRANSFORM ID = "+respondTransformID);

            if(ID==respondTransformID) {
                Log.i(TAG, "Calling TransformHandler " + String.valueOf(msg.what));
                responses.put(ID,bmp);
                responseQueue.add(ID);
                for(int i=ID;i<=responseQueue.size();i++){
                    if(responseQueue.contains(i)){
                        Log.i(TAG,"Responding to Resquest: " + String.valueOf(i)+ " msg.what= "+ String.valueOf(msg.what));
                        respondTransformID=respondTransformID+1;
                        artlistener.onTransformProcessed(responses.get(i));
                        Log.i(TAG,"Transform applied");
                        responses.remove(i);
                        t.add(i);
                    }
                    else
                            break;
                }
                responseQueue.remove(t);
            }
            if (ID!=respondTransformID){
                    responses.put(ID,bmp);
                    responseQueue.add(ID);
            }
        }
    }
    public ArtLib(Activity activity){
        mActivity=activity;
        init();
        requestID=0;
        respondTransformID=1;
    }



    private Messenger mMessenger;
    private boolean mBound;
    ServiceConnection mServiceConnection = new ServiceConnection(){

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mMessenger = new Messenger(service);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

            mMessenger = null;
            mBound = false;
        }
    };


    public void init(){
        mActivity.bindService(new Intent(mActivity, ArtTransformService.class), mServiceConnection, Context.BIND_AUTO_CREATE);
    }
    public String[] getTransformsArray(){
        String[] transforms = {"Gaussian Blur", "Unsharp Mask","Sobel Edge Filter"};//,"Neon Edges"};
        return transforms;
    }

    public TransformTest[] getTestsArray(){
        TransformTest[] transforms = new TransformTest[3];
        transforms[0]=new TransformTest(0, new int[]{100}, new float[]{10.5f});
        transforms[1]=new TransformTest(1, new int[]{}, new float[]{5.5f,10.5f});
        transforms[2]=new TransformTest(2, new int[]{2},new float[]{});
        //transforms[3]=new TransformTest(3, new int []{},new float[]{3.5f,10.7f,0.3f});
        return transforms;
    }

    public void registerHandler(TransformHandler artlistener){
        this.artlistener=artlistener;
    }

    public boolean requestTransform(Bitmap img, int index, int[] intArgs, float[] floatArgs){
        if(img!=null && intArgs!=null && floatArgs!=null) {
            //                MemoryFile memFile = new MemoryFile("someone", 30);
//                ParcelFileDescriptor pfd = MemoryFileUtil.getParcelFileDescriptor(memFile);

            int what = index;
            Bundle dataBundle = new Bundle();
//                dataBundle.putParcelable("pfd", pfd);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            img.compress(Bitmap.CompressFormat.PNG, 100, stream);
            byte[] byteArray = stream.toByteArray();
            dataBundle.putByteArray("Image", byteArray);
            dataBundle.putIntArray("IntArgs", intArgs);
            dataBundle.putFloatArray("FloatArgs", floatArgs);
            requestID=requestID+1;
            dataBundle.putInt("ID",requestID);
            Message msg = Message.obtain(null, what, 2, 3);
            msg.replyTo=mmMessenger;
            msg.setData(dataBundle);


            try {
                mMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        else
            Log.i(TAG,"Arrays are null");
        Log.i(TAG,"Exiting from Request Transform");

        return true;
    }

   final Messenger mmMessenger = new Messenger(new ArtLib.ArtLibHandler());

}
