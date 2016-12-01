/*This file includes the JNI Links to all the native calls from the ArtTransformService
 * Each of them will have a call that matches the arguments defined in the ArtTransformService
 * and an env obj and an instance object
 */
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>

#include "ImageTransforms.h"
#define  LOG_TAG    "Transforms"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
//Call corresponding to motion blur
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_motion_1blur(JNIEnv *env,
                                                                        jobject instance,
                                                                        jobject img,
                                                                        jintArray intArgs_) {
    //Declaring a variable of type AndroidBitmapInfo -Give height and width
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;
    //loading info
    ret=AndroidBitmap_getInfo(env, img, &info);
    //Locking Memory for the pixels
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jint *intArgs = env->GetIntArrayElements(intArgs_, NULL);
    //Calling the transform function - Defined in motion_blur.cpp
    //Declared in ImageTransforms.h header
    motion_blur(&info,pixels,intArgs);
    //Unlocking the memory for the pixels after completion of transform
    AndroidBitmap_unlockPixels(env, img);
    env->ReleaseIntArrayElements(intArgs_, intArgs, 0);
}
//Call corresponding to sobel edge filter
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_sobel_1edge_1filter(JNIEnv *env,
                                                                               jobject instance,
                                                                               jobject img,
                                                                               jintArray a_) {
    //Declaring a variable of type AndroidBitmapInfo -Give height and width
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;
    //loading info
    ret=AndroidBitmap_getInfo(env, img, &info);
    //Locking Memory for the pixels
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jint *a = env->GetIntArrayElements(a_, NULL);
    //Calling the transform function - Defined in sobel_edge.cpp
    //Declared in ImageTransforms.h header
    sobel_edge_filter(&info,pixels,a);
    //Unlocking the memory for the pixels after completion of transform
    AndroidBitmap_unlockPixels(env, img);
    env->ReleaseIntArrayElements(a_, a, 0);
}
//Call corresponding to unsharp mask
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_unsharp_1mask(JNIEnv *env,
                                                                         jobject instance,
                                                                         jobject img,
                                                                         jfloatArray floatArgs_) {
    //Declaring a variable of type AndroidBitmapInfo -Give height and width
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;
    //loading info
    ret = AndroidBitmap_getInfo(env, img, &info);
    //Locking Memory for the pixels
    ret = AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jfloat *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    //Calling the transform function - Defined in unsharp.cpp
    //Declared in ImageTransforms.h header
    unsharp_mask(&info, pixels, floatArgs);
    //Unlocking the memory for the pixels after completion of transform
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
//Call corresponding to gaussian
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_gaussian_1blur(JNIEnv *env,
                                                                          jobject instance,
                                                                          jobject img,
                                                                          jintArray intArgs_,
                                                                          jfloatArray floatArgs_) {
    //Declaring a variable of type AndroidBitmapInfo -Give height and width
    AndroidBitmapInfo info;
    int ret;
    void *pixels;
    ret = AndroidBitmap_getInfo(env, img, &info);
    //Locking Memory for the pixels
    ret = AndroidBitmap_lockPixels(env, img, &pixels);
    int *intArgs = env->GetIntArrayElements(intArgs_, NULL);
    float *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    //Calling the transform function - Defined in gaussian.cpp
    //Declared in ImageTransforms.h header
    gaussian_blur(&info, pixels, intArgs, floatArgs);
    //Unlocking the memory for the pixels after completion of transform
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseIntArrayElements(intArgs_, intArgs, 0);
    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
//Call corresponding to motion neon edge
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_neon_1edge(JNIEnv *env, jobject instance,
                                                                      jobject img,
                                                                      jfloatArray floatArgs_) {
    //Declaring a variable of type AndroidBitmapInfo -Give height and width
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;
    //loading info
    ret=AndroidBitmap_getInfo(env, img, &info);
    //Locking Memory for the pixels
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jfloat *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    //Calling the transform function - Defined in neon_edge.cpp
    //Declared in ImageTransforms.h header
    neon_edges(&info,pixels,floatArgs);
    AndroidBitmap_unlockPixels(env, img);
    //Unlocking the memory for the pixels after completion of transform
    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
}