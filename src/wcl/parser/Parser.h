/*-
 * Copyright (c) 2009 Benjamin Close <Benjamin.Close@clearchain.com>
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

#ifndef WCL_PARSER_H
#define WCL_PARSER_H

#include <string>
#include <stdio.h>
#include <wcl/api.h>
#include <wcl/parser/ParserException.h>

namespace wcl
{
    /**
     * An abstract base class for all parsers.
     * All parsers must implement the 'parse' method that
     * does the actual gurnt work. Upon an error a ParserException is thrown.
     */
    class WCL_API Parser
    {
    public:
        virtual ~Parser();

        /**
         * This pure virtual method must be implemented by all parsers. It's
         * used to fire off the actual parse process of a parser
         */
        virtual void parse() throw (ParserException) = 0;

        /**
         * Provide a means of turning on debug output. By default debug output
         * should be off
         */
        virtual void setDebug(bool state=false);

        /**
         * The print method of a parser is provided as an abstract method that
         * classes extending this base class can implement. The aim is to print
         * out details about the results of parsing, generally based upon the
         * data structure the parser knows about. It's not pure virtual as it's
         * implementation is optional
         */
        virtual void print();

    protected:
        Parser();

        bool debug;

        /**
         * Called by parsers when an error condition occurs
         * This inturn generates an exception which does not need to be freed
         *
         * @throws ParseException
         */
        void parseError(const char *cause, const std::string extra="") throw (ParserException)
        {
            throw ParserException((std::string(cause) + ":" + extra).c_str());
        }
    };

};

#endif
