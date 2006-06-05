#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

#define failedToAllocateMemory() __failed_to_allocate_memory( __FILE__, __LINE__ )
void __failed_to_allocate_memory( char *file, int line );

void gen_fatal_handler_hog (const char *source_file, int line_num, const char *function, char *format, ...) __attribute__ ((format (printf, 4, 5)));
#define gen_fatal(format, args...) gen_fatal_handler_hog (__FILE__, __LINE__, __FUNCTION__, format, ## args), exit (-1)

void message_handler_hog (const char *source_file, int line_num, const char *function, char *format, ...) __attribute__ ((format (printf, 4, 5)));
#define message(format, args...) message_handler_hog (__FILE__, __LINE__, __FUNCTION__, format, ## args)

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
