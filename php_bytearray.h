/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_BYTEARRAY_H
#define PHP_BYTEARRAY_H

extern zend_module_entry bytearray_module_entry;
#define phpext_bytearray_ptr &bytearray_module_entry

#ifdef PHP_WIN32
#	define PHP_BYTEARRAY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_BYTEARRAY_API __attribute__ ((visibility("default")))
#else
#	define PHP_BYTEARRAY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <zlib.h>

#ifdef PHP_WIN32
# define PHP_ZLIB_MODIFIER 100
#else
# define PHP_ZLIB_MODIFIER 1000
#endif

// 定义资源文件
#define PHP_BYTEARRAY_RES_NAME "ByteArray"
#define PHP_BYTEARRAY_RES_SIZE 256 

// 类型
typedef unsigned char byte;

// 方法
PHP_MINIT_FUNCTION(bytearray);
PHP_MSHUTDOWN_FUNCTION(bytearray);
PHP_RINIT_FUNCTION(bytearray);
PHP_RSHUTDOWN_FUNCTION(bytearray);
PHP_MINFO_FUNCTION(bytearray);

PHP_METHOD(ByteArray, __construct);
PHP_METHOD(ByteArray, __destruct);
PHP_METHOD(ByteArray, compress);
PHP_METHOD(ByteArray, readBoolean);
PHP_METHOD(ByteArray, readByte);
PHP_METHOD(ByteArray, readBytes);
PHP_METHOD(ByteArray, readDouble);
PHP_METHOD(ByteArray, readFloat);
PHP_METHOD(ByteArray, readInt);
PHP_METHOD(ByteArray, readMultiByte);
PHP_METHOD(ByteArray, readObject);
PHP_METHOD(ByteArray, readShort);
PHP_METHOD(ByteArray, readUnsignedByte);
PHP_METHOD(ByteArray, readUnsignedInt);
PHP_METHOD(ByteArray, readUnsignedShort);
PHP_METHOD(ByteArray, readUTF);
PHP_METHOD(ByteArray, readUTFBytes);
PHP_METHOD(ByteArray, available);
PHP_METHOD(ByteArray, toString);
PHP_METHOD(ByteArray, uncompress);
PHP_METHOD(ByteArray, writeBoolean);
PHP_METHOD(ByteArray, writeByte);
PHP_METHOD(ByteArray, writeBytes);
PHP_METHOD(ByteArray, writeDouble);
PHP_METHOD(ByteArray, writeFloat);
PHP_METHOD(ByteArray, writeInt);
PHP_METHOD(ByteArray, writeMultiByte);
PHP_METHOD(ByteArray, writeObject);
PHP_METHOD(ByteArray, writeShort);
PHP_METHOD(ByteArray, writeUnsignedInt);
PHP_METHOD(ByteArray, writeUTF);
PHP_METHOD(ByteArray, writeUTFBytes);

/* 
	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(bytearray)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(bytearray)
*/

/* In every utility function you add that needs to use variables 
   in php_bytearray_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as BYTEARRAY_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

void bytearray_short_to_bytes(short n, char *bytes);
void bytearray_int_to_byte(int i, char *bytes);
short bytearray_bytes_to_short(byte *bytes);
int bytearray_bytes_to_int(byte *bytes);
int bytearray_write_bytes(zval *self, void *bytes, long size TSRMLS_DC);
int bytearray_read_bytes(zval *self, void *bytes, long size TSRMLS_DC);

#ifdef ZTS
#define BYTEARRAY_G(v) TSRMG(bytearray_globals_id, zend_bytearray_globals *, v)
#else
#define BYTEARRAY_G(v) (bytearray_globals.v)
#endif

#endif	/* PHP_BYTEARRAY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
