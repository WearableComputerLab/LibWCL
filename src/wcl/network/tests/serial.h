#ifndef _IO_SERIAL_H
#define _IO_SERIAL_H


extern int  SERIALopen (char *device, int block);
extern void SERIALclose (int fd);
extern void SERIALparams (int fd, int baud, int bits, int mode);


#define SERIAL_BLOCK    1 /* Set for blocking open */
#define SERIAL_NONBLOCK 2 /* Set for non-blocking open */

#define SERIAL_LINE 3 /* Set for line based processing */
#define SERIAL_BYTE 4 /* Set for byte based processing */


#endif /* _IO_SERIAL_H */
