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
#include "VideoDecoder.h"

namespace wcl
{

VideoDecoder::VideoDecoder(const std::string &path , const bool iautofpslimit)
    throw( const std::string &):
	isvalid(false),playedFrames(0),autoFPSLimit(iautofpslimit)
{
    VideoDecoder::libraryInit();

    if( av_open_input_file( &this->formatContext, path.c_str(), NULL, 0, NULL) != 0){
	throw std::string("Unable To Open Video File");
    }

    if(av_find_stream_info(this->formatContext) < 0 )
	throw std::string("No Video Stream Information Exists");

#ifdef DEBUG
    dump_format(this->formatContext, 0, 0, false);
#endif

    if( (this->index = this->findVideoStream()) == -1 )
	throw std::string("No Video Streams Exist");

    // Store the codec context
    this->codecContext = this->formatContext->streams[this->index]->codec;

    this->findDecoder(this->codecContext->codec_id);

    this->allocateConversionBuffer( this->codecContext->width,
				    this->codecContext->height );

    this->width = this->codecContext->width;
    this->height = this->codecContext->height;

    this->startTime = av_gettime();
}

VideoDecoder::VideoDecoder(const unsigned iwidth, const unsigned iheight,
			   const CodecID codec, const bool iautofpslimit):
    formatContext(NULL),
    isvalid(false),width(iwidth),height(iheight),
    index(-1),playedFrames(0),autoFPSLimit(iautofpslimit)
{
    VideoDecoder::libraryInit();

    this->codecContext = avcodec_alloc_context();
    this->codecContext->width = iwidth;
    this->codecContext->height = iheight;

    /**
     * XXX This is not right. How can we guarentee that the input format is
     * YUV422 - this should be able to be either specified in the constructor
     * else av* should be able to work it out. For now we hard code it
     * - benjsc 20100719 
     */
    this->codecContext->pix_fmt= PIX_FMT_YUV422P;

    this->findDecoder(codec);
    this->allocateConversionBuffer( this->width, this->height );

    this->startTime = av_gettime();
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
	if( this->formatContext->streams[i]->codec->codec_type ==CODEC_TYPE_VIDEO){
	    if( vstreams == nth )
		return i;
	    else {
		vstreams++;
	    }
	}
    }

    return -1;
}

void VideoDecoder::findDecoder(const enum CodecID id)
    throw (const std::string &)
{
    AVCodec *codec = avcodec_find_decoder(id);
    if( codec == NULL )
	throw std::string("Decoding Codec not found");

#if 0
    // Handle partial frames, not sure if this is useful
    if(codec->capabilities & CODEC_CAP_TRUNCATED)
	this->codecContext->flags |=CODEC_FLAG_TRUNCATED;
#endif

    if(avcodec_open(this->codecContext, codec)<0)
	throw std::string("Unable to open decoding Codec");

    // Hack to correct wrong frame rates that seem to be generated by some
    // codecs
    if(this->codecContext->time_base.num>1000 && this->codecContext->time_base.den==1)
	this->codecContext->time_base.den=1000;

}

VideoDecoder::~VideoDecoder()
{
    this->destroyConversionBuffer();
    avcodec_close(this->codecContext);

    if( this->formatContext ){
	av_close_input_file(this->formatContext);
    }
    else
	av_free(this->codecContext);
}


void VideoDecoder::nextFrame(const unsigned char *ibuffer, const unsigned buffersize)
{
    avcodec_decode_video(this->codecContext, this->someFrame, &this->isvalid,
			 (uint8_t *)ibuffer, buffersize);

}

