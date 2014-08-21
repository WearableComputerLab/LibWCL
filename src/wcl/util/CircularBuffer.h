/*
 * Copyright (c) 2014 Michael Marner <michael@20papercups.net>
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


#ifndef LIBWCL_CIRCULAR_BUFFER_H
#define LIBWCL_CIRCULAR_BUFFER_H

namespace wcl {


    /**
     * A Circular Buffer that can be used to do things like smoothing out data
     * from a tracking system. Templated class, so you can use this buffer to
     * store anything that supports the following:
     *
     * * A default constructor that initialises things with sane values.
     * * operator+
     * * operator>
     * * operator<
     */
    template<typename T> class WCL_API CircularBuffer {

        public:
            /**
             * Constructs a new CircularBuffer of a certain maximum size.
             *
             * @param size The maximum number of elements the buffer will
             * store.
             */
            CircularBuffer(unsigned size, const T& zero) : maxSize(size), currentSize(0), nextIndex(0), zero(zero) {
                data = new T[size];
            }


            /**
             * Desctructor.
             */
            virtual ~CircularBuffer() {
                delete[] data;
            }

            /**
             * Calculates the mean value of the elements stored in the buffer.
             *
             * @return The mean value in the buffer.
             */
            T getMean() const {
                if (currentSize == 0) {
                    return zero;
                }
                T accumulated = zero;
                for (unsigned i=0; i < currentSize; ++i) {
                    accumulated = accumulated + data[i];
                }
                return accumulated / currentSize;
            }


            /**
             * Returns the maximum value stored in the buffer.
             */
            T getMax() const {
                if (currentSize == 0) {
                    return zero;
                }
                T max = data[0];;
                for (unsigned i=1; i < currentSize; ++i) {
                    if (data[i] > max) {
                        max = data[i];
                    }
                }
                return max; 
            };


            /**
             * Returns the minimum value stored in the buffer.
             */
            T getMin() const {
                if (currentSize == 0) {
                    return zero;
                }
                T min = data[0];;
                for (unsigned i=1; i < currentSize; ++i) {
                    if (data[i] < min) {
                        min = data[i];
                    }
                }
                return min; 
            }


            /**
             * Puts a new element into the buffer. If the buffer has reached
             * its maximum size, the oldest element will be rewritten with this
             * new element.
             *
             * @param element The element to add to the buffer.
             */
            void push(const T& element) {
                data[nextIndex] = element;
                nextIndex++;
                if (nextIndex == maxSize) {
                    nextIndex = 0;
                }
                if (currentSize < maxSize) {
                    currentSize++;
                }
            }

            /**
             * Empties the buffer.
             */
            void clear() {
                currentSize = 0;
                nextIndex = 0;
            }

            /**
             * Returns the number of elements currently stored in the buffer.
             */
            unsigned getSize() const {
                return currentSize;
            }

        private:
            /**
             * The maximum size of the buffer
             */
            unsigned maxSize;
            
            /**
             * The number of elements currently stored in the buffer.
             */
            unsigned currentSize;

            /**
             * The next index in the array to use when pushing an element.
             */
            unsigned nextIndex;

            
            /**
             * What is zero in a templated datatype?
             */
            T zero;

            /**
             * Internal storage array
             */
            T* data;

    };
};

#endif
