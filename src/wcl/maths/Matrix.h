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
 *
 * NOTE: Parts of this class are based on the unlicense AptMatrix class by
 * Aaron Toney <Aaron.Toney@hhhh.org>
 */
#ifndef MATRIX_H
#define MATRIX_H

/**
 * T defines the type for all matrix operations. If you alter T
 * you'll need to recompile the library as well
 */
typedef double T;

/**
 * Representation of a NxM matrix. Memory for this matrix
 * is dynamically created and zeroed, with little error checking (an assert)
 * It's designed to work with large matricies and has most the functions you'ld
 * expect. The matrix interally stores its matrix as data[row][column] ie row
 * major.
 *
 * If you are working soley in graphics space your probably after the
 * SMatrix class.
 */
class Matrix
{
public:
    Matrix();
    Matrix( const Matrix & );
    Matrix( unsigned row, unsigned column);
    virtual ~Matrix();

    void setSize( unsigned rows, unsigned columns );

    const T *operator[] ( unsigned) const;
    T *operator[] ( unsigned );

    Matrix  operator+ (const Matrix &) const;
    Matrix  operator- (const Matrix &) const;
    Matrix  operator- () const;
    Matrix  operator* (const T &) const;
    Matrix  operator* (const Matrix &) const;
    Matrix  operator/ (const T & ) const;
    Matrix &operator= (const Matrix &);
    Matrix &operator+=(const Matrix &);
    Matrix &operator-=(const Matrix &);
    Matrix &operator*=(const T &);
    Matrix &operator/=(const T &);
    bool operator == (const Matrix &) const;
    bool operator !=(const Matrix &);

    unsigned getRows() const;
    unsigned getCols() const;

    void storeTranspose( const Matrix & );
    void storeProduct( const Matrix &,  const Matrix & );
    void storeZeros();

    void print () const;

private:
    T **data;

    unsigned rows;
    unsigned cols;
};

// Global Operators
Matrix operator *( const T &, const Matrix & );

// Helper functions
Matrix transpose ( const Matrix & );

#endif
