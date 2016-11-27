#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "Transforms"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define PI 3.14

int value_check(int color_channel){
    if(color_channel>255) return 255;
    else if(color_channel<0) return 0;
    else return color_channel;
}

static void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]) {
    int r = a0[0];
    float std_dev = f0[0];
    int image_width = info->width;
    int image_height=info->height;

    float *g = new float[2 * r + 1];
    int red ,green, blue,alpha;
    //Allocating memory for q_array
    int* q_array = new int[image_height*image_width];
    //Assigning Constant_1 = 2*(Square of standard deviation)
    float constant_1 = 2*std_dev*std_dev;
    //Assigning constant_2 = 1/(square_root(2*pi*(square of  standard deviation))
    float constant_2 = 1/(sqrt(constant_1*PI));
    uint32_t *px = (uint32_t *) pixels;
    for(int k=-1*r;k<=r;k++){
        g[k] = constant_2*exp(-1*((k*k)/(constant_1)));
    }
    for (int y = 0; y < image_height; y++){
        for (int x = 0; x < image_width; x++){
            int position = y*image_width+x;
            red=green=blue=0;
            for(int k=-r;k<=r;k++){
                int pos = y*image_width+(x+k);
                if(x+k>=0 && x+k<image_width) {
                    red += g[k] * (px[pos] & 0x00000FF);
                    green += g[k] * ((px[pos] & 0x0000FF00) >> 8);
                    blue += g[k] * ((px[pos] & 0x00FF0000) >> 16);
                }
            }
            red=value_check(red);
            green=value_check(green);
            blue=value_check(blue);
            q_array[position]=((blue << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00) |
                              (red & 0x000000FF);
        }
    }
    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            int position = y * image_width + x;
            red = green = blue = 0;
            for (int k = -r; k <= r; k++) {
                if (y + k >= 0 && y + k < image_height) {
                    int pos = (y + k) * image_width + x;
                    red += g[k] * (q_array[pos] & 0x00000FF);
                    green += g[k] * ((q_array[pos] & 0x0000FF00) >> 8);
                    blue += g[k] * ((q_array[pos] & 0x00FF0000) >> 16);
                }
            }
            red=value_check(red);
            green=value_check(green);
            blue=value_check(blue);
            px[position] =((blue << 16) & 0x00FF0000) |((green << 8) & 0x0000FF00) |(red & 0x000000FF);
        }
    }
    delete[] q_array;
    delete[] g;
    LOGI("Exiting Gaussian Blur function");
}

static void unsharp_mask(AndroidBitmapInfo* info, uint32_t *pixels, float f[]){
    LOGI("Starting UnSharp");
    //Making a copy of the original image
    uint32_t*original=new uint32_t[info->width*info->height];
    for (int y=0;y<info->height;++y){
        for(int x=0;x<info->width;++x){
            int position = y*info->width+x;
            original[position]=pixels[position];
            LOGI("%d %d",x,y);
        }
    }
    LOGI("Creating Original Copy");
    int a[1];
    float sigma[1];
    a[0]=6*f[0];
    sigma[0]=f[0];
    int red,green,blue;
    //Calling Gaussian Blur with radius=6*standard deviation
    gaussian_blur(info,pixels,a,sigma);
    LOGI("Gaussian Blur Done");
    LOGI("Scaled Subtraction and Addition");
    uint32_t *px= pixels;
    for (int y = 0; y <info->height; y++) {
        for (int x = 0; x < info->width; x++) {
            int pos = y*info->width+x;
            red =f[1]*((pixels[pos] & 0x00000FF)-(original[pos]&0x00000FF));
            green=f[1] *(((pixels[pos] & 0x0000FF00) >> 8)-((original[pos] & 0x0000FF00) >> 8));
            blue=f[1] *(((pixels[pos] & 0x00FF0000) >> 16)-((original[pos] & 0x00FF0000) >> 16));

            red=red+(original[pos]&0x00000FF);green=green+((original[pos] & 0x0000FF00) >> 8);
            blue=blue+((original[pos] & 0x00FF0000) >> 16);

            px[pos]=((blue << 16) & 0x00FF0000) |((green << 8) & 0x0000FF00) |(red & 0x000000FF);

        }
    }
    delete []original;
    LOGI("Exiting Unsharp");
}
extern "C" {
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_unsharp_1mask(JNIEnv *env,
                                                                         jobject instance,
                                                                         jobject img,
                                                                         jfloatArray floatArgs_) {
    LOGI("JNI: - Unsharp Mask");
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;

    ret=AndroidBitmap_getInfo(env, img, &info);
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jfloat *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    LOGI("Locking pixels");
    unsharp_mask(&info,pixels,floatArgs);

    LOGI("Unlocking");
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}

JNIEXPORT void JNICALL
    Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_gaussian_1blur(JNIEnv *env, jobject instance,
                                                                              jobject img,
                                                                              jintArray intArgs_,
                                                                              jfloatArray floatArgs_) {
        LOGI("JNI: - Gaussian Blur");
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