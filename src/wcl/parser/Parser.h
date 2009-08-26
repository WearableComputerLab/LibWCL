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

namespace wcl
{
    /**
     * A parser exception is used to indicate an error that occurred during
     * parsing. 
     */
    class ParserException
    {
    public:
        enum ParserExceptionCause { INVALID_INPUT, UNKNOWN_ERROR };

        ParserExceptionCause cause;
        std::string reason;

        int getCause() const { return cause; };
        const std::string getReason() const { return reason; };
    };

    /**
     * An abstract base class for all parsers.
     * All parsers must implement the 'parse' method that
     * does the actual gurnt work. Upon an error a ParserException is thrown.
     */
    class Parser
    {
    public:
        virtual ~Parser();
        virtual void parse() = 0;

    protected:
        Parser();

        /**
         * Called by parsers when an error condition occurs
         * This inturn generates an exception which does not need to be freed
         *
         * @throws ParseException
         */
        void parseError(const ParserException::ParserExceptionCause cause,
                        const std::string &reason)
        {
            static ParserException e;
            e.cause = cause;
            e.reason = reason;

            throw e;
        }
    };

};

#endif
