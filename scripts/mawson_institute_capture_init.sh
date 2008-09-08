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

CAMERA_ARGS="$ENVIRONMENT $PROGRAM $DEBUG $CAMERA $PORT"
echo "CAMERA_ARGS = $CAMERA_ARGS"

echo "setting capture mode to on"
# turn on capture mode... otherwise the set-config calls wont work
$CAMERA_ARGS --set-config capture="on"
echo "set capture to on returned $?"

echo "setting image quality to superfine"
# set the image quality
$CAMERA_ARGS --set-config canonimgquality="2"
echo "set image quality returned $?"

echo "setting image size to large"
# set the image size
$CAMERA_ARGS --set-config canonimgsize="0"
echo "set image size returned $?"

echo "turning off photoeffects"
# turn off any photo effects
$CAMERA_ARGS --set-config photoeffect="0"
echo "turning off photoeffects returned $?"

echo "setting zoom to bottom"
# zoom the camera out completely
$CAMERA_ARGS --set-config zoom="0"
echo "setting zoom returned $?"

echo "turning off the flash"
# turn off the flash
$CAMERA_ARGS --set-config canonflashmode="0"
echo "turning off the flash returned $?"

echo "setting aperture to automatic"
# set aperture to auto
$CAMERA_ARGS --set-config aperture="0"
echo "setting the aperture returned $?"

echo "setting shutter speed to automatic"
# set shutter speed to auto
$CAMERA_ARGS --set-config shutterspeed="0"
echo "setting the shutter speed returned $?"

echo "setting long distance to long distance"
# set AF distance to "Long Distance"
$CAMERA_ARGS --set-config afdistance="2"
echo "setting long distance returned $?"
