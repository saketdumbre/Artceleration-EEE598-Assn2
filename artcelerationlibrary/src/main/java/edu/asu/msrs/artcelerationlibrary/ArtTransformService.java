package edu.asu.msrs.artcelerationlibrary;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;

public class ArtTransformService extends Service {
    public ArtTransformService() {
    }
    String TAG = "AtTransformService";
    static final int MSG_HELLO = 1;
    static final int MSG_MULT = 2;

    class ArtTransformHandler extends Handler{
        @Override
        public void handleMessage(Message msg){
            Log.d(TAG, "handleMessage(msg):" + msg.what);
            switch(msg.what){
                case MSG_HELLO:
                    Log.d(TAG, "HELLO!");
                    break;

                case MSG_MULT:
                    Bundle dataBundle = msg.getData();
                    //ParcelFileDescriptor pfd = (ParcelFileDescriptor)dataBundle.get("pfd");
                    //FileInputStream fios = new FileInputStream(pfd);

                    int result = msg.arg1 * msg.arg2;
                    Log.d(TAG, "MULT! " + result);
                    break;

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

    public boolean respondTransform(Bitmap img, int index, int[] intArgs, float[] floatArgs){
        try {
            MemoryFile memFile = new MemoryFile("someone",30);
            ParcelFileDescriptor pfd =  MemoryFileUtil.getParcelFileDescriptor(memFile);

            int what = ArtLib.MSG_MULT;
            Bundle dataBundle = new Bundle();
            dataBundle.putParcelable("pfd", pfd);
            Message msg = Message.obtain(null,what,4,5);
            msg.setData(dataBundle);

            try {
                mMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return true;
    }

}
