/*
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
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

#include <wcl/maths/Matrix.h>

/**
 * Performs a singlular value decomposition on a matrix.
 *
 * Based on the following tutorial:
 *
 * http://www.miislita.com/information-retrieval-tutorial/svd-lsi-tutorial-1-understanding.html
 *
 * @param[in] a The matrix to decompose.
 * @param[out] d The diagonal matrix part of the decomposition
 * @param[out] u wtf mate?! \todo What the hell am I doing here!
 */

void SVD(const Matrix& a, Matrix& d, Matrix& u)
{
	int rows = a.getRows();
	int cols = a.getCols();

	if (rows<cols)
	{
		throw std::string("Sorry buddy, rows >= cols for SVD!");
	}
	d = a;
	double g = 0;
	double f,h;
	double x = 0
	// rowvector e(n)
	// rectMatrixRow ei(e,0)
	// d.resize(n);
	// rectmatrixcol uci(u,0)
	// rectmatrixrow uri(u,0,1,cols-1)
	
	
	for (int i=0;i<cols;i++)
	{
		

	}

}

