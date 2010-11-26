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
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "GrayCode.h"


GrayCode::GrayCode(const unsigned iwidth, const unsigned iheight ):
    width(iwidth), height(iheight), totalGrayCodes(0), images(NULL),stage(0)
{
    this->buildGrayCodes();
};


GrayCode::~GrayCode()
{
    if( this->images ){
	for( unsigned i = 0; i <  this->totalGrayCodes; i++ )
	    delete this->images[i];

	delete this->images;
    }
}

void GrayCode::next()
{
    this->stage++;
    if( this->stage >= this->totalGrayCodes)
	this->stage=0;
}

const unsigned char *GrayCode::generate()
{
    return this->images[this->stage];
}

void GrayCode::buildGrayCodes()
{
    // Work out how many patterns are needed for the
    // specified rows and columns. Since we are trying to
    // encode a binary sequence, this is: count = log2(x)
    // where x is the width or the height - depending if we
    // are encoding columns or rows. We also have to cater
    // for rounding in c hence we ceil to the next
    // highest value then cast the value to get count;
    unsigned greyCodeColumnCount = (unsigned)ceil(log2(this->width));
    unsigned greyCodeRowCount = (unsigned)ceil(log2(this->height));

    // Record the total amount of patterns required for
    // both the columns and the rows
    this->totalGrayCodes = greyCodeColumnCount + greyCodeRowCount;

    // Create the data storage for the patterns.
    this->createStorage();

    //
    // Build the required grey code images.
    //
    //

    // We start by building the greycoded columns.
    for(unsigned column = 0; column < this->width; column++ ) {
	for ( unsigned imageCount = 0; imageCount < greyCodeColumnCount; imageCount++ ){

	    unsigned char *data = this->images[imageCount];

	    //
	    // Work out the value for this column
	    //

	    // Binary encoding. Put basically, a particular column over
	    // greyCodeColumnCount images represents a binary number. What
	    // we do here is work out what bit value that number is for this
	    // particular (imageCount) image. Ie the number for column 0 is
	    // always 0..n  where n is greyCodeColumnCount long. Column 1 always
	    // equates to 1 in binary. Hence in image 0 column 1 is black. In
	    // image greyCodeColumnCount column 1 is white.
	    //
	    // To work what colour (white/black) a column should be in this particular image
	    // we need to work out the bit value for this image. Hence if column =10
	    // which is binary n-4..1010 and we're in the second to last image the
	    // column must be white. To calculate colour we bitshift the column
	    // count (ie the value for the column) by the imagecount we are in.
	    // This shifts the bit we care about down to the least significant
	    // bit. We can simply test it using a bit comparison against 1 to
	    // then see if we need to be white or black.
	    // Now the only issue with this is we work from least significant
	    // bit up to most siginificate bit. This makes decoding the number
	    // harder. Hence instead we shift by greyCodeColumnCount-imageCount.
	    // This then works from most significant bit down to least
	    // significant bit which makes deciding easier.
	    //

	    unsigned char value;

	    value  = ( toGrayCode(column) >> (greyCodeColumnCount - imageCount -1)) & 1;

	    // We now update the value of the column to be in the range 0
	    // (black) or 255 (white). As this is what GL expects for a texture
	    value *=255;

	    // Store the value in the image
	    this->setPixel(data,column,0 /* y */, value);

	    // Copy the value down for all rows of the image.
	    // after this the entire column for this image is set
	    for( unsigned row = 1; row < this->height; row++ )
		this->setPixel(data,column,row, value);
	}
    }

    //
    // We now repeat the above setup but focus on the rows not
    // the columns
    //
    for( unsigned row=0; row < this->height; row++){
	for ( unsigned imageCount = 0; imageCount < greyCodeRowCount; imageCount++ ){
	    unsigned char *data = this->images[imageCount + greyCodeColumnCount];
	    unsigned char value  = ( row >> (greyCodeRowCount - imageCount -1)) & 1;

	    // We now update the value of the column to be in the range 0
	    // (black) or 255 (white). As this is what GL expects for a texture
	    value *=255;

	    // Store the value in the image
	    this->setPixel(data,0 /* X */,row, value);

	    // Copy the value down for all rows of the image.
	    // after this the entire column for this image is set
	    for( unsigned column = 1; column < this->width; column++ )
		this->setPixel(data,column,row, value);
	}
    }
}

/**
 * Create the memory required for all the required grey code images
 */
void GrayCode::createStorage()
{
    this->images = new unsigned char *[this->totalGrayCodes];
    for( unsigned i = 0; i < this->totalGrayCodes; i++ )
	this->images[i] = new unsigned char [this->width * this->height];
}

void GrayCode::setPixel(unsigned char *data, const unsigned x, const unsigned y, const unsigned char colour )
{
    assert(x < this->width && "Column out of bounds" );
    assert(y < this->height && "Row out of bounds");

    data[(y*this->width)+x]= colour;
}

/**
 * Given a grey code, obtain the row/column that represents that column
 */
wcl::Vector GrayCode::getRowCol(const unsigned rowcode, const unsigned columncode) const
{
    wcl::Vector v(2);

    // Decode the greycode back into an x, y from the projector
    v[0] = GrayCode::fromGrayCode(columncode);
    v[1] = GrayCode::fromGrayCode(rowcode);

    return v;
}


unsigned int GrayCode::fromGrayCode(const unsigned value)
{
    unsigned int temp = value ^ (value>>8);
    temp ^= (temp>>4);
    temp ^= (temp>>2);
    temp ^= (temp>>1);
    return temp;
}

unsigned int GrayCode::toGrayCode(const unsigned value)
{
    return ( value >> 1 ) ^ (value);
}
