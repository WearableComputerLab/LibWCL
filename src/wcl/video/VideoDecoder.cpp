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
#include "VideoDecoder.h"

namespace wcl
{

VideoDecoder::VideoDecoder(const unsigned iwidth, const unsigned iheight):
    width(iwidth),height(iheight)
{
    avcodec_init();
    avcodec_register_all();

    AVCodec *codec = avcodec_find_decoder(CODC_ID_MPEG1VIDEO);

    this->context = avcodec_alloc_context();
    this->YUVframe = avcodec_alloc_frame();
    this->RGBframe = avcodec_alloc_frame();
    int size = avpicture_get_size(PIX_FMT_RGB24,this->width, this->height);

    this->buffer = new uint8_t[size];

    avcodec_open(context, codec);
}

VideoDecoder::~VideoDecoder()
{
    avcodec_close(context);
    av_free(RGBframe);
    av_free(YUVframe);
    delete [] this->buffer;
}

void VideoDecoder::nextFrame(const unsigned char *ibuffer, const unsigned buffersize)
{
    bool isvalid;

    avcodec_decode_video(this->context, this->YUVframe, &isvalid, &ibuffer, buffersize);
}

const unsigned char *VideoDecoder::getFrame()
{
    // Output yuv
    img_convert(this->RGBframe, PIX_FMT_RGB24, this->YUVFrame, PIX_FMT_YUV420P, this->width, this->height );

    return image;
}

};
