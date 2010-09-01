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

#ifndef WCL_CAMERA_CAMERAEXCEPTION_H
#define WCL_CAMERA_CAMERAEXCEPTION_H

#include <wcl/api.h>
#include <wcl/Exception.h>

namespace wcl {

class WCL_API CameraException: public Exception
{
public:
    static const char *EACCESS;
    static const char *NOTFOUND;
    static const char *UNKNOWN;
    static const char *NOCAPTURE;
    static const char *INVALIDFORMAT;
    static const char *BUFFERERROR;
    static const char *EXPOSUREERROR;
    static const char *CONTROLERROR;
    static const char *BRIGHTNESSERROR;
    static const char *IRISERROR;
    static const char *GAINERROR;
    static const char *FRAMERATEERROR;

    CameraException(const char *);
    virtual ~CameraException() throw();
    virtual const char *what() const throw();

private:
    const char *reason;
};

}; // namespace wcl


#endif
