#include "global.h"
#include "serial.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>




int SERIALopen (char *device, int block)
{
  int fd;
  
  /* Open the device with non-blocking and no controlling TTY attributes */
  if (block == SERIAL_NONBLOCK)
    fd = open (device,O_RDONLY|O_NONBLOCK);//fd = open (device, O_NONBLOCK | O_NOCTTY | O_RDWR | O_NDELAY);
  //else if (block == SERIAL_BLOCK)
    //fd = open (device, O_NOCTTY | O_RDWR | O_NDELAY);
  else
    gen_fatal ("Block flag %d is not known - argument passed was incorrect", block);
  
  /* Check for errors and return */
  if (fd < 0)
    gen_fatal ("Could not open the serial device %s with control flags - %s", device, error_string());
  egen (io, "Opened the serial port %s on fd %d", device, fd);
  return (fd);
}




void SERIALclose (int fd)
{
  if (close (fd) < 0)
    gen_fatal ("Could not close serial port fd %d - %s", fd, error_string ());
}




void SERIALparams (int fd, int baud_rate, int bits, int mode)
{
  /* Work out the constant to use for the baud rate from the input value */
  int baud = -1; /* Keep the compiler happy, it can't seem to detect that this
		    value is initialised correctly */
  switch (baud_rate)
    {
    case 300:    baud = B300;  break;
    case 1200:   baud = B1200; break;
    case 2400:   baud = B2400; break;
    case 4800:   baud = B4800; break;
    case 9600:   baud = B9600; break;
    case 19200:  baud = B19200; break;
    case 38400:  baud = B38400; break;
    case 57600:  baud = B57600; break;
    case 115200: baud = B115200; break;
    default:
      gen_fatal ("The baud rate value %d given is not a supported serial port rate", baud_rate);
      break;
    }
  
fprintf(stderr, "error after buad rate set here!");

  /* Decide on a bit flag */
  int bitflag;
  if (bits == 7)
    bitflag = CS7;
  else if (bits == 8)
    bitflag = CS8;
  else
    gen_fatal ("The number of bits %d must be 7 or 8", bits);
  
  /* We read in the existing terminal attributes otherwise we could clobber over
     flags which are required in some OSes (got burned in Cygwin before) */
  struct termios new_serial;
  if (tcgetattr (fd, &new_serial) < 0)
    gen_fatal ("tcgetattr() call on fd %d failed - %s", fd, error_string());
  
  /* Set the baud rate in a portable fashion */
  cfsetispeed (&new_serial, baud);
  cfsetospeed (&new_serial, baud);
  
  if (mode == SERIAL_LINE)
    {
      /* This is for a line-based cooked mode */
      new_serial.c_iflag |= ICRNL;
      new_serial.c_lflag |= ICANON;
    }
  else if (mode == SERIAL_BYTE)
    {
      /* This is for raw byte mode */
      new_serial.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    }
  else
    gen_fatal ("Mode flag %d is not known - argument passed was incorrect", mode);
  
  /* Set either 7N1 or 8N1 mode depending on bitflag set above */
  new_serial.c_cflag &= ~PARENB;
  new_serial.c_cflag &= ~CSTOPB;
  new_serial.c_cflag &= ~CSIZE;
  new_serial.c_cflag |= bitflag;
  
  /* Disable hardware flow control */
  new_serial.c_cflag &= ~CRTSCTS;
  
  /* Disable software flow control */
  new_serial.c_iflag &= ~(IXON | IXOFF | IXANY);
  
  /* Set the serial port to use the specified settings */
  if (tcsetattr (fd, TCSANOW, &new_serial) < 0)
    gen_fatal ("tcsetattr() call on fd %d failed - %s", fd, error_string());
    
  /* None of our devices use flow control, but some devices like the Basic Stamp don't like to
     have DTR set because this indicates it needs to be reprogrammed. So we need to turn off DTR */
  int status;
  if (ioctl (fd, TIOCMGET, &status) < 0)
    gen_fatal ("ioctl(TIOCMGET) call on fd %d failed - %s", fd, error_string());
  status &= ~TIOCM_DTR;
  if (ioctl (fd, TIOCMSET, &status) < 0)
    gen_fatal ("ioctl(TIOCMSET) call on fd %d failed - %s", fd, error_string());
  
  /* Debug */
  egen (io, "Serial port fd %d reset to baud %d, line %d", fd, baud_rate, (mode == SERIAL_LINE));
}


/*this is what the qtopia app uses to communicate with it's devices
	attr.c_cflag &= ~CBAUD;
	attr.c_cflag |= B19200 | CS8 | CLOCAL;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	attr.c_cflag &= ~CRTSCTS;
*/