const unsigned char *VideoDecoder::getFrame()
{
    // FormatContext is only defined if we are reading from a file. In that
    // case, next frame will not be called hence we must read from the file
    // We also rate limit based on the internal playback rate of the file
    if( this->formatContext ){
	AVPacket packet;
	bool found=false;

	int64_t neededFrame=0;;

	// Check to see if we are being called faster than the movie should be
	// played back if we are and we are limiting the frames, simply return
	// the same buffer;
	if( this->autoFPSLimit ){
	    neededFrame= (int)(((av_gettime()-this->startTime)/1000000.0)*this->getFPS());
	    if(neededFrame <= this->playedFrames )
		return (unsigned char *)this->buffer;
	}

	// Keep processing frames until we find the next frame we are
	// after. This may be more than one frame if autofps limiting is enabled
	while(av_read_frame(this->formatContext, &packet) >= 0 && !found){
	    if( packet.stream_index==this->index){
		avcodec_decode_video(this->codecContext, this->someFrame,
				     &this->isvalid, packet.data, packet.size);
		if( this->isvalid ){
		    this->playedFrames++;
		    sws_scale(this->imageConvertContext,
			      this->someFrame->data, this->someFrame->linesize,
			      0, this->height,
			      this->RGBFrame->data, this->RGBFrame->linesize);

		    // If we are rate limiting, keep decoding frames until
		    // we catch up to where we should be
		    if(this->autoFPSLimit ){
			if(this->playedFrames >= neededFrame )
			    found=true;
		    }
		    // Else we simply return the next frame
		    else {
			found=true;
		    }
		}
	    }
	    av_free_packet(&packet);
	}

	// No more frames in the file
	if(!found)
	    return NULL;
    } else {
	//
	// Stream decoding
	//
	if( this->isvalid ){
		this->playedFrames++;
		sws_scale(this->imageConvertContext,
			  this->someFrame->data, this->someFrame->linesize,
			  0, this->height,
			  this->RGBFrame->data, this->RGBFrame->linesize);
	}
    }
    return (unsigned char *)this->buffer;
}


void VideoDecoder::libraryInit()
{
    static int init;
    if(!init){
	av_register_all();
	init=1;
    }
}

void VideoDecoder::allocateConversionBuffer(const unsigned width, const unsigned height)
{
    this->someFrame = avcodec_alloc_frame();
    this->RGBFrame = avcodec_alloc_frame();
    int size = avpicture_get_size(PIX_FMT_RGB24, width, height);
    this->buffer = new uint8_t[size];
    avpicture_fill((AVPicture *)this->RGBFrame, this->buffer, PIX_FMT_RGB24, width, height);
    this->imageConvertContext =  sws_getContext( width, height,
						 this->codecContext->pix_fmt,
						 width, height,
						 PIX_FMT_RGB24, SWS_BICUBIC,
						 NULL, NULL, NULL );
}

void VideoDecoder::destroyConversionBuffer()
{
    delete [] this->buffer;

    av_free(this->RGBFrame);
    av_free(this->someFrame);
    sws_freeContext( this->imageConvertContext);
}

unsigned VideoDecoder::getWidth() const
{
    return this->width;
}

unsigned VideoDecoder::getHeight() const
{
    return this->height;
}

float VideoDecoder::getFPS() const
{
    if( this->formatContext &&  this->autoFPSLimit ){
	return(1.0 *
	       this->formatContext->streams[this->index]->r_frame_rate.num /
	       (float) this->formatContext->streams[this->index]->r_frame_rate.den);
    } else {
	float t = (av_gettime()-this->startTime)/1000000.0;
	return (t>1? this->playedFrames / t : 0);
    }
}

int64_t VideoDecoder::getCurrentFrame() const
{
    return this->playedFrames;
}

int64_t VideoDecoder::getLastFrame() const
{
    if( this->formatContext )
	return this->formatContext->streams[this->index]->nb_frames;

    return -1;
}

void VideoDecoder::rewind()
{
	if( this->formatContext )
	{
		av_seek_frame(this->formatContext, this->index,0, AVSEEK_FLAG_BACKWARD);
		avcodec_flush_buffers(this->codecContext);
	}
}

bool VideoDecoder::atEnd() const
{
    if( this->formatContext && this->getCurrentFrame() >= this->getLastFrame())
		return true;
    return false;
}

};



