#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

#include "debug.h"

void failed_to_allocate_memory_handler( char *file, int line )
{
	fprintf( stderr, "%s:%d - Failed to allocated memory\n", file, line );
	abort();
//	exit( 1 );
}

/* This is a special version of the vsprintc_handler function, except this version is
   designed to be used from within gen_fatal, there is the possibility that this function
   could call gen_fatal itself recursively */
char *vsprintc_fatal( char* format, va_list args )
{
#define FATAL_LENGTH 4096
  static char temp_str [FATAL_LENGTH];
  
  /* Check size of string printed out for safety */
  if (vsnprintf (temp_str, FATAL_LENGTH, format, args) >= FATAL_LENGTH)
    {
      fprintf (stderr, "Double fatal error - vsnprintf attempted to overflow the buffer of %d bytes\n", FATAL_LENGTH);
      fprintf (stderr, "Format string is [%s]\n", format);
      exit (1);
    }
  
  /* Return it back now */
  return (temp_str);
}

/* This is the function to call whenever a fatal mistake is made. There is another event
   mechanism, but we keep this function simple to ensure that it is always available for
   execution. This code is simple C with no object oriented code in it */
void gen_fatal_handler( const char *source_file, int line_num, const char *function, char *format, ...)
{
  /* Setup the args structure to retrieve the parameters after format, render it, and clean up */
  va_list args;
  va_start (args, format);
  char *message = vsprintc_fatal (format, args);
  va_end (args);
  
  
  /* Flush the console before doing anything in case something is messed up */
  fflush (stdout);
  fflush (stderr);
  
  
  /* Print out info about where this gen_fatal was called from */
  fprintf (stderr, "\nFatal error generated in code %s:%d in function %s()\n",
           source_file, line_num, function);
  
  
  /* Now print the rest of the message supplied to this function */
  fprintf (stderr, "\n%s\n\n", message);
  
  
  /* Flush the console before doing anything in case something is messed up */
  fflush (stdout);
  fflush (stderr);
  
  
  /* Now terminate execution */
  _exit (1);
}


void message_handler( const char *source_file, int line_num, const char *function, char *format, ...)
{
  /* Setup the args structure to retrieve the parameters after format, render it, and clean up */
  va_list args;
  va_start (args, format);
  char *message = vsprintc_fatal (format, args);
  va_end (args);
  
  /* Flush the console before doing anything in case something is messed up */
  fflush (stdout);
  fflush (stderr);
  
  /* Print out info about where this gen_fatal was called from */
  fprintf ( stderr, "%s:%d in function %s(): %s\n", source_file, line_num, function, message );
  
  /* Flush the console before doing anything in case something is messed up */
  fflush (stdout);
  fflush (stderr);
}

void errno_message_handler( const char *source_file, int line_num, const char *function )
{
	// Flush the console before doing anything in case something is messed up
	fflush( stdout );
	fflush( stderr );

	// Print out info about where this message was called from
	fprintf ( stderr, "%s:%d in function %s(): %s\n", source_file, line_num, function, strerror( errno ) );
  
	// Flush the console before doing anything in case something is messed up 
	fflush (stdout);
	fflush (stderr);
}
