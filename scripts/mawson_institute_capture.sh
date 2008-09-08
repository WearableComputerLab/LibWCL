#!/bin/bash

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
echo "PATH = $PATH"

HOME=/home/user
echo "HOME = $HOME"

DATE=`date +%Y_%m_%d_%H%M`
echo "DATE = $DATE"

ENVIRONMENT="env LANG=C"
echo "ENVIRONMENT = $ENVIRONMENT"

PROGRAM=gphoto2
echo "PROGRAM = $PROGRAM"

DEBUG_FILE="$HOME/MI_debug/MI_capture_canon_debug_$DATE.txt"
echo "DEBUG_FILE = $DEBUG_FILE"

DEBUG="--debug --debug-logfile=$DEBUG_FILE"
echo "DEBUG = $DEBUG"

CAMERA=--camera\ "Canon Digital IXUS 400 (PTP mode)"
echo "CAMERA = $CAMERA"

PORT=--port\ "usb:"
echo "PORT = $PORT"

IMG_FILENAME="$HOME/MI_photos/$DATE.jpg"
echo "IMG_FILENAME = $IMG_FILENAME"

CAMERA_ARGS="$ENVIRONMENT $PROGRAM $DEBUG $CAMERA $PORT"
echo "CAMERA_ARGS = $CAMERA_ARGS"

echo "attempting the capture and download"
# capture image and download to computer hard drive.
$CAMERA_ARGS --filename $IMG_FILENAME --capture-image-and-download
echo "capture and download returned $?"

echo "checking that the file exists"
#check that the file was downloaded from the camera
if [ -f $IMG_FILENAME ]
then
	echo "Image successfully downloaded from camera."
else
	echo "ERROR: failed to download image from camera."
	exit
fi

echo "attempting to backup the image to the wcl."

# attempt to send the file to the wcl.
echo "put $IMG_FILENAME" | sftp snappybackup@wcl.ml.unisa.edu.au
echo "sftp returned $?"

echo "attempting to compress the debug output"
# compress the debug info
gzip $DEBUG_FILE
echo "debug compression returned $?"
