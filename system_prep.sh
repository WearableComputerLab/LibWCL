#!/bin/sh
#
# This script installs all required modules on a Ubuntu System
# This script does *NOT* install required dependancies for ptgrey module, 
#   so do that by yourself, or configure using 
#    $ configure --enable-all --enable-camera-ptgrey=no
#   to get by without it. 
# This script also assumes that you have seperatly installed ARToolKitPlus. 

apt-get install \
build-essential \
automake \
flex \
bison \
libraw1394-dev \
libdc1394-22-dev \
libavcodec-dev \
libavformat-dev \
libswscale-dev \

