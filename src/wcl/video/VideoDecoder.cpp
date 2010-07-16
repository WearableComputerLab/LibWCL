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

VideoDecoder::VideoDecoder(const std::string &path )
{
    int index;

    VideoDecoder::libraryInit();

    if( av_open_input_file( &this->formatContext, path.c_str(), NULL, 0, NULL) != 0){
	throw std::string("Unable To Open Video File");
    }

    if(av_find_stream_info(this->formatContext) < 0 )
	throw std::string("No Video Stream Information Exists");

    if( (index = this->findVideoStream()) == -1 )
	throw std::string("No Video Streams Exist");

    // Store the codec context
    this->codecContext = &this->formatContext->streams[index]->codec;

    this->allocateConversionBuffer( this->codecContext->width,
				    this->codecContext->height );
}

VideoDecoder::VideoDecoder(const unsigned width, const unsigned height, const CodecID codec):
    formatContext(NULL)
{
    VideoDecoder::libraryInit();

    this->codecContext = avcodec_alloc_context();
    this->findDecoder(codec)
    this->allocateConversionBuffer( width, height );
}


int VideoDecoder::findVideoStream(const int nth)
{
    int i;
    int vstreams;

    // In order to find a stream the formatContext must already be initialised
    // this currently only happens for file loads, not when the user specifies 
    // the input format
    assert( this->formatContext != NULL );

    for( i = 0, vstreams=0; i < this->formatContext->nb_streams; i++ ){
	if( formatContext->streams[i]->codec.codec_type ==CODEC_TYPE_VIDEO){
	    if( vstreams == nth )
		return i;
	    else {
		vstream++;
	    }
	}
    }

    return -1;
}

void VideoDecoder::findDecoder(const enum CodecID id)
{
    AVCodec *codec = avcodec_find_decoder(id);
    if( codec == NULL )
	throw std::string("Decoding Codec not found");

    // Handle partial frames
    if(codec->capabilities & CODEC_CAP_TRUNCATED)
	this->codecContext->flags |=CODEC_FLAG_TRUNCATED;


    if(avcodec_open(this->codecContext, codec)<0)
	throw std::string("Unable to open decoding Codec");
}

VideoDecoder::~VideoDecoder()
{
    this->destroyConversionBuffer();
    avcodec_close(this->codecContext);

    if( this->formatContext )
	av_close_input_file(this->formatContext);
}


void VideoDecoder::nextFrame(const unsigned char *ibuffer, const unsigned buffersize)
{
    avcodec_decode_video(this->codecContext, this->someFrame, &this->isvalid, &ibuffer, buffersize);
}

const unsigned char *VideoDecoder::getFrame()
{
    if( this->isValid )
	img_convert(this->RGBframe, PIX_FMT_RGB24, this->SomeFrame,
		    this->codecContext->pix_fmt, this->codecContext->width,
		    this->codecContext->height);

    return this->RGBFrame;
}

VideoDecoder &VideoDecoder::createFromFile(const std::string &path)
{
   AVFormatContext *pFormatCtx;
    VideoDecoder decoder(const std::string &path);

    return VideoDecoder;
}

void VidoeDecoder::libraryInit()
{
    static int init;
    if(!init){
	avcodec_init();
	avcodec_register_all();
	init=1;
    }
}

void VideoDecoder::allocateConversionBuffer(const unsigned width, const unsigned height)
{
    this->someFrame = avcodec_alloc_frame();
    this->RGBframe = avcodec_alloc_frame();
    int size = avpicture_get_size(PIX_FMT_RGB24, width, height);
    this->buffer = new uint8_t[size];
}

void VideoDecoder::destroyConversionBuffer()
{
    delete [] this->buffer;

    av_free(this->RGBframe);
    av_free(this->someFrame);
}

};



