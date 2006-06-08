#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void failed_to_allocate_memory_handler( char *file, int line );
#define failed_to_allocate_memory() failed_to_allocate_memory( __FILE__, __LINE__ )

void gen_fatal_handler (const char *source_file, int line_num, const char *function, char *format, ...) __attribute__ ((format (printf, 4, 5)));
#define gen_fatal(format, args...) gen_fatal_handler (__FILE__, __LINE__, __FUNCTION__, format, ## args), exit (-1)

void message_handler (const char *source_file, int line_num, const char *function, char *format, ...) __attribute__ ((format (printf, 4, 5)));
#define message(format, args...) message_handler (__FILE__, __LINE__, __FUNCTION__, format, ## args)

void errno_message_handler( const char* source_file, int line_num, const char *function );
#define errno_message() errno_message_handler( __FILE__, __LINE__, __FUNCTION__ );

#define STANDARD_MALLOC
#ifdef STANDARD_MALLOC

#define cmalloc malloc
#define cfree free

#else

#warning Using Tinmith malloc() interface functions

#define cmalloc(size) cmalloc_handler(size, __FILE__, __LINE__)
void *cmalloc_handler (int size, char *source_file, int line_num);

#define cfree(ptr)      cfree_handler(ptr, __FILE__, __LINE__)
void cfree_handler (void *ptr, char *source_file, int line_num);

#endif /* STANDARD_MALLOC */


#endif // __DEBUG_H_
