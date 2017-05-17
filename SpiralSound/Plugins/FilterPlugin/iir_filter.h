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


#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#ifndef _IIR_FILTER
#define _IIR_FILTER

/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */ 

class FILTER{ 
	public:
	FILTER() {history=coef=NULL;}
    unsigned int length;       /* size of filter */ 
    float *history;            /* pointer to history in filter */ 
    float *coef;               /* pointer to coefficients of filter */ 
}; 

#define FILTER_SECTIONS   2   /* 2 filter sections for 24 db/oct filter */ 

typedef struct { 
        double a0, a1, a2;       /* numerator coefficients */ 
        double b0, b1, b2;       /* denominator coefficients */ 
} BIQUAD; 

extern BIQUAD ProtoCoef[FILTER_SECTIONS];     /* Filter prototype coefficients, 
	                                                 1 for each filter section */
													 
float iir_filter(float input,FILTER *iir);
 
void szxform( 
    double *a0, double *a1, double *a2,     /* numerator coefficients */ 
    double *b0, double *b1, double *b2,   /* denominator coefficients */ 
    double fc,           /* Filter cutoff frequency */ 
    double fs,           /* sampling rate */ 
    double *k,           /* overall gain factor */ 
    float *coef);         /* pointer to 4 iir coefficients */ 

void prewarp(double *a0, double *a1, double *a2, double fc, double fs); 

void bilinear( 
    double a0, double a1, double a2,    /* numerator coefficients */ 
    double b0, double b1, double b2,    /* denominator coefficients */ 
    double *k,                                   /* overall gain factor */ 
    double fs,                                   /* sampling rate */ 
    float *coef); 
	
#endif
