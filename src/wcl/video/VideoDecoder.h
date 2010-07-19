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
#ifndef WCL_VIDEO_VIDEODECODER_H
#define WCL_VIDEO_VIDEODECODER_H

#include <string>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
};

namespace wcl
{

    /**
     * VideoDecoder provides the means to decode a video off disk or from memory
     * which has been created with any codec that libAVCodec supports and get a
     * frame from the video. The class does it's own internal timing if a disk
     * video is used so the next frame obtained is correct in the sequence of
     * the video.
     */
    class VideoDecoder
    {
    public:
	VideoDecoder(const std::string &path) throw (const std::string &);
	VideoDecoder(const unsigned width, const unsigned height, const CodecID codec );
	~VideoDecoder();

	void nextFrame(const unsigned char *inputbuffer, const unsigned buffersize);

	/**
         * Obtain a pointer to the next frame of the video. This frame will
	 * always be returned in RGB24 (R8,G8,B8) format.
	 *
	 * @return A pointer to the current frame
	 */
	const unsigned char *getFrame();

	unsigned getHeight() const;
	unsigned getWidth() const;

    private:
	uint8_t *buffer;
	AVFrame *someFrame;
	AVFrame *RGBFrame;
	AVCodecContext *codecContext;
	AVFormatContext *formatContext;
	SwsContext *imageConvertContext;
	int isvalid;
	unsigned width;
	unsigned height;
	int index;

	/**
	 * Find the nth video stream in the avcodec context and return the
	 * stream number of that stream. 
	 *
	 * @param nth Indicate we are after the nth video stream (0 indexed)
	 * @return -1 if the video stream is not found or a avcoded stream  context numberk
	 */
	int findVideoStream(const int nth = 0);
	void findDecoder(const enum CodecID id) throw (const std::string &);

	/**
	 * Routines to aid in converting from format X to RGB
	 */
	void allocateConversionBuffer(const unsigned width, const unsigned height);
	void destroyConversionBuffer();

	/**
	 * Initialise AvCodec Library if not already initialised
	 */
	static void libraryInit();

    };

};

#endif
