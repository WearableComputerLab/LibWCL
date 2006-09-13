/*
 *
 * TODO:
 *   * Ability to turn on/off messages on a class/file/method basis
 *   * config file for the above
 *   * Stack trace support
 *   * ability to email on error
 *   * callback support for messages/debugging
 *   * autocatch evil signals
 *   * ability to define the error output format
 *   * colour support
 *   * unit test support
 *   * memory checking 
 *   * namespace for libwcl
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Defines
 */
#define DBG_INFO   0x1
#define DBG_WARN   0x2
#define DBG_ERROR  0x4
#define DBG_DETAIL 0x8
#define DBG_ALL    0xf

// Enable Debugging if NDEBUG is not defined
// NDEBUG also disables assertions used with assert.h
#ifndef NDEBUG
#define DEBUG 1
#endif

/**
 * Display a debug message in the specified level. This can
 * be conditionally compiled out by defining NDEBUG
 * 
 * @param The debugging level this message falls into
 * @param fmt The fprintf format for the debugging message
 * @param args The various arguments to fprintf
 */ 
#define debug(level,fmt, args...) \
        { \
                if(NDEBUG && (level & DBG_LEVEL))\
                         fprintf( stderr, __FILE__ "(%d):%#x: " fmt "\n", __LINE__ , type ,##args ); \
        }

/**
 * Display a message in. This
 * message can be enabled or disabled at runtime.. well in the future anyway
 * 
 * @param fmt The fprintf format for the debugging message
 * @param args The various arguments to fprintf
 */ 
#define message( fmt, args... )\
        { \
	  fprintf( stderr, __FILE__ "(%d):%#x: "  fmt "\n", __LINE__ , type ,##args ); \
        }


/**
 * Display a message and then exit the application with
 * an exit level of -1
 *
 * @param fmt The fprintf format for the debugging message
 * @param args The various arguments to fprintf
 */
#define fatal( fmt, args... )\
        { \
	  fprintf( stderr, __FILE__ "(%d):%#x: " fmt "\n", __LINE__ , type ,##args ); \
	  exit(-1); \
        }

#endif
