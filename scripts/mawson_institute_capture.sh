#!/bin/bash

# make sure that the camera isn't reseting.
LOCKFILE=snappy_reset.lock

if [ -f $LOCKFILE ] ; then
  echo "camera is resetting. therefore I am not going to do anything"

  #If there are still problems we might want to implement this...

  #Increment error count (store a count in /tmp/snappyError.log
   
  #If error count is >  threshold (perhpas 5 tries)
  #reboot snappy to try and sort things out 
  # shutdown -r now
  
  #If not up to threshold, just exit for now and see if it sorts it self out
  exit

fi

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
echo "PATH = $PATH"

HOME=/home/user
echo "HOME = $HOME"

DATE=`date +%Y_%m_%d_%H%M`
echo "DATE = $DATE"

#Store the time as seperate parts so we can create the matching folder
# structure later on.
YEAR=`date +%Y`
MONTH=`date +%m`
DAY=`date +%d`
TIME=`date +%H%M`


ENVIRONMENT="env LANG=C"
echo "ENVIRONMENT = $ENVIRONMENT"

PROGRAM=gphoto2
echo "PROGRAM = $PROGRAM"

DEBUG_FILE="$HOME/MI_debug/MI_capture_canon_debug_$DATE.txt"
echo "DEBUG_FILE = $DEBUG_FILE"

#DEBUG="--debug --debug-logfile=$DEBUG_FILE"
echo "DEBUG = $DEBUG"

#CAMERA=--camera\ "Canon Digital IXUS 400 (PTP mode)"
echo "CAMERA = $CAMERA"

#PORT=--port\ "usb:"
echo "PORT = $PORT"

FILEPATH=images/$YEAR/$MONTH/$DAY
FILENAME=$TIME.jpg

#Print the expected path and filename out
echo "Creating File: $FILEPATH/$FILENAME"

#Create the directory structur for the day, this works the first time it is run
# for the day. It will not cause error when if the dirs already exist
/bin/mkdir -p $HOME/MI_photos/$FILEPATH

echo "List of all photos taken today:"
/bin/ls $HOME/MI_photos/$FILEPATH 
echo

#IMG_FILENAME="$HOME/MI_photos/$DATE.jpg"
IMG_FILENAME="$HOME/MI_photos/$FILEPATH/$FILENAME"
echo "IMG_FILENAME = $IMG_FILENAME"

CAMERA_ARGS="$ENVIRONMENT $PROGRAM $DEBUG $CAMERA $PORT"
echo "CAMERA_ARGS = $CAMERA_ARGS"

echo "attempting the capture and download"
# capture image and download to computer hard drive.
$CAMERA_ARGS --filename "$IMG_FILENAME" --capture-image-and-download
echo "capture and download returned $?"
echo "checking that the file exists"
#sleep 4

#check that the file was downloaded from the camera
if [ -f $IMG_FILENAME ]
then
	echo "Image successfully downloaded from camera."
else
	echo "ERROR: failed to download image from camera."
  echo "Sleeping for 2 seconds before attempting a repair"
  #sleep 2
  echo "Attempting to reset the Camera with the power cycle hack!!"
  date > /home/user/logs/dynamicReset.log
  echo "Warning, if this file exists something bad has happened and we neede to turn the camera off / on and init it during the day..." >> /home/user/logs/dynamicReset.log
  /bin/sh /home/user/libwcl/scripts/cameraReset.sh >> /home/user/logs/dynamicReset.log
#  sleep 1
  exit 1
fi

#Create the dirs on wcl starting with year.
echo "mkdir images/$YEAR" | sftp snappybackup@wcl.ml.unisa.edu.au
#Create the Month dir, this will fail after it has run once!! but that is expected 
echo "mkdir images/$YEAR/$MONTH" | sftp snappybackup@wcl.ml.unisa.edu.au

#Create the Day dir
echo "mkdir images/$YEAR/$MONTH/$DAY" | sftp snappybackup@wcl.ml.unisa.edu.au

#Make sure everyone can read it when it is copied to wcl
chmod og+r $IMG_FILENAME

echo "attempting to backup the image to the wcl."

# attempt to send the file to the wcl.
echo "put $IMG_FILENAME" | sftp snappybackup@wcl.ml.unisa.edu.au:$FILEPATH
echo "sftp returned $?"

echo "attempting to compress the debug output"
# compress the debug info
gzip $DEBUG_FILE
echo "debug compression returned $?"
