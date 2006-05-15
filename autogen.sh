# $ClearChain: projects/clearchain/autogen.sh,v 1.1 2006/03/17 05:33:23 benjsc Exp $
# 
# Simple little script to build all the makefiles
aclocal 
autoconf
autoheader
automake -a
