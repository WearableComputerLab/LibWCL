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

#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H

#include <wcl/maths/Vector.h>
#include <wcl/maths/Matrix.h>

/**
 * A class that generates different patterns that can be used for
 * projector calibration, object detection, object capture
 */
class GrayCode
{
public:
    GrayCode(const unsigned width, const unsigned height );
    ~GrayCode();

    void reset();
    bool next();
    const unsigned char *generate();

    wcl::Vector getRowCol(const wcl::Vector &) const;
    void decode(const unsigned char **capturedImages, const unsigned int threshold=64);

    static unsigned int toGrayCode( const unsigned int);
    static unsigned int fromGrayCode(const unsigned int);

    unsigned getRequiredImageCount() const;
    const unsigned char *getDebugImage();

    const unsigned char **getCodedImages();

private:
    unsigned width;
    unsigned height;

    unsigned totalGrayCodes;
    unsigned grayCodeRowCount;
    unsigned grayCodeRowPhase;
    unsigned grayCodeColumnCount;
    unsigned grayCodeColumnPhase;
    unsigned char **codedImages;
    unsigned stage;
    wcl::Matrix decodedColumns;
    wcl::Matrix decodedRows;

    void setPixel(unsigned char *, const unsigned, const unsigned, const unsigned char);
    void createStorage();
    void buildGrayCodes();
};

#endif
