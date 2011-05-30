#!/bin/sh
#
# This script installs all required modules on a Ubuntu System
# This script does *NOT* install required dependancies for ptgrey module, 
#   so do that by yourself, or configure using 
#    $ configure --enable-all --enable-camera-ptgrey=no
#   to get by without it. 

# Add the ARToolKitPlus repo to our sources. 
# Copy, replace etc the file because otherwise it'll get run multipule times
# And that'll append multipule repos
# And that's probably bad. Better way to do this would be to check if repo's in 
# sources list, but that's alot of effort. Feel free to fix. 

cp /etc/apt/sources.list /etc/apt/sources.list.backup
echo '' >> /etc/apt/sources.list
echo '# ARToolKitPlus repository' >> /etc/apt/sources.list
echo 'deb http://ppa.launchpad.net/michael-the-drummer/wcl/ubuntu lucid main' >> /etc/apt/sources.list

apt-get update

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
artoolkitplus-dev \
libcwiid-dev 

# Replace the sources.list with the backup
mv /etc/apt/sources.list.backup /etc/apt/sources.list
