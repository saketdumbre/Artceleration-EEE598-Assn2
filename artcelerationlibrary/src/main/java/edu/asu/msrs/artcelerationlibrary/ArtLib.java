/*This class describes the functionality of the Art Transform Library
* The Important member fields are
*   1. String TAG = "ArtLibrary" - Tag used while displaying log messages
    2. private int requestID     - Used to number the transform according to the order of request
    3. private int respondTransformID - Used to keep track of which was the latest transform that was
                                     applied
    4. HashMap<Integer,Bitmap> responses- A hashmap of images and their associated request ID
    5. Queue<Integer> responseQueue     - Stores the requestIDs of the transforms that have been processed
    6. final Messenger mmMessenger - This Objectis bound to the ArtLib Handler - which responds when the
                                     transform is processed;
    7.private Messenger mMessenger - This object iss bound to te service and is used to send requests
                                    to the service
    8.private boolean mBound - Thsi is used to detected if the service is connected or not.
    9.static final int GAUSSIAN_BLUR, UNSHARP_MASK,SOBEL_EDGE, NEON_EDGE, MOTION_BLUR
        - These are flags that are used to classify the image transforms.
    10. ServiceConnection mServiceConnection - used to establish a connection to the service
    11.private TransformHandler artlistener - used to pass the image processed back to the UI Thread

Each member function has a description within the class.
*/
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

/**
 * Created by rlikamwa on 10/2/2016.
 */

public class ArtLib {
    private TransformHandler artlistener;
    private Activity mActivity;
    private int requestID;
    private int respondTransformID;
    private HashMap<Integer,Bitmap> responses=new HashMap<>();
    private Queue<Integer> responseQueue = new LinkedList<Integer>();
    private Messenger mMessenger;
    private boolean mBound;

    String TAG = "ArtLibrary";
    static final int GAUSSIAN_BLUR = 0;
    static final int UNSHARP_MASK = 1;
    static final int SOBEL_EDGE =2;
    static final int NEON_EDGE=3;
    static final int MOTION_BLUR=4;
    List<Integer> t= new ArrayList<Integer>();

