/*-
 * Copyright (c) 2004-2008 Benjamin Close <Benjamin.Close@clearchain.com>
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
#ifndef SMATRIX_H
#define SMATRIX_H

#include <wcl/api.h>
#include "Matrix.h"
namespace wcl
{

class WCL_API SMatrix : public Matrix
{
public:
    SMatrix( unsigned );
    explicit SMatrix( const Matrix & );
    SMatrix( const SMatrix & );
    virtual ~SMatrix();

    void setSize( unsigned );

    SMatrix operator +( const SMatrix & ) const;
    SMatrix operator -( const SMatrix & ) const;
    SMatrix operator -() const;
    SMatrix operator *( const T & ) const;
    SMatrix operator *( const SMatrix & ) const;
    SMatrix operator /( const T & ) const;
    SMatrix operator /( const SMatrix & ) const;

    // Not inherited
    SMatrix & operator = ( const SMatrix & );
    SMatrix & operator +=( const SMatrix & );
    SMatrix & operator -=( const SMatrix & );
    SMatrix & operator *=( const T & );
    SMatrix & operator *=( const SMatrix & );
    SMatrix & operator /=( const T & );
    SMatrix & operator /=( const SMatrix & );

    SMatrix& storeInverse( const SMatrix & );
    SMatrix& storeIdentity();

    SMatrix& storeOrthographicProjection(T left, T right, T bottom, T top, T near = -1.0, T far = 1.0);

private:
    SMatrix();
    using Matrix::setSize;
};

// Global operators
SMatrix WCL_API operator * ( const T &, const SMatrix & );

// Helper functions
SMatrix WCL_API transpose ( const SMatrix & );
SMatrix WCL_API inv       ( const SMatrix & );
T       WCL_API det       ( const SMatrix & );

}; //end wcl
#endif
