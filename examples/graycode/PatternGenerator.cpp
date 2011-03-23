/*-
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "PatternGenerator.h"


PatternGenerator::PatternGenerator(const unsigned iwidth, const unsigned iheight ):
    width(iwidth), height(iheight), stage(0)
{

    this->data= new unsigned char [width * height];
};


PatternGenerator::~PatternGenerator()
{
    delete this->data;
}


void PatternGenerator::next()
{
    this->stage++;
}

const unsigned char *PatternGenerator::generate()
{
    return this->generateGreyCodedBinary(this->stage);
//    return this->generateGrid();
}


const unsigned char *PatternGenerator::generateGrid()
{
    unsigned interval = 50; //FIXME: Should be tunable
    unsigned xcount = this->width / interval;
    unsigned ycount = this->height / interval;

    // Vertical Lines
    for( unsigned int x = 0; x <= xcount; x++ )
	this->drawLine( x*interval, 0, x*interval, this->height, 255);

    for( unsigned int y = 0; y <= ycount; y++ )
	this->drawLine( 0, y*interval, this->width, y*interval, 255);

    return this->data;
}

const unsigned char *PatternGenerator::generateGreyCodedBinary(const unsigned stage )
{
    unsigned phase = this->width / (stage+1);
    bool off=true;

    std::cout << phase << std::endl;
    for( unsigned row=0; row < this->height; row++)
	for( unsigned col=0; col < this->width; col++ ){
	    this->setPixel(col,row,off?255:0);
	    if( col % phase == 0 )
		off = !off;
	}

    return this->data;
}

void PatternGenerator::setPixel(const unsigned x, const unsigned y, const unsigned char colour )
{
    assert(x < this->width && "Column out of bounds" );
    assert(y < this->height && "Row out of bounds");

    this->data[(y*this->width)+x]= colour;
}

/**
 * Draw a line using bresenhams algorithm optimised to use integers only.
 * Converted from Pseudocode taken from:
 * http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 */
#define swap(a,b) {int temp = a; a=b; b=temp;}

void PatternGenerator::drawLine(int x0, int y0, int x1, int y1, const unsigned char colour )
{
    bool steep = abs( y1 - y0 ) > abs( x1-x0 );
    if( steep ){
	swap(x0,y0);
	swap(x1,y1);
    }

    if( x0 > x1 ){
	swap(x0,x1);
	swap(y0,y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = deltax / 2;
    int ystep;
    int y = y0;
    if (y0 < y1)
	ystep = 1;
    else
	ystep = -1;

    for (int x = x0; x < x1; x++){
	if (steep)
	    setPixel((unsigned)y,(unsigned)x, colour);
	else
	    setPixel((unsigned)x,(unsigned)y, colour);

	error = error - deltay;
	if (error < 0 ){
	    y = y + ystep;
	    error = error + deltax;
	}
    }
}

