#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include <cpu-features.h>
#include <arm_neon.h>
#include "ImageTransforms.h"
#define  LOG_TAG    "Transforms"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//extern value_check(int color_channel);
//extern void get_gradient_x(AndroidBitmapInfo* info,int* q_array,uint32_t* pixels);
//extern void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]);

static void neon_edges(AndroidBitmapInfo* info, uint32_t *pixels, float f[]){
    uint32_t* sobel_edge = new uint32_t[info->height*info->width];

    for (int y = 0; y <info->height ; ++y) {
        for (int x = 0; x <info->width ; ++x) {
            int index=y*info->width+x;
            sobel_edge[index]=pixels[index];
        }
    }
    int a[]={2};
    float floatArgs[]={f[0]};
    sobel_edge_filter(info,sobel_edge,a);
    gaussian_blur(info,sobel_edge,a,floatArgs);
    if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures()
                                                             & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
    {
        LOGI("NEON POSSIBLE");
        int position =0;
        int pixel_count=info->width*info->height;
        while(position<pixel_count) {
            //Loading scaling factors into neon registers
            uint8x8_t sobel_scale = vdup_n_u8(f[1]);
            uint8x8_t original_scale = vdup_n_u8(f[2]);
            //loading rgb values of 8 pixels of sobel_edge array
            uint8x8x3_t rgb = vld3_u8((const unsigned char*)sobel_edge);
        }

    }
    /*else
    {
        // use non-NEON fallback routines to test on personal device
        int red,green,blue;
        uint32_t *px= pixels;
        for (int y = 0; y <info->height ; ++y) {
            for (int x = 0; x <info->width ; ++x) {
                int index=y*info->width+x;
                red=((sobel_edge[index] & 0x00000FF)* f[1])+((pixels[index] & 0x00000FF)* f[2]);
                green=(((sobel_edge[index] & 0x0000FF00) >> 8)*f[1])+(((pixels[index] & 0x0000FF00) >> 8)*f[2]);
                blue=(((sobel_edge[index] & 0x00FF0000) >> 16)*f[1])+(((pixels[index] & 0x00FF0000) >> 16)*f[2]);
                red=value_check(red);
                green=value_check(green);
                blue=value_check(blue);
                px[index]=((blue << 16) & 0x00FF0000) |((green << 8) & 0x0000FF00) |(red & 0x000000FF);
            }
        }
    }*/

    delete[] sobel_edge;
}
extern "C" {
JNIEXPORT void JNICALL
Java_edu_asu_msrs_artcelerationlibrary_ArtTransformService_neon_1edge(JNIEnv *env, jobject instance,
                                                                      jobject img,
                                                                      jfloatArray floatArgs_) {
    AndroidBitmapInfo info;
    int ret;
    uint32_t *pixels;

    ret=AndroidBitmap_getInfo(env, img, &info);
    ret=AndroidBitmap_lockPixels(env, img, (void **) &pixels);
    jfloat *floatArgs = env->GetFloatArrayElements(floatArgs_, NULL);
    neon_edges(&info,pixels,floatArgs);
    AndroidBitmap_unlockPixels(env, img);

    env->ReleaseFloatArrayElements(floatArgs_, floatArgs, 0);
}
}