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

#ifndef WCL_GRAPHICS_STRUCTUREDLIGHT_GRAYCODE
#define WCL_GRAPHICS_STRUCTUREDLIGHT_GRAYCODE

#include <wcl/maths/Vector.h>
#include <wcl/maths/Matrix.h>

namespace wcl {

/**
 * A class that provides structured light via the GrayCode encoding method.
 * Given a projector (pwidth, pheight) and a camera (cwidth,cheight) the class
 * builds the required sequence of images that are needed to represent every
 * pixel in the projector image, allowing detection by the camera. The actual
 * projection and detection must be performed external to this class. Once all
 * the required images have been captured (cwidthxcheightxmono8) then they can
 * be fed to the decode method.
 *
 * At the end of the decode method a mask (getMaskImage) and the offset
 * row/column for each camera coordinate can be found (getRowCol).
 * Additionally a maskedDebugImage (showing detection masked) or a raw debug
 * image can be used to display detection results.
 */
class WCL_API GrayCode
{
public:
    GrayCode(const unsigned pwidth, const unsigned pheight, const unsigned cwidth, const unsigned cheight );
    ~GrayCode();

    /**
     * Reset the internal decoder  to the first image, clear all results from
     * the previous decoding
     */
    void reset();

    /**
     * Cycle to the next image needed for decoding
	 * @return true if there is another image to show
     */
    bool next();

    /**
     * Obtain the image to be displayed
     */
    const unsigned char *generate();

    wcl::Vector /*ProjectorPixel*/ getRowCol(const wcl::Vector &cameraPixel) const;
    bool isValidRowCol(const wcl::Vector &cameraPixel) const;
    void decode(const unsigned char **capturedImages, const unsigned int threshold=64);

    /**
     * Obtain a list of images required to be captured
     */
    unsigned getRequiredImageCount() const;

    /**
     * Obtain an image showing raw decoding output with no mask
     */
    const unsigned char *getDebugImage();

    /**
     * Obtain a mask of valid detected pixels. The mask is constructed during
     * the decoding routine. As decoding takes place, the mask is generated.
     * The mask image contains values of 0 or 255 so can be used directly with
     * OpenGL textures. The returned image size is cwidthxcheight
     */
    const unsigned char *getMaskImage();

    /**
     * Obtain the merged debug image and mask image, showing only valid
     * detection details
     */
    const unsigned char *getMaskedDebugImage();

    /**
     * Get images that can be used for displaying. These are pwidhtxpheight
     */
    const unsigned char **getCodedImages();

private:
    unsigned pwidth;
    unsigned pheight;
    unsigned totalGrayCodes;
    unsigned grayCodeRowCount;
    unsigned grayCodeRowPhase;
    unsigned grayCodeColumnCount;
    unsigned grayCodeColumnPhase;
    unsigned char **codedImages;
    unsigned stage;

    wcl::Matrix decodedColumns;
    wcl::Matrix decodedRows;
    unsigned cwidth;
    unsigned cheight;
    unsigned char *mask;

    void setPixel(unsigned char *data, 
			      const unsigned x, const unsigned y, const unsigned stride, 
				  const unsigned char colour );

    void createStorage();
    void buildGrayCodes();
};

}; // namespace wcl

#endif
