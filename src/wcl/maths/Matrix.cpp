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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Matrix.h"

namespace wcl {


/**
 * Constructor:
 */
Matrix::Matrix()
{
    this->rows = 0;
    this->cols = 0;
    this->data = NULL;
}

/**
 * Copy Constructor
 *
 * @param m The Matrix to copy
 */
Matrix::Matrix( const Matrix &m )
{
    this->rows = 0;
    this->cols = 0;
    this->data = NULL;

    this->setSize ( m.rows, m.cols );

    // Copy Data
    for ( unsigned i = 0; i < m.rows; i++ ){
	for ( unsigned j = 0; j < m.cols; j++ ){
	    this->data[i][j] = m.data[i][j];
	}
    }
}

/**
 * Create the matrix of the desired size. This
 * also allocates the memory
 *
 * @param rows
 * @param columns
 */
Matrix::Matrix(unsigned rows, unsigned columns)
{
    this->rows = 0;
    this->cols = 0;
    this->data = NULL;

    this->setSize( rows, columns );
}

/**
 * Destructor
 */
Matrix::~Matrix()
{
    if ( this->data ){
	free ( this->data );
    }
}

/**
 * Allocate the requested size for the matrix
 *
 * @param rows The amount of rows to allocate
 * @param columns The amount of columns to allocate
 */
void Matrix::setSize( unsigned rows, unsigned columns )
{
    if ( this->data ){
	free ( this->data );
    }

    this->rows = rows;
    this->cols = columns;

    // Allocate size for the pointers and the data they contain. We do this
    // in one call as it quicker however, we must remember to point our row pointers
    // to the correct location afterwards else we have dangling pointers
    this->data = (T**) calloc( 1, (rows * sizeof ( T * )) +( rows * cols * sizeof(T)));
    assert( this->data != NULL );

    // Our row data starts immediately after the pointers, to make the code clearer we
    // setup a variable to this address. Since the pointers are 0 -> rows-1 we can get 
    // the offset to the actual data by using rows;
    T *rowdata = (T *) &(this->data[rows]);

    // We then assign our pointers to point to the relevant locations
    for ( unsigned i = 0; i < this->rows; i ++ ){
	this->data[i]= &rowdata[i*columns];
    }
}

/**
 * Obtain a constant pounsigneder to the selected row
 *
 * @return The pounsigneder to the selected row
 */
const T *Matrix::operator[]( unsigned row) const
{
    return this->data[row];
}


/**
 * Obtain a pounsigneder to the selected row
 *
 * @return The pounsigneder to the selected row
 */
T *Matrix::operator[]( unsigned row)
{
    return this->data[row];
}

/**
 * Allow summation of two matrixes. It is assumed that the matrixes
 * are both of the same size (an assertion checks);
 * 
 * @param im The matrix to sum this matrix against
 */
Matrix Matrix::operator+ (const Matrix &im) const
{
    assert( this->rows == im.rows && "Rows are not the same size");
    assert( this->cols == im.cols && "Columns are not the same size");

    Matrix m( *this );

    for( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    m.data[i][j]+=im.data[i][j];
	}
    }

    return m;
}

/**
 * Allow subtration of two matrixes. It is assumed that the matrixes
 * are both of the same size ( an assertion checks );
 *
 * @param imThe matrix to subtract from the current
 */
Matrix  Matrix::operator- (const Matrix &im) const
{
    assert( this->rows == im.rows && "Rows are not the same size");
    assert( this->cols == im.cols && "Columns are not the same size");

    Matrix m ( *this );

    for( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    m.data[i][j]-=im.data[i][j];
	}
    }

    return m;
}


/**
 * Perform the negation of the matrix. 
 *
 * @param The matrix to negate
 */
Matrix  Matrix::operator- () const
{
    Matrix m ( *this );

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    m.data[i][j]=0 - m.data[i][j];
	}
    }

    return m;
}

/**
 * Multiply the entire matrix by the given value. 
 *
 * @param v The value to multiply the elements of the matrix by
 */
Matrix Matrix::operator* (const T &v) const
{
    Matrix m ( *this );

    for ( unsigned i =0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    m.data[i][j]*=v;
	}
    }

    return m;
}


/**
 * Multiply this matrix by the given matrix. It is assmed that the 
 * matrixes have the correct dimentions( this is checked via assertions)
 * 
 * @param im The matrix to multiply by
 */
Matrix  Matrix::operator* (const Matrix &im) const
{
    Matrix m(this->rows, im.cols);
    m.storeProduct( *this, im );

    return m;
}

/**
 * Divide the various elements of the matrix through by the given value
 *
 * @param v The value to divide by
 */
Matrix Matrix::operator/ (const T &v ) const
{
    Matrix m ( *this );

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    m.data[i][j]/= v;
	}
    }

    return m;
}

/**
 * Set the current matrix to equal the given matrix. This allows assignment
 * between differently dimentioned matrixes as well.
 *
 * @param im The input matrix to set this matrix as
 */
