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

using namespace std;

GrayCode::GrayCode(const unsigned iwidth, const unsigned iheight ):
    width(iwidth), height(iheight), totalGrayCodes(0), codedImages(NULL),stage(0),
    decodedColumns(iwidth, iheight), decodedRows(iwidth, iheight)
{
    // Work out how many patterns are needed for the
    // specified rows and columns. Since we are trying to
    // encode a binary sequence, this is: count = log2(x)
    // where x is the width or the height - depending if we
    // are encoding columns or rows. We also have to cater
    // for rounding in c hence we ceil to the next
    // highest value then cast the value to get count;
    this->grayCodeColumnCount = (unsigned)ceil(log2(this->width));
    this->grayCodeRowCount = (unsigned)ceil(log2(this->height));

    // When considering the columns and the rows we also have to consider
    // that the width/height is not an exact factor of 2 (ie 800 pixels).
    // Hence we may have to shift the phase of a binary image so that
    // the binary bands line up correctly.
    this->grayCodeColumnPhase = (unsigned)floor((pow(2.0,this->grayCodeColumnCount)-this->width)/2);
    this->grayCodeRowPhase = (unsigned)floor((pow(2.0,this->grayCodeRowCount)-this->height)/2);

    // Record the total amount of patterns required for
    // both the columns and the rows, we also cater for the inverts and the
    // initial images
    this->totalGrayCodes = ((this->grayCodeColumnCount + this->grayCodeRowCount) * 2) +4;

    // Create the data storage for the patterns.
    this->createStorage();

    // Build the various images required for graycode displaying
    this->buildGrayCodes();

    // zero the decode matrixes
    this->decodedColumns.storeZeros();
    this->decodedRows.storeZeros();
};


GrayCode::~GrayCode()
{
    if( this->codedImages ){
	for( unsigned i = 0; i <  this->totalGrayCodes; i++ )
	    delete this->codedImages[i];

	delete this->codedImages;
    }
}

bool GrayCode::next()
{
    this->stage++;
    if( this->stage >= this->totalGrayCodes){
	this->stage=0;
	return false;
    }

    return true;
}

const unsigned char *GrayCode::generate()
{
    return this->codedImages[this->stage];
}

