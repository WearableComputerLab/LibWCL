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

#include <assert.h>
#include "SMatrix.h"


namespace wcl
{

/**
 * Default Constructor
 */
SMatrix::SMatrix()
{}

/**
 * Create a Square Matrix of the given size
 *
 * @param rows The amount of rows in the matrix
 */
SMatrix::SMatrix( unsigned rows):
    Matrix( rows, rows )
{}

/**
 * Copy a matrix class, the matrix must be square ( checked
 * by assertions
 *
 * @param im The input matrix to adapt to a SMatrix
 */
SMatrix::SMatrix( const Matrix &im ):
    Matrix( im )
{
    assert ( im.getCols() == im.getRows() && "Input matrix is not square");
}

/**
 * Copy constructor
 *
 * @param im Input matrix to copy
 */
SMatrix::SMatrix( const SMatrix & im):
    Matrix( im )
{}

/**
 * Destructor
 */
SMatrix::~SMatrix()
{}

/**
 * Set the size of the matrix
 */
void SMatrix::setSize( unsigned size )
{
    Matrix::setSize( size, size );
}

/**
 * Matrix Addition, Sum the two square matrixes together.
 * They should both be the same size (checked by assertions)
 *
 * @param im The matrix to use for addition
 */
SMatrix SMatrix::operator +( const SMatrix &im ) const
{
    SMatrix s ( *this );

    s+= im;

    return s;
}

/**
 * Matrix subtraction, Subtract one matrix from the other matrix
 * They should both be the same size (checked by assertions
 *
 * @param im The matrix to use for subtraction
 */
SMatrix SMatrix::operator -( const SMatrix &im ) const
{
    SMatrix s(*this);

    s -= im;

    return s;
}

/** 
 * Negate the matrix
 */
SMatrix SMatrix::operator -() const
{
    SMatrix s( *this );

    s *= -1.0;

    return s;
}

/**
 * Multiply the matrix with the given value
 *
 * @param v The value to multiply the matrix with
 */
SMatrix SMatrix::operator *( const T &v ) const
{
    SMatrix m ( *this );
    return  SMatrix(((Matrix)m).operator *(v));
}

/**
 * Multiply one matrix by the other - the matrixes are the same size
 * Multiplication is done: this * im
 *
 * @param im The matrix to multiply by
 */
SMatrix SMatrix::operator *( const SMatrix &im ) const
{
    SMatrix m ( im.getRows());
    m.storeProduct( *this, im );

    return m;
}

/**
 * Divide the matrix with the given value
 *
 * @param v The value to divide the matrix from
 */
SMatrix SMatrix::operator /( const T &v ) const
{
    SMatrix s( *this );

    s *= 1.0 / v;

    return s;
}

/**
 * Divide the matrix by the given matrix. The matrixes must be the same size
 * this is checked via assertions. We perform the division by multiplication
 * of the inverse so this is a costly operation.
 *
 * @param im The matrix to divide by
 */
SMatrix SMatrix::operator /( const SMatrix &im ) const
{
    assert( this->getCols() == im.getCols() && "Amount of Columns Differ");
    assert( this->getRows() == im.getRows() && "Amount of Rows Differ" );

    return (*this) * inv ( im );
}

/**
 * Assignment Operator
 *
 * @param im The matrix to assign from
 */
SMatrix & SMatrix::operator = ( const SMatrix &im )
{
    return (SMatrix &) Matrix::operator = ( im );	
}

/**
 * Addition Assignment
 *
 * @param im The matrix to assign from
 */
SMatrix & SMatrix::operator +=( const SMatrix &im )
{
    return (SMatrix &)Matrix::operator += ( im );
}

/**
 * Subtraction Assignment
 *
 * @param im The matrix to substract from
 */
SMatrix & SMatrix::operator -=( const SMatrix &im )
{
    return (SMatrix &)Matrix::operator -= (im );
}

/**
 * Multiplication Assignment
 *
 * @param v The value to Multiply by
 */
SMatrix & SMatrix::operator *=( const T &v )
{
    return (SMatrix &)Matrix::operator *= ( v );
}

/**
 * Multiplication Assignment 
 *
 * @param im The input Matrix ( must be same size )
 */
SMatrix & SMatrix::operator *=( const SMatrix &im )
{
    return (*this) = (*this) * im;
}

/** 
 * Division Assingment
 *
 * @param v The value to divide by
 */
SMatrix & SMatrix::operator /=( const T &v )
{
    return (*this) *= 1.0 / v;
}

/** 
 * Divide one matrix by another. Each element of the current matrix
 * are divided by the corresponding element of the given matrix.
 * The input matrix must be the same size as the given matrix this is checked
 * by an assertion.
 *
 * @param im The matrix to divide by
 */
SMatrix & SMatrix::operator /=( const SMatrix &im )
{
    assert( this->getCols() == im.getCols() && "Amount of Columns Differ");
    assert( this->getRows() == im.getRows() && "Amount of Rows Differ" );

    return *this = (*this) / im;
}

/**
 * Store the inverse of the matrix.
 *
 * @param im The matrix to inverse
 */
SMatrix& SMatrix::storeInverse( const SMatrix &im )
{
    SMatrix m ( im );
    T tmp;
    unsigned i,j,k, rows;

    assert ( det ( im ) != 0.0 && "Matrix does not have an inverse" );

    rows = im.getRows();

    // If this matrix is a different size, resize accordingly
    if ( this->getRows() != rows){
	this->setSize ( rows);
    }

    // Make the matrix an identity matrix
    this->storeIdentity();

    // Perform the required row reduction
    for ( i = 0; i < rows; i++ ){
	if ( m[i][i] == 0.0 ){
	    for ( j = 0; j < rows; j++ ){
		if ( m[j][i] != 0.0 ){
		    // Copy the row
		    for ( k = 0; k < rows; k++ ){
			m[i][k] += m[j][k];
			(*this)[i][k] += (*this)[j][k];
		    }
		    break;
		}
	    };
	};

	tmp = m[i][i];

	// Multiply the rows
	for ( k = 0; k < rows; k++ ){
	    m[i][k] *= 1.0 / tmp;
	    (*this)[i][k] *= 1.0 / tmp;
	}

	for ( j = 0; j < rows; j++ ){
	    if ( j != i ){
		tmp = -m[j][i];
		for ( k = 0; k < rows; k++ ){
		    m[j][k] += m[i][k] * tmp;
		    (*this)[j][k] += (*this)[i][k] * tmp;
		}
	    }
	}
    }
	return *this;
}


/**
 * Store the identity/unity matrix within the current matrix. 
 */
SMatrix& SMatrix::storeIdentity()
{
    for ( unsigned i = 0; i < this->getRows(); i++ ){
	for ( unsigned j = 0; j < this->getRows(); j++ ){
	    if ( i == j ){
		(*this)[i][j]=1.0;
	    }else {
		(*this)[i][j]=0.0;
	    }
	}
    }
	return *this;
}


//
// Global Operators
//

/**
 * Create a matrix based on the multiplication of the matrix and a value
 *
 * @param v The value to multiply the matrix by
 * @param im The matrix to multiply
 */
SMatrix operator * ( const T &v, const SMatrix &im )
{
    return (SMatrix( im ) * v );
}

//
// Helper Methods
//

/** 
 * Transpose the Given matrix into the new Matrix
 *
 * @param im The matrix to transpose
 */
SMatrix transpose ( const SMatrix &im )
{
    SMatrix m( im.getRows());

    m.storeTranspose( im );

    return m;
}


/**
 * Inverse the given matrix
 *
 * @param im The matrix to invert
 */
SMatrix inv ( const SMatrix &im )
{
    SMatrix m ( im.getRows() );

    m.storeInverse ( im );

    return m;
}

/**
 * Calculate the determinate of matrix. Theres a number of
 * ways to do this. Using coofactors of a matrix uses recursion so
 * we avoid this. Instead we use Gauss-Jordan Elimination and use
 * an iterative approach.
 *
 * This elimination comes into play due to the fact
 * that if the elements in lower triangle of the
 * matrix are all zeroes, the determinant is simply
 * the product of the diagonals. This also applies
 * in the upper triangle zeroes. It means the determinate
 * ends up being the product of the main diagonal after elmination
 *
 *
 * @param im The matrix to calculate the determinate of
 */
T det ( const SMatrix &im )
{
    unsigned i, j, k;
    T temp, factor, detvalue = 1.0;

    // Determinate of a 1x1 matrix
    if ( im.getRows() == 1 ){
	return im[0][0];
    }

    // Determinate of a 2x2 matrix
    else if ( im.getRows() == 2 ){
	return im[0][0]*im[1][1] - im[1][0]*im[0][1];
    }

    // Determinate of a 3x3 matrix
    else if ( im.getRows() == 3 ){
	detvalue = -im[0][2]*im[1][1]*im[2][0];//-a02 a11 a20
	detvalue += im[0][1]*im[1][2]*im[2][0];//+ a01 a12 a20
	detvalue += im[0][2]*im[1][0]*im[2][1];//+ a02 a10 a21
	detvalue -= im[0][0]*im[1][2]*im[2][1];//- a00 a12 a21
	detvalue -= im[0][1]*im[1][0]*im[2][2];//- a01 a10 a22
	detvalue += im[0][0]*im[1][1]*im[2][2];//+ a00 a11 a22
	return detvalue;
    }

    // The determinate for any other size matrix
    SMatrix m ( im );

    for ( i = 0; i  < m.getRows(); i++ ){

	// If the main diagonal is zero, resort the matrix
	if ( m[i][i] == 0.0 ){
	    for (  j = i+1; j < m.getRows(); j++ ){
		if ( m[j][i] != 0.0 ){
		    // Add one row to other
		    for ( k = 0; k < m.getCols(); k++ ){
			temp  = m[i][k];
			m[i][k] = m[j][k];
			m[j][k] = temp;
		    }
		    //  For Gauss-Jordan Elimination if we do a switch,
		    //the determinant switches sign.
		    detvalue = -detvalue;
		    break;
		}
	    }
	}
	// If after resort the diagonal is still zero then the determinate
	// is definately zero
	// Pointless contining if det is already zero
	if ( m[i][i] == 0.0 ){
	    return 0.0;
	}
	// We now elimiate the lower rows to obtain a triangle of zeros
	for ( j = i+1; j < m.getRows(); j++ ){
	    if ( j != i ){
		factor = ( m[j][i] * 1.0 )	/ m[i][i];

		// Add one row to the other
		for ( k = i; k < m.getCols(); k++ ){
		    m[j][k]-=factor * m[i][k];
		}
	    }
	}
    }

    // Calculate the main diagonal
    for ( i = 0; i < m.getRows(); i++ ){
	detvalue *= m[i][i];
    }

    return detvalue;
}

}; //end namespace

