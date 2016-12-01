/* This file includes the implementation for the motion blur filter*/
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include "ImageTransforms.h"
//Motion blur averages the nearby pixels and is either applied in the horizontal direction or vertical
void motion_blur(AndroidBitmapInfo* info,uint32_t *pixels,int a[]){
    int r = a[1];
    int a0= a[0];
    int red,green,blue;
    uint32_t *px=pixels;
    if(a0==0){
        //a0==0 corresponds to hozintal motion blur
        // the average of pixel values in a range of -r to r rows of pixel in consideration
        //any out of bound pixel is assigned 0
        for (int y = 0; y <info->height ; ++y) {
            for (int x = 0; x <info->width ; ++x) {
                red = green = blue = 0;
                int out_of_bounds_pixel = 0;
                int position = y * info->width + x;
                for (int k = -1 * r; k < r; ++k) {
                    int pos = y * info->width + (x + k);
                    if (x + k < 0 || x + k >= info->width) {
                        red += out_of_bounds_pixel;
                        green += out_of_bounds_pixel;
                        blue += out_of_bounds_pixel;
                    }
                    else {
                        red += (px[pos] & 0x00000FF);
                        green += ((px[pos] & 0x0000FF00) >> 8);
                        blue += ((px[pos] & 0x00FF0000) >> 16);
                    }

                }
                //Computing the average
                red = red / (2 * r + 1);
                green = green / (2 * r + 1);
                blue = blue / (2 * r + 1);
                px[position]=((blue << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00) |
                             (red & 0x000000FF);
            }
        }
    }
    else if (a0==1){
        //a0==1 corresponds to Vertical motion blur
        // the average of pixel values in a range of -r to r columns of pixel in consideration
        //any out of bound pixel is assigned 0
        for (int x = 0; x <info->width ; ++x){
             for (int y = 0; y <info->height ; ++y) {
                red = green = blue = 0;
                int out_of_bounds_pixel = 0;
                int position = y * info->width + x;
                for (int k = -1 * r; k < r; ++k) {
                    int pos = (y+k) * info->width +x;
                    if (y + k < 0 || y + k >= info->height) {
                        red += out_of_bounds_pixel;
                        green += out_of_bounds_pixel;
                        blue += out_of_bounds_pixel;
                    }
                    else {
                        red += (px[pos] & 0x00000FF);
                        green += ((px[pos] & 0x0000FF00) >> 8);
                        blue += ((px[pos] & 0x00FF0000) >> 16);
                    }

                }
                 //Computing averagees
                red = red / (2 * r + 1);
                green = green / (2 * r + 1);
                blue = blue / (2 * r + 1);
                px[position]=((blue << 16) & 0x00FF0000) | ((green << 8) & 0x0000FF00) |
                             (red & 0x000000FF);
            }
        }

    }
}