void GrayCode::buildGrayCodes()
{
    //
    // Build the required gray code images.
    // When building the images we build both the initial image and
    // the image invert to aid in detection
    //

    // The first two images are the most sigificant bit and full black/white
    // this allows us to work out the region the graycodes are projected onto
    // We set the initial pattern here we calculate all inverts below
    unsigned char **codedColumnImages = this->codedImages;
    memset( *codedColumnImages, 255, this->width * this->height );
    codedColumnImages+=2;

    // We start by building the graycoded columns.
    for(unsigned column = 0; column < this->width; column++ ) {
	for ( unsigned imageCount = 0; imageCount < this->grayCodeColumnCount; imageCount++ ){

	    unsigned char *data = codedColumnImages[imageCount*2];

	    //
	    // Work out the value for this column
	    //

	    // Binary encoding. Put basically, a particular column over
	    // grayCodeColumnCount images represents a binary number. What
	    // we do here is work out what bit value that number is for this
	    // particular (imageCount) image. Ie the number for column 0 is
	    // always 0..n  where n is grayCodeColumnCount long. Column 1 always
	    // equates to 1 in binary. Hence in image 0 column 1 is black. In
	    // image grayCodeColumnCount column 1 is white.
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
	    // harder. Hence instead we shift by grayCodeColumnCount-imageCount.
	    // This then works from most significant bit down to least
	    // significant bit which makes deciding easier.
	    //
	    // We also take into account that the width and the height of an
	    // image is not a power of two. Hence we offset each column/row by the
	    // relevant phase shift to align the graycodes correctly.
	    //unsigned char value  = (column+this->grayCodeColumnPhase) >> (this->grayCodeColumnCount - imageCount -1) & 1;
	    unsigned char value  = ((column) >> (this->grayCodeColumnCount - imageCount -1)) & 1;

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
    unsigned char **codedRowImages= codedColumnImages+(this->grayCodeColumnCount*2);
    memset( *codedRowImages, 255, this->width * this->height );
    codedRowImages+=2;

    for( unsigned row=0; row < this->height; row++){
	for ( unsigned imageCount = 0; imageCount < this->grayCodeRowCount; imageCount++ ){
	    unsigned char *data = codedRowImages[imageCount*2];
	    //unsigned char value  = (toGrayCode(row+this->grayCodeRowPhase)) >> (this->grayCodeRowCount - imageCount -1) & 1;
	    //unsigned char value  = (toGrayCode(row)) >> (this->grayCodeRowCount - imageCount -1) & 1;
	    unsigned char value  = (row) >> (this->grayCodeRowCount - imageCount -1) & 1;
	    value *=255;
	    this->setPixel(data,0 /* X */,row, value);
	    for( unsigned column = 1; column < this->width; column++ )
		this->setPixel(data,column,row, value);
	}
    }

    //
    // Now create the invert images for each pattern
    //
    for( unsigned count = 0; count < this->totalGrayCodes; count+=2 ){
	    for( unsigned row = 0; row < this->height; row++ ){
		for( unsigned column = 0; column < this->width; column++ ){
		this->codedImages[count+1][row*this->width+column] =
		    this->codedImages[count][row*this->width+column] ? 0 : 255;
	    }
	}
    }
}

/**
 * Create the memory required for all the required gray code images
 */
void GrayCode::createStorage()
{
    this->codedImages = new unsigned char *[this->totalGrayCodes];
    for( unsigned i = 0; i < this->totalGrayCodes; i++ )
	this->codedImages[i] = new unsigned char [this->width * this->height];
}

void GrayCode::setPixel(unsigned char *data, const unsigned x, const unsigned y, const unsigned char colour )
{
    assert(x < this->width && "Column out of bounds" );
    assert(y < this->height && "Row out of bounds");

    data[(y*this->width)+x]= colour;
}

/**
 * Given a camera coo
 */
wcl::Vector GrayCode::getRowCol(const wcl::Vector &input) const
{
    wcl::Vector v(2);

    int col = input[0];
    int row = input[1];

    // Search the row/column matrixes for the input pixel values
    v[0] = this->decodedColumns[col][row];
    v[1] = this->decodedRows[col][row];

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


/**
 * Given the correct amount of images, decode the structured light sequence.
 * Note the images are expected to be in 8 bit grayscale.
 *
 * @param capturedImages the images captured for each structured light sequence
 */
void GrayCode::decode(const unsigned char **capturedImages)
{
    // First we separate the rows from the columns;
    const unsigned char **columnImages = capturedImages;
    const unsigned char **rowImages = capturedImages+ this->grayCodeRowCount;

    // We now begin the process of decoding the images back into the relevant
    // graycodes. The decoding works as follows. When capturing the graycodes
    // we generated both the gray code image and the image invert. By looking
    // at the difference between these two images we find what the bit is at
    // each pixel of the imagepair. For pixels that are not part of the
    // projected image they get assigned a bit value of zero. Ie all undetected
    // pixels are 0.
    for(unsigned columnCount = 0; columnCount < this->grayCodeColumnCount; columnCount++){

	// Obtain pointers to the gray code image and the image invert
	const unsigned char *gray    = columnImages[ columnCount ];
	const unsigned char *invgray = columnImages[ columnCount + 1];

	// Now decode each pixel of the image pair.
	for( unsigned y = 0; y < this->height; y++ ){
	    for( unsigned x = 0; x < this->width; x++ ){

		unsigned offset = (this->width * y) + x;

		// Find the bit value for this pixel in the image
		bool bitvalue = gray[offset] >= invgray[offset];

		// Store the value of this bit in decoded matrix at the correct
		// location. The columnCount indicates the significance of the
		// bit in the number.
		int bit = this->grayCodeColumnCount - columnCount - 1;
		if( bitvalue ){
		    int value = (int)this->decodedColumns[x][y];
		    value |= 1 << bit;
		    this->decodedColumns[x][y] = value;
		}
	    }
	}
    }

    // Repeat for rows
    for(unsigned rowCount = 0; rowCount < this->grayCodeRowCount; rowCount++){

	// Obtain pointers to the gray code image and the image invert
	const unsigned char *gray    = rowImages[ rowCount ];
	const unsigned char *invgray = rowImages[ rowCount + 1];

	// Now decode each pixel of the image pair.
	for( unsigned y = 0; y < this->height; y++ ){
	    for( unsigned x = 0; x < this->width; x++ ){

		unsigned offset = (this->width * y) + x;
		bool bitvalue = gray[offset] >= invgray[offset];
		int bit = this->grayCodeRowCount - rowCount - 1;
		if( bitvalue ){
		    int value = (int)this->decodedRows[x][y];
		    value |= 1 << bit;
		    this->decodedRows[x][y] = value;
		}
	    }
	}
    }
    // Each stored value in the matrixes are gray coded still. We now
    // convert the values back to binary. At the same time we filter to make
    // sure the values are within range of what they should be (ie width or
    // height of the original graycode)
    for(unsigned y; y < this->height; y++ ){
	for(unsigned x; x < this->width; x++){
	    //this->decodedRows[x][y] = fromGrayCode((unsigned) this->decodedRows[x][y]);
	    //this->decodedColumns[x][y] = fromGrayCode((unsigned) this->decodedColumns[x][y]);
	    if( this->decodedRows[x][y] > this->height )
		this->decodedRows[x][y] = 0.0;
	    if( this->decodedColumns[x][y] > this->width )
		this->decodedRows[x][y] = 0.0;
	}
    }
}

unsigned GrayCode::getRequiredImageCount() const
{
    return this->totalGrayCodes;
}


void GrayCode::reset()
{
    this->decodedColumns.storeZeros();
    this->decodedRows.storeZeros();
}

const unsigned char *GrayCode::getDebugImage()
{
    static unsigned char *buffer = new unsigned char[this->width*this->height];


    // Display the values of the detection in the rgb image
    for(unsigned y = 0; y < this->height; y++ ){
	for(unsigned x = 0; x < this->width; x++){
	    this->setPixel(buffer, x, y, (int)((this->decodedColumns[x][y] /
						(float)this->width)) * 255.0);
	}
    }

    return buffer;
}
