#ifndef __CFOX__MISC__SINGLETON_DOT_H__INCLUDED__
#define __CFOX__MISC__SINGLETON_DOT_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
/*!
    @header     singleton
    @abstract   Template Singletonizer class
    @discussion Defines the Singleton pattern template class
  
  @author Original Code Sample from Erik Staats, NWG.
          FWCamAkiz was written by Adrian Daerr
          C+FOX was written by Joel Falcou

  Copyright (c) 2002 Adrian Daer & Université Paris VII "Denis Diderot".									 
  Copyright (c) 2004 Joel Falcou & Université Clermont-Ferrand II "Blaise Pascal".									 
  All rights reserved.																						 
                                                                   
  This file is part of the C+FOX Library.  This
  library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1,
  or (at your option) any later version.
                                               
  This library is distributed in the hope that it will be useful,	 
  but WITHOUT ANY WARRANTY; without even the implied warranty of	 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 
  GNU Lesser General Public License for more details.							 
                                                                   
  You should have received a copy of the GNU Lesser General				 
  Public License along with this library; see the file COPYING.		 
  If not, send mail to the developers of C+FOX										 
                                                                   
  As a special exception, you may use this file as part of a free	 
  software library without restriction. Specifically, if other		 
  files instantiate templates or use macros or inline functions		 
  from this file, or you compile this file and link it with other	 
  files to produce an executable, this file does not by itself		 
  cause the resulting executable to be covered by the GNU Lesser	 
  General Public License.  This exception does not however				 
  invalidate any other reasons why the executable file might be		 
  covered by the GNU Lesser General Public License.
*/
////////////////////////////////////////////////////////////////////////////////

#include <memory>

  ////////////////////////////////////////////////////////////////////////////////
  /*!
      @class      Singleton
      @abstract   "Singletonizer" class.
      @discussion Singleton is a template class that is used by inheritance
      to turn any other class into a singleton. Singletons are C++ classes
      that can only have ONE instance during the application lifetime. This is
      usually done with restricting construction and copy and providing the
      client a  static method to access to the only one Singleton instance.
      
      To turn a class Foo into a singleton , one has to write the following :
      
      class Foo : public Singleton<Foo>
      {
          friend class Singleton<Foo>;
          
          // Your code here
      
          protected :
          Foo();
      };
      
      Then to get an instance of Foo :
      
      Foo instance = Foo::Instance();    
  */
////////////////////////////////////////////////////////////////////////////////
template< typename T > class Singleton
{
	public:

		static T* Instance() 
		{
			if (!me.get())
			{
				me.reset(new T);
			}

			return me.get();
		}

	protected:

		static std::auto_ptr<T> me;

		Singleton() {}
		Singleton(const Singleton &) {}
		Singleton & operator=(const Singleton &) {}
		virtual ~Singleton() {}
};

template<class T> std::auto_ptr<T> Singleton<T>::me = std::auto_ptr<T>(0);
#endif
