#
# These macros are used for other tests
# which don't fit in anywhere else
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

#
# Hacks that just are needed to fix autotools
# -----------------------------------------
AC_DEFUN([WCL_HACKS],[

# The mac uses objective C code for the 1394cam module
# for some reason its not possible to conditionally 
# include the source file and not have to declare 
# an objective C dependancy. So we hack in what
# it should be by default

AM_CONDITIONAL([am__fastdepOBJC], false)

])
