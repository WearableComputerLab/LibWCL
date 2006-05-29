#
# These macros allow compilation of libwcl with MSVC++7.
# Credit for some of this should got to Systems In Motion as some of these
# were taken from their aclocal.m4 macro in the Coin-2 library
#

#
# Wake up the user macro...
# -----------------------------------------
#
AC_DEFUN([WCL_FRIDGE_CHECK], [
AC_MSG_CHECKING([Coke in the Fridge])
AC_MSG_RESULT([status: Low])
AC_MSG_WARN([Coke level is low, please restock the fridge!])
])
