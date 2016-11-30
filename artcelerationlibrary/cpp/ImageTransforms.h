//
// Created by Deepthy on 11/30/2016.
//

#ifndef ARTCELERATION_IMAGETRANSFORMS_H
#define ARTCELERATION_IMAGETRANSFORMS_H
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>

int square_add(int position, uint32_t *pInt, uint32_t *y);
int value_check(int color_channel);
void get_gradient_y(AndroidBitmapInfo* info,int* q_array,uint32_t* pixels);
void get_gradient_x(AndroidBitmapInfo* info,int* q_array,uint32_t* pixels);

void sobel_edge_filter(AndroidBitmapInfo* info,uint32_t* pixels, int a[]);
void gaussian_blur(AndroidBitmapInfo* info, void* pixels, int a0[], float f0[]);
void neon_edges(AndroidBitmapInfo* info, uint32_t *pixels, float f[]);
void unsharp_mask(AndroidBitmapInfo* info, uint32_t *pixels, float f[]);
#endif //ARTCELERATION_IMAGETRANSFORMS_H
