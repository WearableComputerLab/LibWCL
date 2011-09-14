#!/bin/sh
# $ClearChain: projects/clearchain/autogen.sh,v 1.1 2006/03/17 05:33:23 benjsc Exp $
# 
# Simple little script to build all the makefiles
#
#
#
ACLOCAL=`which aclocal`
AUTOCONF=`which autoconf`
AUTOHEADER=`which autoheader`
AUTOMAKE=`which automake`

LIBTOOLIZE=`which libtoolize`
if [ "$?" -ne "0" ] ;
then
	echo "Libtoolize not on path! Trying glibtoolize (osx)"
	LIBTOOLIZE=`which glibtoolize`
	if [ "$?" -ne "0" ] ; then
		echo "Nope, couldn't find libtoolize, bailing."
		exit
	else
		echo "Yep, glibtoolize works"
	fi
fi

#
# LIBTOOLIZE
#
# We don't check the libtoolize verision as we don't know if we need to
$LIBTOOLIZE --force

#
#  ACLOCAL
#

ACLOCALMINVERSION="1.9.6"
acl_version=`${ACLOCAL:-aclocal} --version 2>/dev/null|head -n 1| sed -e 's/^.* \([0-9]\)/\1/' -e 's/[a-z]* *$//' -e 's/\(.*\)\(-p.*\)/\1/'`	
if test -z "$acl_version"; then	
  echo "buildconf: aclocal not found."	
  echo "            You  aclocal version $ACLOCALMINVERSION or newer installed."	
  exit 1	
fi	
IFS=.; set $acl_version; IFS=' '	
if test "$1" = "1" -a "$2" -lt "9" || test "$1" -lt "1"; then	
  echo "buildconf: aclocal version $acl_version found."	
  echo "            You need aclocal version $ACLOCALMINVERSION or newer installed."	
  echo "            If you have a sufficient aclocal installed, but it"	
  echo "            is not named 'aclocal', then try setting the"	
  echo "            aclocal environment variable."	
  exit 1	
fi


$ACLOCAL  -I cfg


#
# AUTOCONF
#

AUTOCONFMINVERSION="2.59"
ac_version=`${AUTOCONF:-autoconf} --version 2>/dev/null|head -n 1| sed -e 's/^.* \([0-9]\)/\1/' -e 's/[a-z]* *$//' -e 's/\(.*\)\(-p.*\)/\1/'`	
if test -z "$ac_version"; then	
  echo "buildconf: autoconf not found."	
  echo "            You  autoconf version $AUTOCONFMINVERSION or newer installed."	
  exit 1	
fi	
IFS=.; set $ac_version; IFS=' '	
if test "$1" = "2" -a "$2" -lt "59" || test "$1" -lt "2"; then	
  echo "buildconf: autoconf version $ac_version found."	
  echo "            You need autoconf version $AUTOCONFMINVERSION or newer installed."	
  echo "            If you have a sufficient autoconf installed, but it"	
  echo "            is not named 'autoconf', then try setting the"	
  echo "            AUTOCONF environment variable."	
  exit 1	
fi

$AUTOCONF

#
# AUTOHEADER
#
AUTOHEADERMINVERSION="2.59"

ah_version=`${AUTOHEADER:-autoheader} --version 2>/dev/null|head -n 1| sed -e 's/^.* \([0-9]\)/\1/' -e 's/[a-z]* *$//' -e 's/\(.*\)\(-p.*\)/\1/'`	
if test -z "$ah_version"; then	
  echo "buildconf: autoheader not found."	
  echo "            You  autoheader version $AUTOHEADERMINVERSION or newer installed."	
  exit 1	
fi	
IFS=.; set $ah_version; IFS=' '	
if test "$1" = "2" -a "$2" -lt "59" || test "$1" -lt "2"; then	
  echo "buildconf: autoheader version $ah_version found."	
  echo "            You need autoheader version $AUTOHEADERMINVERSION or newer installed."	
  echo "            If you have a sufficient autoheader installed, but it"	
  echo "            is not named 'autoheader', then try setting the"	
  echo "            AUTOHEADER environment variable."	
  exit 1	
fi

$AUTOHEADER

#
# AUTOMAKE
#
AUTOMAKEMINVERSION="1.9.6"
am_version=`${AUTOMAKE:-automake} --version 2>/dev/null|head -n 1| sed -e 's/^.* \([0-9]\)/\1/' -e 's/[a-z]* *$//' -e 's/\(.*\)\(-p.*\)/\1/'`	
if test -z "$am_version"; then	
  echo "buildconf: automake not found."	
  echo "            You  automake version $AUTOMAKEMINVERSION or newer installed."	
  exit 1	
fi	
IFS=.; set $am_version; IFS=' '	
if test "$1" = "1" -a "$2" -lt "9" || test "$1" -lt "1"; then	
  echo "buildconf: automake version $am_version found."	
  echo "            You need automake version $AUTOMAKEMINVERSION or newer installed."	
  echo "            If you have a sufficient automake installed, but it"	
  echo "            is not named 'automake', then try setting the"	
  echo "            AUTOMAKE environment variable."	
  exit 1	
fi

$AUTOMAKE -a
