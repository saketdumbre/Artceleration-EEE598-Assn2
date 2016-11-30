#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>

#include "ImageTransforms.h"
#define  LOG_TAG    "Transforms"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_sobel_1edge_1filter(JNIEnv *env,
                                                                               jobject instance,
                                                                               jobject img,
                                                                               jintArray a_) {
//    LOGI("JNI: - Sobel Edge Filter");
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;

    ret=AndroidBitmap_getInfo(env, img, &info);
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jint *a = env->GetIntArrayElements(a_, NULL);
//    LOGI("Locking pixels");
    sobel_edge_filter(&info,pixels,a);

//    LOGI("Unlocking");
    AndroidBitmap_unlockPixels(env, img);
    env->ReleaseIntArrayElements(a_, a, 0);
}
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_unsharp_1mask(JNIEnv *env,
                                                                         jobject instance,
                                                                         jobject img,
                                                                         jfloatArray floatArgs_) {
    //    LOGI("JNI: - Unsharp Mask");
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;

    ret = AndroidBitmap_getInfo(env, img, &info);
    ret = AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jfloat *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    //    LOGI("Locking pixels");
    unsharp_mask(&info, pixels, floatArgs);

    //    LOGI("Unlocking");
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_gaussian_1blur(JNIEnv *env,
                                                                          jobject instance,
                                                                          jobject img,
                                                                          jintArray intArgs_,
                                                                          jfloatArray floatArgs_) {
    //        LOGI("JNI: - Gaussian Blur");
    AndroidBitmapInfo info;
    int ret;
    void *pixels;
    ret = AndroidBitmap_getInfo(env, img, &info);
    ret = AndroidBitmap_lockPixels(env, img, &pixels);
    int *intArgs = env->GetIntArrayElements(intArgs_, NULL);
    float *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);

    //        LOGI("Locking pixels");
    gaussian_blur(&info, pixels, intArgs, floatArgs);

    //        LOGI("Unlocking");
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseIntArrayElements(intArgs_, intArgs, 0);
    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
}