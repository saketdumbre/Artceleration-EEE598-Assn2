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
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Created by rlikamwa on 10/2/2016.
 */

public class ArtLib {
    private TransformHandler artlistener;
    private Activity mActivity;

    class ArtLibHandler extends Handler {
        @Override
        public void handleMessage(Message msg){
            Log.d(TAG, "handleMessage(msg):" + msg.what);
            switch(msg.what){
                case UNSHARP_MASK:
                    break;
                case GAUSSIAN_BLUR:
                    Log.i(TAG,"After Respond Transform");
                    Bundle dataBundle = msg.getData();
                    byte[] byteArray = dataBundle.getByteArray("Image");
                    Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);

                    Log.i(TAG,"Calling TransformHandler");
                    artlistener.onTransformProcessed(bmp);
            }
        }
    }

    String TAG = "AtTransformService";
    static final int GAUSSIAN_BLUR = 1;
    static final int UNSHARP_MASK = 0;


    public ArtLib(Activity activity){
        mActivity=activity;
        init();
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
        String[] transforms = {"Unsharp Mask", "Gaussian Blur", "Color Filter"};
        return transforms;
    }

    public TransformTest[] getTestsArray(){
        TransformTest[] transforms = new TransformTest[3];
        transforms[0]=new TransformTest(0, new int[]{1,2,3}, new float[]{0.1f, 0.2f, 0.3f});
        transforms[1]=new TransformTest(1, new int[]{45}, new float[]{0.8f});
        transforms[2]=new TransformTest(2, new int[]{51,42,33}, new float[]{0.5f, 0.6f, 0.3f});

        return transforms;
    }

    public void registerHandler(TransformHandler artlistener){
        this.artlistener=artlistener;
    }

    public boolean requestTransform(Bitmap img, int index, int[] intArgs, float[] floatArgs){
        if(img!=null && intArgs!=null && floatArgs!=null) {
            try {
                MemoryFile memFile = new MemoryFile("someone", 30);
                ParcelFileDescriptor pfd = MemoryFileUtil.getParcelFileDescriptor(memFile);

                int what = index;
                Bundle dataBundle = new Bundle();
                dataBundle.putParcelable("pfd", pfd);
                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                img.compress(Bitmap.CompressFormat.PNG, 100, stream);
                byte[] byteArray = stream.toByteArray();
                dataBundle.putByteArray("Image", byteArray);
                dataBundle.putIntArray("IntArgs", intArgs);
                dataBundle.putFloatArray("FloatArgs", floatArgs);
                Long timestamp = System.currentTimeMillis() / 1000;
                dataBundle.putLong("TimeStamp", timestamp);
                Message msg = Message.obtain(null, what, 2, 3);
                msg.replyTo=mmMessenger;
                msg.setData(dataBundle);

                try {
                    mMessenger.send(msg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            } catch (IOException e) {
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
