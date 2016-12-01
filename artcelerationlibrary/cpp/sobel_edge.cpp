/* This file includes the implementation for the sobel edge filter
 * when the argument =0 - gradient_x is implemented
 * when the argument=1 - gradient_y is implemented
 * when */
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <math.h>
#include "ImageTransforms.h"
#define PI 3.14

extern int value_check(int color_channel);
extern int square_add(int position, uint32_t *gr_x, uint32_t *gr_y);

/*Calculates the gradient using the Sx sobel operator*/
void get_gradient_x(AndroidBitmapInfo* info,int* q_array,uint32_t* pixels){
    //Defining the sobel operator
    int sx[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
    uint32_t *px = (uint32_t *) pixels;
    //The following loops compute the individual product elements obtained by multiplying the
    //sx matrix with the corresponding pixels
    //The green channel is chosen for computation
    for (int y = 0; y < info->height; ++y) {
        for (int x = 0; x <info->width ; ++x) {
            int position=y*info->width+x;
            int p1,p2,p3,p4,p5,p6,p7,p8,p9;
            p1=p2=p3=p4=p5=p6=p7=p8=p9=0;
            if(x-1>=0) {
                if (y - 1 >= 0)
                    p1 = sx[0][0] * ((q_array[(y - 1)*info->width + (x - 1)] & 0x0000FF00) >> 8);
                p4 = sx[0][1] * ((q_array[(y) * info->width + (x - 1)] & 0x0000FF00) >> 8);
                if(y+1<info->height)
                    p7 = sx[0][2] * ((q_array[(y+1) * info->width + (x - 1)] & 0x0000FF00) >> 8);
            }
            if(x+1<info->width){
                if(y+1<info->height)
                    p9 = sx[2][2] * ((q_array[(y+1) * info->width + (x + 1)] & 0x0000FF00) >> 8);
                if (y - 1 >= 0)
                    p3 = sx[2][0] * ((q_array[(y-1) * info->width + (x + 1)] & 0x0000FF00) >> 8);
                p6=sx[2][1] * ((q_array[(y) * info->width + (x + 1)] & 0x0000FF00) >> 8);
            }
            if(y-1>=0)
                p2= sx[1][0] * ((q_array[(y-1) * info->width + (x)] & 0x0000FF00) >> 8);
            if(y+1<info->height)
                p8= sx[1][2] * ((q_array[(y+1) * info->width + (x)] & 0x0000FF00) >> 8);
            p5=sx[1][1] * ((q_array[(y) * info->width + (x)] & 0x0000FF00) >> 8);

            int val;
            //the products are summed and checked for boundary conditions using value check
            val=p1+p2+p3+p4+p5+p6+p7+p8+p9;
            val=value_check(val);
            //All color channels given the same value
            px[position]=((val << 16) & 0x00FF0000) |((val << 8) & 0x0000FF00) |(val & 0x000000FF);

        }
    }
}
/*Calculates the gradient using the Sy sobel operator*/
void get_gradient_y(AndroidBitmapInfo* info,int* q_array,uint32_t* pixels){
    //Defining the sobel operator
    int sy[3][3]={{-1,-2,-1},{0,0,0},{-1,2,1}};
    uint32_t *px = (uint32_t *) pixels;
    //The following loops compute the individual product elements obtained by multiplying the
    //sx matrix with the corresponding pixels
    //The green channel is chosen for computation
    for (int y = 0; y < info->height; ++y) {
        for (int x = 0; x <info->width ; ++x) {
            int position=y*info->width+x;
            int p1,p2,p3,p4,p5,p6,p7,p8,p9;
            p1=p2=p3=p4=p5=p6=p7=p8=p9=0;
            if(x-1>=0) {
                if (y - 1 >= 0)
                    p1 = sy[0][0] * ((q_array[(y - 1)*info->width + (x - 1)] & 0x0000FF00) >> 8);
                p4 = sy[0][1] * ((q_array[(y) * info->width + (x - 1)] & 0x0000FF00) >> 8);
                if(y+1<info->height)
                    p7 = sy[0][2] * ((q_array[(y+1) * info->width + (x - 1)] & 0x0000FF00) >> 8);
            }
            if(x+1<info->width){
                if(y+1<info->height)
                    p9 = sy[2][2] * ((q_array[(y+1) * info->width + (x + 1)] & 0x0000FF00) >> 8);
                if (y - 1 >= 0)
                    p3 = sy[2][0] * ((q_array[(y-1) * info->width + (x + 1)] & 0x0000FF00) >> 8);
                p6=sy[2][1] * ((q_array[(y) * info->width + (x + 1)] & 0x0000FF00) >> 8);
            }
            if(y-1>=0)
                p2= sy[1][0] * ((q_array[(y-1) * info->width + (x)] & 0x0000FF00) >> 8);
            if(y+1<info->height)
                p8= sy[1][2] * ((q_array[(y+1) * info->width + (x)] & 0x0000FF00) >> 8);
            p5=sy[1][1] * ((q_array[(y) * info->width + (x)] & 0x0000FF00) >> 8);
            //the products are summed and checked for boundary conditions using value check
            int val =p1+p2+p3+p4+p5+p6+p7+p8+p9;
            val=value_check(val);
            //All color channels given the same value
            px[position]=((val << 16) & 0x00FF0000) |((val << 8) & 0x0000FF00) |(val & 0x000000FF);

        }
    }
}

/*This function callsget_gradient_x or get_gradient_y or both
 * depending on the value of the int arguments*/
void sobel_edge_filter(AndroidBitmapInfo* info,uint32_t* pixels, int a[]){

    int red,green,blue;
    uint32_t *px=pixels;
    int* q_array= new int[info->height*info->width];
    //creating a grayscale brightness array - q_array
    for(int y=0;y<info->height;++y){
        for(int x=0;x<info->width;++x){
            int position = y*info->width+x;
            red=(pixels[position] & 0x00000FF)* 0.2989;
            green=((pixels[position] & 0x0000FF00) >> 8)*0.5870;
            blue=((pixels[position] & 0x00FF0000) >> 16)*0.1140;
            red=value_check(red);
            green=value_check(green);
            blue=value_check(blue);
            q_array[position]=((blue << 16) & 0x00FF0000) |((green << 8) & 0x0000FF00) |(red & 0x000000FF);
        }
    }
    if(a[0]==0)
        get_gradient_x(info, q_array,px);
    if(a[0]==1)
        get_gradient_y(info,q_array,px);
    if(a[0]==2){
        uint32_t * gr_x =new uint32_t[info->width*info->height];
        get_gradient_x(info,q_array,gr_x);
        uint32_t * gr_y =new uint32_t[info->width*info->height];
        get_gradient_y(info,q_array,gr_y);
        int value;
        for (int y = 0; y <info->height; ++y) {
            for (int x = 0; x <info->width ; ++x) {
                int position=y*info->width+x;
                value =sqrt(square_add(position,gr_x,gr_y));
                green=value_check(value);
                px[position]=((value << 16) & 0x00FF0000) |((value << 8) & 0x0000FF00) |(value & 0x000000FF);
            }
        }
        //freeing memory allocated with new[]for gr_x and gr_y
        delete [] gr_x;
        delete [] gr_y;
    }
    //Freeing memory allocated with new[] for q_array
    delete [] q_array;
}