Matrix &Matrix::operator= (const Matrix &im)
{
    if ( *this == im ){
	return *this;
    }

    this->setSize ( im.rows, im.cols );

    for ( unsigned i = 0; i < im.rows; i++ ){
	for ( unsigned j = 0; j < im.cols; j++ ){
	    this->data[i][j] = im.data[i][j];
	}
    }

    return *this;
}

/**
 * Allow summation of two matrixes
 *
 * @param im the matrix to sum against
 */
Matrix &Matrix::operator+=(const Matrix &im)
{
    assert( this->rows == im.rows && "Rows are not the same size");
    assert( this->cols == im.cols && "Columns are not the same size");

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]+=im.data[i][j];
	}
    }

    return *this;
}

/**
 * Perform subtraction of the input matrix from this matrix
 *
 * @param im the matrix to subtract
 */
Matrix &Matrix::operator-=(const Matrix &im)
{
    assert( this->rows == im.rows && "Rows are not the same size");
    assert( this->cols == im.cols && "Columns are not the same size");

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]-=im.data[i][j];
	}
    }

    return *this;
}

/**
 * Multiply the matrix by the given value
 *
 * @param v value to mulitply by
 */
Matrix &Matrix::operator*=(const T &v)
{
    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]*= v;
	}
    }

    return *this;
}

/**
 * Divide the matrix by the given value
 *
 * @param v The value to divide each element of the matrix by
 */
Matrix &Matrix::operator/=(const T &v)
{
    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]/=v;
	}
    }

    return *this;
}

/**
 * Compare the content of the matrixes and return if they differ
 *
 * @param im The matrix to compare against
 * @return true if the input matrix is the same as this matrix, false otherwise
 */
bool Matrix::operator == (const Matrix &im) const
{
    if ( this->rows == im.rows && this->cols == im.cols ){
	for ( unsigned i = 0; i < this->rows; i++ ){
	    for ( unsigned j = 0; j < this->cols; j++ ){
		if ( this->data[i][j] != im.data[i][j] ){
		    return false;
		}
	    }
	}

	return true;
    }
    else {
	return false;
    }
}

/**
 * Compare the content of the matrixes and return if they differ
 * 
 * @param im The matrix to compare against
 * @return true if the input matrix is different to the current matrix
 */
bool Matrix::operator !=(const Matrix &im)
{
    return ! ( (*this ) == im );
}

/**
 * Obtain the amount of rows in the matrix
 *
 * @return the amount of rows in the matrix
 */
unsigned Matrix::getRows() const
{
    return this->rows;
}

/**
 * Obtain the amount of columns in the matrix
 *
 * @return the amount of columns in the matrix
 */
unsigned Matrix::getCols() const
{
    return this->cols;
}

/**
 * Store the transpose of the matrix 
 *
 * @param im The matrix which to transpose
 */
void Matrix::storeTranspose( const Matrix &im )
{
    assert( this->rows == im.cols && this->cols == im.rows && "Transpose Size error");
    assert( this != &im && "Cannot transpose myself" );

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j=0; j < this->cols; j++ ){
	    this->data[i][j]=im.data[j][i];
	}
    }
}

/**
 * Store the product of the two matrixes in this matrix
 *
 * @param m1 The matrix on the left to multiply by
 * @param m2 The matrix on the right to multiply by
 */
void Matrix::storeProduct(const Matrix &m1, const Matrix &m2)
{
    assert( m1.cols == m2.rows && "Invalid Multiplication Attempted");
    assert( this->rows == m1.rows && "Matrix not the correct size for storing Product");
    assert( this != &m1 && this != &m2 && "Cannot multiply by myself" );

    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]=0;
	    for ( unsigned k = 0; k < m1.cols; k++ ){
		this->data[i][j]+=m1.data[i][k]*m2.data[k][j];
	    }
	}
    }
}

/** 
 * Print out the matrix
 *
 * @param m The matrix to print
 */
void Matrix::print() const
{
    printf("Matrix(%d,%d)={\n", this->rows, this->cols );
    for ( unsigned rows = 0; rows < this->rows; rows++ ){
	const T *row = this->data[rows];
	printf("\t{");
	for ( unsigned cols = 0; cols < this->cols; cols++ ){
	    printf("%5.5g%s ", row[cols], cols==this->cols-1?"":",");
	}
	printf("}%s\n", rows==this->rows-1?"":",");
    }
    printf("};\n");
}

/**
 * Zero out the matrix. All elements are zeroed
 */
void Matrix::storeZeros()
{
    for ( unsigned i = 0; i < this->rows; i++ ){
	for ( unsigned j = 0; j < this->cols; j++ ){
	    this->data[i][j]=0.0;
	}
    }
}

///
// Global Operators, these aid us in using the matrix
//

/**
 * Perform the multiplication of the given value to the matrix
 * with the multiplication being done im * v;
 *
 * @param v value to multiply by
 * @param im The matrix to multiply
 */
Matrix operator *( const T &v, const Matrix &im )
{
    return im * v;
}

/**
 * Transpose the elements of the matrix
 *
 * @param im Matrix to transpose
 */
Matrix transpose ( const Matrix &im )
{
    Matrix m( im.getCols(), im.getRows());

    m.storeTranspose( im );

    return m;
}

}; //namespace wcl
