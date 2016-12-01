//Defines two general purpose functions used for the transforms
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include "ImageTransforms.h"
#define PI 3.14

//This function checks if the values of each of the color channel are within the allowed
//pixel value boundaries
int value_check(int color_channel){
    if(color_channel>255) return 255;
    else if(color_channel<0) return 0;
    else return color_channel;
}
//This function returns the sum of square values of the green channel values of two arrays
//Used in the Sobel Edge Filter
int square_add(int position, uint32_t *gr_x, uint32_t *gr_y) {
    int sq1,sq2;
    sq1 = ((gr_x[position] & 0x0000FF00) >> 8) * ((gr_x[position] & 0x0000FF00) >> 8);
    sq2 = ((gr_y[position] & 0x0000FF00) >> 8) * ((gr_y[position] & 0x0000FF00) >> 8);

    return (sq1+sq2);
}


