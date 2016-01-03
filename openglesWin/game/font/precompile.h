#ifndef _PRECOMPILE_H_INCLUDED_
#define _PRECOMPILE_H_INCLUDED_

#ifdef WIN32
// enable memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#ifdef WIN32
#include <GLES/gl.h>
//#include <GLES/PVRTglesExt.h>
#else
#include <GLES/gl.h>
#endif
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h> 
#include <assert.h>
#include <map>
#include <vector>
#include <boost/unordered_map.hpp>  
#define fmap boost::unordered_map

#ifdef WIN32
#pragma warning (error: 4715)
#pragma warning (error: 4706)
#pragma warning (error: 4553)	// '==' : operator has no effect; did you intend '='?
#pragma warning (error: 4150)	// deletion of pointer to incomplete type
#pragma warning (error: 4390)	// empty controlled statement found;
#pragma warning (error: 4804)	// unsafe use of type 'bool'
#pragma warning (error: 4552)	// missing return
#pragma warning (disable: 4996)
#pragma warning( disable : 4800 ) 
#pragma warning( disable : 4100 ) 
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4127) // conditional expression is constant
#pragma warning (disable : 4355) // 'this' : used in base member initializer list
#endif

//#define APPSTORE_BUILD  1

#ifdef APPSTORE_BUILD
#define cout ostream(0).flush()
#endif



#endif // _PRECOMPILE_H_INCLUDED_

