#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "Transforms"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define PI 3.14


static void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]) {
    int r = a0[0];
    float std_dev = f0[0];
    int image_width = info->width;
    int image_height=info->height;

    float *g = new float[2 * r + 1];
    int red ,green, blue;
    int* q_array = new int[image_height*image_width];
    float constant_1 = 2*std_dev*std_dev;
    float constant_2 = 1/(sqrt(constant_1*PI));
    uint32_t *px = (uint32_t *) pixels;
    for(int k=0;k<2*r+1;k++){
        g[k] = constant_2*exp(-1*((k*k)/(constant_1)));
    }
    for (int y = 0; y <image_height; y++) {
        for (int x = 0; x <image_width; x++) {
            int position = y*image_width+x;
            int index = x-r;
            for(int k=0;k<2*r+1;k++){
                red=green=blue=0;
                if(index+k>=0 && index+k<image_width){
                    int pos = y*image_width+(index+k);

//                    red+=g[k]*(int) (px[pos] & 0x00000FF);
//                    green+=g[k]*(int)((px[pos] & 0x0000FF00) >> 8);
//                    blue+=g[k]*(int) ((px[pos] & 0x00FF0000) >> 16);
                    red+=g[k]*px[pos];
                }
            }
//            q_array[position]=((blue << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00) |
//                               (red & 0x000000FF);
            q_array[position]=red;
//            px[position]= (((blue << 16) & 0xFFFF0000) | ((green << 8) & 0xFF00FF00) |
//                               (red & 0xFF0000FF));
        }
    }
    //px= (uint32_t *) q_array;
    for (int x = 0; x < image_width; x++){
        for (int y = 0; y < image_height; y++){
            int position = y*image_width+x;
            int index = x-r;
            for(int k=0;k<2*r+1;k++){
                red=green=blue=0;

                if(index+k>=0 && index+k<image_height){
                    int pos = (index+k)*image_width+x;
                    red+=g[k]*q_array[pos];
//                    red+=g[k]*(int) (q_array[pos] & 0x00000FF);
//                    green+=g[k]*(int)((q_array[pos] & 0x0000FF00) >> 8);
//                    blue+=g[k]*(int) ((q_array[pos] & 0x00FF0000) >> 16);
                    px[position]=red;
                }
            }
//            px[position]=((blue << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00) |
//                          (red & 0x000000FF);

        }
    }
    delete[] q_array;
    delete[] g;
    LOGI("Exiting Gaussian Blur function");
}
extern "C" {
    JNIEXPORT void JNICALL
    Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_gaussian_1blur(JNIEnv *env, jobject instance,
                                                                              jobject img,
                                                                              jintArray intArgs_,
                                                                              jfloatArray floatArgs_) {
        LOGI("JNI Link to Native Call");
        AndroidBitmapInfo info;
        int ret;
        void *pixels;
        ret=AndroidBitmap_getInfo(env, img, &info);
        ret=AndroidBitmap_lockPixels(env, img, &pixels);
        int *intArgs = env->GetIntArrayElements(intArgs_, NULL);
        float *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);

        LOGI("Locking pixels");
        gaussian_blur(&info,pixels, intArgs, floatArgs);

        LOGI("Unlocking");
        AndroidBitmap_unlockPixels(env, img);

        env->ReleaseIntArrayElements(intArgs_, intArgs, 0);
        env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
    }
}