    /*This class handles the responses from the Service after a transform has been processed and
    * completed. The class extends the Handler subclass and overrides the handleMessage(Message)
    * function to and calls the artlistener.onTransformProcessed(image) to pass the image back to
    * the UI Thread.
    * the FIFO order is maintained by using a Linkedlist Queue, requestID and respondTransformID
    * if the requestID of received message is greater than the respondTransformID - queue the
    * requestID and store the image in Hashmap<requestID,Bitmap>
    * if requestID of message=respondTransformID - calls the artlistener.onTransformProcessed(image)
    * And loop hrough the queue to see if the next one has been processed and received already*/
    class ArtLibHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Transform:" + msg.what);
            //Get bundled data from the message object
            Bundle dataBundle = msg.getData();
            int ID = dataBundle.getInt("ID");
            byte[] byteArray = dataBundle.getByteArray("Image");
            //Retrieving the Bitmap from the AyteArray
            Bitmap bmp = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
            Log.d(TAG, "Transform Received:" +ID);
            //Check if the response received is the next one to be processed according
            //to FIFO order
            responses.put(ID,bmp);
            responseQueue.add(ID);
            for(int i=respondTransformID;i<responseQueue.size();i++){
                if(responseQueue.contains(i)){
                    Log.i(TAG, "Calling TransformHandler for ID =" + String.valueOf(i));
                    // Increment the respondTransformID to record next expected transform
                    artlistener.onTransformProcessed(responses.get(i));
                    respondTransformID=respondTransformID+1;
                    responses.remove(i);
                    // Keeping a list of all requests that are processed
                    t.add(i);
                }
                else {
                    //Add the image to the HashMap using requestID as key
                    responses.put(ID, bmp);
                    //Add the requestID to the queue
                    responseQueue.add(ID);
                    break;
                }
            }
                //Remove transforms already processed from the queue
                responseQueue.remove(t);
            //Transforms requested later responds early then Add to Queue
        }
    }
    /*Constructor for the class -
    * 1. Assigns the mActivity member to the activity that calls the Library object
    * 2. calls the init function that binds the service
    * 3. initializes the requestID to be 0
    * 4. initialized the respondTransformID to be 0
    *    have ID=1 - else queue it*/
    public ArtLib(Activity activity){
        mActivity=activity;
        init();
        requestID=0;
        respondTransformID=0;
    }
    /*Establishing connection to the service through an IBinder object and setting up a messenger
    * with the service*/
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

    /*'Bind the Activity to the service using the Service connection object created above*/
    public void init(){
        mActivity.bindService(new Intent(mActivity, ArtTransformService.class), mServiceConnection, Context.BIND_AUTO_CREATE);
    }
    /*This functions returns a String array of the names of the transforms that the app will
    * implement - namely the Gaussian Blur, Unsharp Mask Sobel Edge Filter Neon Edges and motion
    * Blur*/
    public String[] getTransformsArray(){
        String[] transforms = {"Gaussian Blur", "Unsharp Mask","Sobel Edge Filter","Neon Edges","Motion Blur"};
        return transforms;
    }
    /*This functions initializes the arguments that each of the transforms will take as input
    * An array of type TransformTest is returned with 5 elements each with an int array and a
    * float array */
    public TransformTest[] getTestsArray(){
        TransformTest[] transforms = new TransformTest[5];
        transforms[0]=new TransformTest(0, new int[]{60}, new float[]{5.5f});
        transforms[1]=new TransformTest(1, new int[]{}, new float[]{20f,10f});
        transforms[2]=new TransformTest(2, new int[]{2},new float[]{});
        transforms[3]=new TransformTest(3, new int []{},new float[]{3.5f,10.7f,0.5f});
        transforms[4]=new TransformTest(4, new int[]{1,10},new float[]{});
        return transforms;
    }
    /*This function registers the TransformHandler - the registered handler is used to call
     * the onTransformProcessed method  */
    public void registerHandler(TransformHandler artlistener){
        this.artlistener=artlistener;
    }

    /*This function is called from the MainViewer Activity and does the following
     * 1.checks whether the arguments are valid for the TransformTest that it selected
     * 2. if it is valid, a requestID is assigned to it (requestId increases in serial order and is
     * used to implement the "First In" part of the FIFO functionality.
     * If invalid arguments-false is returned to MainViewer Activity
     * 3. The the bitmap,index,requestID are bundled into a message and sent to the service to
     * be processed.
     * 4. msg.what is assigned the index of transform so that approprate function calls can be made
     * 5. msg.replyTo is assigned the mmMessenger object which is linked to the ArtLibHandler
     */
    public boolean requestTransform(Bitmap img, int index, int[] intArgs, float[] floatArgs){
        if(img!=null && intArgs!=null && floatArgs!=null) {
            if(checkArgumentValidity(index,intArgs,floatArgs)){
                int what = index;
                Bundle dataBundle = new Bundle();
                //Compress Bitmap to byte aray
                ByteArrayOutputStream stream = new ByteArrayOutputStream();
                img.compress(Bitmap.CompressFormat.PNG, 100, stream);
                byte[] byteArray = stream.toByteArray();
                dataBundle.putByteArray("Image", byteArray);
                dataBundle.putIntArray("IntArgs", intArgs);
                dataBundle.putFloatArray("FloatArgs", floatArgs);
                //Increment the requestID (Initial value=0 therefore first request will be
                // assigned ID=1;
                dataBundle.putInt("ID",requestID);
                requestID=requestID+1;
                Message msg = Message.obtain(null, what, 2, 3);
                msg.replyTo=mmMessenger;
                msg.setData(dataBundle);

                try {
                    mMessenger.send(msg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                return true;
            }
            else{
                Log.i(TAG,"Invalid Arguments");
                return false;
            }

        }
        else {
            Log.i(TAG,"Invalid Arguments");
            return false;
        }

    }
    /*This function checks the validity of the arguments in the selected transform
    * From the documentation of transforms te conditions are checked.They are as follows
     * 1. Sobel edge filter should have int argument value to be 0,1,2 only
     * 2. Motion Blur Should taken in only positive integer and float values
     * 3. Gaussian Blur should taken in only positive integer and float values
     * 4. Since Neon Edges are based on Sobel edge and gaussian blur filter -
     *    the condition check is as described in 1 &3
     * 5. Since the Unsharp mask is dependent on Gaussian blur - it has the same float value
     *    condition check*/
    private boolean checkArgumentValidity(int index, int[] intArgs, float[] floatArgs) {

        if(index==SOBEL_EDGE){
            if(intArgs[0]==0 ||intArgs[0]==1||intArgs[0]==2)
                return true;
            else
                return false;
        }
        if(index==MOTION_BLUR){
            if(intArgs[0]==0||intArgs[0]==1)
                if(intArgs[1]>0)
                    return true;
            else
                return false;
        }
        if(index==GAUSSIAN_BLUR){
            if(intArgs[0]>=0 && floatArgs[0]>=0)
                return true;
            else
                return false;
        }
        if(index==UNSHARP_MASK) {
            if (floatArgs[0] >= 0 && floatArgs[1] >= 0)
                return true;
            else
                return false;
        }
        if(index==NEON_EDGE){
            if (floatArgs[0] >= 0 && floatArgs[1] >= 0 && floatArgs[2] >= 0)
                return true;
            else
                return false;
        }
        return true;
    }
    /*This Objectis bound to the ArtLib Handler - which responds when the transform is processed;*/
    final Messenger mmMessenger = new Messenger(new ArtLib.ArtLibHandler());

}
