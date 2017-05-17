/*  SpiralSynth
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/ 

/* 
Resonant low pass filter source code. 
This code was originally written by baltrax@hotmail.com 

See <http://www.harmony-central.com/Computer/Programming/resonant-lp-filter.c>
for the full version with explanatory comments, and the maths!
*/ 

#include "iir_filter.h"

BIQUAD ProtoCoef[FILTER_SECTIONS]; 

float iir_filter(float input,FILTER *iir) 
   { 
    unsigned int i; 
    float *hist1_ptr,*hist2_ptr,*coef_ptr; 
    float output,new_hist,history1,history2; 

/* allocate history array if different size than last call */ 

    if(!iir->history) { 
        iir->history = (float *) calloc(2*iir->length,sizeof(float)); 
        if(!iir->history) { 
            printf("\nUnable to allocate history array in iir_filter\n"); 
            exit(1); 
        } 
    } 

    coef_ptr = iir->coef;                /* coefficient pointer */ 

    hist1_ptr = iir->history;            /* first history */ 
    hist2_ptr = hist1_ptr + 1;           /* next history */ 

        /* 1st number of coefficients array is overall input scale factor, 
         * or filter gain */ 
    output = input * (*coef_ptr++); 

    for (i = 0 ; i < iir->length; i++) 
    { 
        history1 = *hist1_ptr;           /* history values */ 
        history2 = *hist2_ptr; 

        output = output - history1 * (*coef_ptr++); 
        new_hist = output - history2 * (*coef_ptr++);    /* poles */ 

        output = new_hist + history1 * (*coef_ptr++); 
        output = output + history2 * (*coef_ptr++);      /* zeros */ 

        *hist2_ptr++ = *hist1_ptr; 
        *hist1_ptr++ = new_hist; 
        hist1_ptr++; 
        hist2_ptr++; 
    } 

    return(output); 
} 

void prewarp( 
    double *a0, double *a1, double *a2, 
    double fc, double fs) 
{ 
    double wp, pi; 

    pi = 4.0 * atan(1.0); 
    wp = 2.0 * fs * tan(pi * fc / fs); 

    *a2 = (*a2) / (wp * wp); 
    *a1 = (*a1) / wp; 
} 

void bilinear( 
    double a0, double a1, double a2,    /* numerator coefficients */ 
    double b0, double b1, double b2,    /* denominator coefficients */ 
    double *k,           /* overall gain factor */ 
    double fs,           /* sampling rate */ 
    float *coef         /* pointer to 4 iir coefficients */ 
) 
{ 
    double ad, bd; 

                 /* alpha (Numerator in s-domain) */ 
    ad = 4. * a2 * fs * fs + 2. * a1 * fs + a0; 
                 /* beta (Denominator in s-domain) */ 
    bd = 4. * b2 * fs * fs + 2. * b1* fs + b0; 

                 /* update gain constant for this section */ 
    *k *= ad/bd; 

                 /* Denominator */ 
    *coef++ = (2. * b0 - 8. * b2 * fs * fs) / bd;         /* beta1 */ 
    *coef++ = (4. * b2 * fs * fs - 2. * b1 * fs + b0) / bd; /* beta2 */ 
                 /* Nominator */ 
    *coef++ = (2. * a0 - 8. * a2 * fs * fs) / ad;         /* alpha1 */ 
    *coef = (4. * a2 * fs * fs - 2. * a1 * fs + a0) / ad;   /* alpha2 */ 
} 

void szxform( 
    double *a0, double *a1, double *a2, /* numerator coefficients */ 
    double *b0, double *b1, double *b2, /* denominator coefficients */ 
    double fc,         /* Filter cutoff frequency */ 
    double fs,         /* sampling rate */ 
    double *k,         /* overall gain factor */ 
    float *coef)         /* pointer to 4 iir coefficients */ 
{ 
                 /* Calculate a1 and a2 and overwrite the original values */ 
        prewarp(a0, a1, a2, fc, fs); 
        prewarp(b0, b1, b2, fc, fs); 
        bilinear(*a0, *a1, *a2, *b0, *b1, *b2, k, fs, coef); 
} 

