dnl $Id$
dnl config.m4 for extension bytearray

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(bytearray, for bytearray support,
dnl Make sure that the comment is aligned:
dnl [  --with-bytearray             Include bytearray support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(bytearray, whether to enable bytearray support,
Make sure that the comment is aligned:
[  --enable-bytearray           Enable bytearray support])

if test "$PHP_BYTEARRAY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-bytearray -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/bytearray.h"  # you most likely want to change this
  dnl if test -r $PHP_BYTEARRAY/$SEARCH_FOR; then # path given as parameter
  dnl   BYTEARRAY_DIR=$PHP_BYTEARRAY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for bytearray files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BYTEARRAY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BYTEARRAY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the bytearray distribution])
  dnl fi

  dnl # --with-bytearray -> add include path
  dnl PHP_ADD_INCLUDE($BYTEARRAY_DIR/include)

  dnl # --with-bytearray -> check for lib and symbol presence
  dnl LIBNAME=bytearray # you may want to change this
  dnl LIBSYMBOL=bytearray # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BYTEARRAY_DIR/lib, BYTEARRAY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BYTEARRAYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong bytearray lib version or lib not found])
  dnl ],[
  dnl   -L$BYTEARRAY_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(BYTEARRAY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(bytearray, bytearray.c, $ext_shared)
fi
