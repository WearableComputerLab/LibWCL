#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define error_string() strerror(errno)
#define gen_fatal(format, args...) fprintf (stderr, format "\n" , ## args), exit (1)
#define egen(dest, format, args...) fprintf (stderr, format "\n" , ## args)
