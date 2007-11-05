#!/bin/sh
# $ClearChain: projects/clearchain/autogen.sh,v 1.1 2006/03/17 05:33:23 benjsc Exp $
# 
# Simple little script to build all the makefiles
#
#
#
ACLOCALMINVERSION="1.10"
AUTOHEADERMINVERSION="2.61"
AUTOCONFMINVERSION="2.61"

ACLOCAL=`which aclocal`
AUTOCONF=`which autoconf`
AUTOHEADER=`which autoheader`
AUTOMAKE=`which automake`

#
#  ACLOCAL
#

if test "x$ACLOCAL" = "x"; then
	echo "You must install aclocal (at least version $ACLOCALMINVERSION)";
	exit 1
fi

$ACLOCAL --version | grep $ACLOCALMINVERSION > /dev/null
if test $? -ne 0; then
	echo "You version of aclocal is too old, you  at least $ACLOCALMINVERSION";
	exit 1
fi

$ACLOCAL  -I cfg

#
# AUTOCONF
#

if test "x$AUTOCONF" = "x"; then
	echo "You must install autoconf (at least version $AUTOCONFMINVERSION)";
	exit 1
fi

$AUTOCONF --version |grep $AUTOCONFMINVERSION > /dev/null
if test $? -ne 0; then
	echo "You version of autoconf is too old, you  at least $AUTOCONFMINVERSION";
	exit 1;
fi

$AUTOCONF

#
# AUTOHEADER
#
if test "x$AUTOHEADER" = "x"; then
	echo "You must install autoheader (at least version $AUTOHEADERMINVERSION)";
	exit 1
fi

$AUTOHEADER --version |grep $AUTOHEADERMINVERSION > /dev/null
if test $? -ne 0; then
	echo "You version of autohead is too old, you  at least $AUTOHEADERMINVERSION";
	exit 1;
fi

$AUTOHEADER

#
# AUTOMAKE
#
AUTOMAKEMINVERSION="1.9"
am_version=`${AUTOMAKE:-automake} --version 2>/dev/null|head -n 1| sed -e 's/^.* \([0-9]\)/\1/' -e 's/[a-z]* *$//' -e 's/\(.*\)\(-p.*\)/\1/'`	
if test -z "$am_version"; then	
  echo "buildconf: automake not found."	
  echo "            You  automake version $AUTOMAKEMINVERSION or newer installed."	
  exit 1	
fi	
IFS=.; set $am_version; IFS=' '	
if test "$1" = "1" -a "$2" -lt "9" || test "$1" -lt "1"; then	
  echo "buildconf: automake version $am_version found."	
  echo "            You  automake version $AUTOMAKEMINVERSION or newer installed."	
  echo "            If you have a sufficient automake installed, but it"	
  echo "            is not named 'automake', then try setting the"	
  echo "            AUTOMAKE environment variable."	
  exit 1	
fi
if test "x$AUTOMAKE" = "x"; then
    echo "You must install automake (at least version $AUTOMAKEMINVERSION)";
    exit 1
fi

$AUTOMAKE -a
