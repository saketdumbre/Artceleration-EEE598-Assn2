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
import android.os.MemoryFile;
import android.os.Message;
import android.os.Messenger;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class ArtTransformService extends Service {
    public ArtTransformService() {
    }
    String TAG = "AtTransformService";
    static final int GAUSSIAN_BLUR = 1;
    static final int UNSHARP_MASK = 2;
    static final int SOBEL_EDGE =3;

    static {
        System.loadLibrary("ImageTransforms");
    }
    public native void gaussian_blur(Bitmap img, int[] intArgs, float[] floatArgs);
    public native void unsharp_mask(Bitmap img,float[] floatArgs);
    public native void sobel_edge_filter(Bitmap img,int[] a);


    class ArtTransformHandler extends Handler{
        @Override
        public void handleMessage(Message msg){
            Bundle dataBundle = msg.getData();
            mmMessenger=msg.replyTo;
            byte[] byteArray = dataBundle.getByteArray("Image");
            Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
            Log.d(TAG, "handleMessage(msg):" + msg.what);
            switch(msg.what){
                case UNSHARP_MASK:
                    unsharp_mask(bmp,dataBundle.getFloatArray("FloatArgs"));
                    respondTransform(bmp,UNSHARP_MASK,dataBundle.getLong("TimeStamp"));
                    break;
                case GAUSSIAN_BLUR:
                    gaussian_blur(bmp,dataBundle.getIntArray("IntArgs"),dataBundle.getFloatArray("FloatArgs"));
                    respondTransform(bmp,GAUSSIAN_BLUR,dataBundle.getLong("TimeStamp"));
                    break;
                case SOBEL_EDGE:
                    sobel_edge_filter(bmp,dataBundle.getIntArray("IntArgs"));
                    respondTransform(bmp,SOBEL_EDGE,dataBundle.getLong("TimeStamp"));
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

    public boolean respondTransform(Bitmap img, int index, Long requestTime){
        Log.i(TAG,"Respond Transform called");
        try {
            MemoryFile memFile = new MemoryFile("someone",30);
            ParcelFileDescriptor pfd =  MemoryFileUtil.getParcelFileDescriptor(memFile);

            int what = index;
            Bundle dataBundle = new Bundle();
            dataBundle.putParcelable("pfd", pfd);
//            dataBundle.putParcelable("Image",img);
            Message msg = Message.obtain(null,what,4,5);
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            img.compress(Bitmap.CompressFormat.PNG, 100, stream);
            byte[] byteArray = stream.toByteArray();
            dataBundle.putByteArray("Image",byteArray);

            msg.setData(dataBundle);

            try {
                mmMessenger.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return true;
    }
}
