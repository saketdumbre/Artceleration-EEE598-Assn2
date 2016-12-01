/* This file includes the implementation for the neon edges ur filter*/

#include <android/bitmap.h>

#include "ImageTransforms.h"

extern void sobel_edge_filter(AndroidBitmapInfo* info,uint32_t* pixels, int a[]);
extern void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]);

void neon_edges(AndroidBitmapInfo* info, uint32_t *pixels, float f[]){
    uint32_t* sobel_edge = new uint32_t[info->height*info->width];

    //Thhese loops create a copy of tht original image into the array sobel_edge
    for (int y = 0; y <info->height ; ++y) {
        for (int x = 0; x <info->width ; ++x) {
            int index=y*info->width+x;
            sobel_edge[index]=pixels[index];
        }
    }
    int a[1] ={2};
    int r[1]={6*f[0]};
    float sigma[1]={f[0]};
    // Calling the sobel Edge filter with given arguments
    sobel_edge_filter(info,sobel_edge,a);
    // Applying Gaussian Blur to the Sobel edge transformed array
    gaussian_blur(info,sobel_edge,r,sigma);
    int red,green,blue;
    uint32_t *px= pixels;
    //The following loops Scale the sobel-edged,gaussian blurred output by a factor of f[1]
    //and add it to the original image scaled by a value of f[2] to give the output of the
    //NeonEdges transform
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
    //Freeing Men=mory allocated with new[] for sobel_edge array
    delete[] sobel_edge;
}

