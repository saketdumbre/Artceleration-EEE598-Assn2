/* This file includes the implementation for the gaussian filter
 * using the gaussian vector kernel method*/
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include "ImageTransforms.h"
#define PI 3.14

extern int value_check(int color_channel);

void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]) {
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
    //This loop computes the gaussian Kernel Method
    for(int k=-1*r;k<=r;k++){
        g[k] = constant_2*exp(-1*((k*k)/(constant_1)));
    }
    //The following set of loops applys the gaussian kernel and sums it for pixels within r(radius)
    // number rows of the pixel in consideration
    for (int y = 0; y < image_height; y++){
        for (int x = 0; x < image_width; x++){
            int position = y*image_width+x;
            red=green=blue=0;
            for(int k=-r;k<=r;k++){
                int pos = y*image_width+(x+k);
                if((x+k)>=0 && ((x+k)<image_width)) {
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
    //The following set of loops applys the gaussian kernel and sums it for pixels within r(radius)
    // number columns of the pixel in consideration
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
    //Free the memory allocated with new[] for the q_array and gaussian kernel
    delete[] q_array;
    delete[] g;
}
