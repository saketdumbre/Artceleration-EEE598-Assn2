/* This file includes the implementation for the unsharp mask filter*/

#include <android/bitmap.h>

#include "ImageTransforms.h"

extern void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]);

void unsharp_mask(AndroidBitmapInfo* info, uint32_t *pixels, float f[]){
    //Making a copy of the original image
    uint32_t*original=new uint32_t[info->width*info->height];
    for (int y=0;y<info->height;++y){
        for(int x=0;x<info->width;++x){
            int position = y*info->width+x;
            original[position]=pixels[position];
        }
    }
    //One float argument passed in
    int a[1];
    float sigma[1];
    //Setting 6*f0 as the radius for the gaussian kernel
    a[0]=6*f[0];
    //Setting f0 as the standard deviation for the gaussian kernel
    sigma[0]=f[0];
    int red,green,blue;
    //Calling Gaussian Blur with above parameters
    gaussian_blur(info,pixels,a,sigma);
    //The following loops will scale the difference of the gaussian output and original image
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
    //Free the memory allocated with new
    delete []original;
}
