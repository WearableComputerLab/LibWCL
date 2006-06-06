# !/bin/sh
# $ClearChain: projects/clearchain/autogen.sh,v 1.1 2006/03/17 05:33:23 benjsc Exp $
# 
# Simple little script to build all the makefiles
#
#
#
ACLOCALMINVERSION=1.8.5
AUTOMAKEMINVERSION=1.8.5
AUTOHEADERMINVERSION=2.5
AUTOCONFMINVERSION=2.5

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
	echo "You version of aclocal is too old, you need at least $ACLOCALMINVERSION";
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
	echo "You version of autoconf is too old, you need at least $AUTOCONFMINVERSION";
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
	echo "You version of autoconf is too old, you need at least $AUTOHEADERMINVERSION";
	exit 1;
fi

$AUTOHEADER

#
# AUTOMAKE
#
if test "x$AUTOMAKE" = "x"; then
	echo "You must install automake (at least version $AUTOMAKEMINVERSION)";
	exit 1
fi

$AUTOMAKE --version |grep $AUTOMAKEMINVERSION > /dev/null
if test $? -ne 0; then
	echo "You version of automake is too old, you need at least $AUTOMAKEMINVERSION";
	exit 1;
fi

$AUTOMAKE -a
