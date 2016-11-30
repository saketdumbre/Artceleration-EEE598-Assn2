//
// Created by Deepthy on 11/30/2016.
//
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include "ImageTransforms.h"


//extern void gaussian_blur(AndroidBitmapInfo *info, uint32_t *pixels, int a[], float sigma[]);

void unsharp_mask(AndroidBitmapInfo* info, uint32_t *pixels, float f[]){
    //LOGI("Starting UnSharp");
    //Making a copy of the original image
    uint32_t*original=new uint32_t[info->width*info->height];
    for (int y=0;y<info->height;++y){
        for(int x=0;x<info->width;++x){
            int position = y*info->width+x;
            original[position]=pixels[position];
            //  LOGI("%d %d",x,y);
        }
    }
   // LOGI("Creating Original Copy");
    int a[1];
    float sigma[1];
    a[0]=6*f[0];
    sigma[0]=f[0];
    int red,green,blue;
    //Calling Gaussian Blur with radius=6*standard deviation
    gaussian_blur(info,pixels,a,sigma);
//    LOGI("Gaussian Blur Done");
//    LOGI("Scaled Subtraction and Addition");
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
    //LOGI("Exiting Unsharp");
}
