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
#ifndef WCL_API_H
#define WCL_API_H

/**
 * This file contains macros to allow us to
 * define the expose scope of each class in the library.
 * It's based on the macros at http://gcc.gnu.org/wiki/Visibility
 */

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define WCL_HELPER_DLL_IMPORT __declspec(dllimport)
  #define WCL_HELPER_DLL_EXPORT __declspec(dllexport)
  #define WCL_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define WCL_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
    #define WCL_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
    #define WCL_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define WCL_HELPER_DLL_IMPORT
    #define WCL_HELPER_DLL_EXPORT
    #define WCL_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define WCL_API and WCL_LOCAL.
// WCL_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// WCL_LOCAL is used for non-api symbols.
#ifdef WCL_DLL // defined if WCL is compiled as a DLL
  #ifdef WCL_DLL_EXPORTS // defined if we are building the WCL DLL (instead of using it)
    #define WCL_API WCL_HELPER_DLL_EXPORT
  #else
    #define WCL_API WCL_HELPER_DLL_IMPORT
  #endif // WCL_DLL_EXPORTS
  #define WCL_LOCAL WCL_HELPER_DLL_LOCAL
#else // WCL_DLL is not defined: this means WCL is a static lib.
  #define WCL_API
  #define WCL_LOCAL
#endif // WCL_DLL

#endif
