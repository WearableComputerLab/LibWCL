#!/bin/bash

ENVIRONMENT="env LANG=C"
PROGRAM=gphoto2
DEBUG="--debug --debug-logfile=MI_capture_canon_debug.txt"
CAMERA=--camera\ "Canon Digital IXUS 400 (PTP mode)"
PORT=--port\ "usb:"
DATE=`date +%Y_%m_%d_%H%M`

IMG_FILENAME="$HOME/MI_photos/$DATE.jpg"

CAMERA_ARGS="$ENVIRONMENT $PROGRAM $DEBUG $CAMERA $PORT"

# turn on capture mode... otherwise the set-config calls wont work
$CAMERA_ARGS --set-config capture="on"

# set the image quality
$CAMERA_ARGS --set-config canonimgquality="2"

echo "Return val = $?"

# set the image size
$CAMERA_ARGS --set-config canonimgsize="0"

# turn off any photo effects
$CAMERA_ARGS --set-config photoeffect="0"

# zoom the camera out completely
$CAMERA_ARGS --set-config zoom="0"

# turn off the flash
$CAMERA_ARGS --set-config canonflashmode="0"

# set aperture to auto
$CAMERA_ARGS --set-config aperture="0"

# set shutter speed to auto
$CAMERA_ARGS --set-config shutterspeed="0"

# set AF distance to "Long Distance"
$CAMERA_ARGS --set-config afdistance="2"

# capture image and download to computer hard drive.
$CAMERA_ARGS --filename $IMG_FILENAME --capture-image-and-download

#check that the file was downloaded from the camera
if [ -f $IMG_FILENAME ]
then
	echo "Image successfully downloaded from camera."
else
	echo "ERROR: failed to download image from camera."
	exit
fi

# attempt to send the file to the wcl.
echo "put $IMG_FILENAME" | sftp snappybackup@wcl.ml.unisa.edu.au

echo "return value was $?"
