#!/bin/bash

PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
echo "PATH = $PATH"

HOME=/home/user
echo "HOME = $HOME"

DATE=`date +%Y_%m_%d_%H%M`
echo "DATE = $DATE"

#ENVIRONMENT="env LANG=C"
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

CAMERA_ARGS="$ENVIRONMENT $PROGRAM $DEBUG $CAMERA $PORT"
echo "CAMERA_ARGS = $CAMERA_ARGS"

echo "attempting to set capture mode to on"
# turn on capture mode... otherwise the set-config calls wont work
$CAMERA_ARGS --set-config capture="on"
echo "setting capture returned $?"

  echo "Checking whether or not capture was acturally turned on (after a short nap"

# setting capture to on can fail and still return 0. If capture has not been
# set correctly then --list-config will be missing a bunch of option. This code
# keeps retrying to set capture to on until --list-config confirms that the
# other options are available
#
# It is important for capture to be set to on because otherwise the remaining
# config calls will not do anything.
while [ `$CAMERA_ARGS --list-config | wc -l` != 24 ]
do
  echo "capturing has not been set yet, will try again after a short nap"
  sleep 25

  echo "attempting to set capture mode to on"
  # turn on capture mode... otherwise the set-config calls wont work
  $CAMERA_ARGS --set-config capture="on"
  echo "setting capture returned $?"

  echo "Checking whether or not capture was acturally turned on (after a short nap"

  sleep 10
done

echo "Capturing has been turned on"

# By default image quality is set to fine. This command beefs up the image a
# bit
echo "setting image quality to superfine"
$CAMERA_ARGS --set-config canonimgquality="2"
echo "set image quality returned $?"

# image size is large by default, therefore don't do anything.

#echo "setting image size to large"
# set the image size
#$CAMERA_ARGS --set-config canonimgsize="0"
#echo "set image size returned $?"

# photoeffect is Off (0) by default. Therefore we don't need to do anything
# here.

#echo "turning off photoeffects"
# turn off any photo effects
#$CAMERA_ARGS --set-config photoeffect="0"
#echo "turning off photoeffects returned $?"

# zoom is 0 by default. Therefore don't do anything.
#echo "setting zoom to bottom"
#$CAMERA_ARGS --set-config zoom="0"
#echo "setting zoom returned $?"

echo "turning off the flash"
# turn off the flash
$CAMERA_ARGS --set-config canonflashmode="0"
echo "turning off the flash returned $?"

# by default the camera aperture is set to 2.8. Set it to automatic. So that
# lighting conditions to no effect the shot
echo "setting aperture to automatic"
$CAMERA_ARGS --set-config aperture="0"
echo "setting the aperture returned $?"

# by default shutterspeed is set to 1/125. Set this to auto so that light does
# not effect the shot
echo "setting shutter speed to automatic"
$CAMERA_ARGS --set-config shutterspeed="0"
echo "setting the shutter speed returned $?"

# by default afdistance is set to Off. Set to long distance.
echo "setting long distance to long distance"
$CAMERA_ARGS --set-config afdistance="2"
echo "setting afdistance returned $?"

exit 0 
