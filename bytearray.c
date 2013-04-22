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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_bytearray.h"
#include <stdbool.h>

/* If you declare any globals in php_bytearray.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(bytearray)

/* True global resources - no need for thread safety here */
static int le_bytearray;

zend_class_entry *bytearray_ce;

ZEND_BEGIN_ARG_INFO(arg_value, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arg_bytes, 0)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arg_mutli_byte, 0)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

/* {{{ bytearray_functions[]
 *
 * Every user visible function must have an entry in bytearray_functions[].
 */
const zend_function_entry bytearray_functions[] = {
    PHP_ME(ByteArray, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(ByteArray, __destruct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
    PHP_ME(ByteArray, compress, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readBoolean, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readByte, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readBytes, arg_bytes, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readDouble, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readFloat, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readInt, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readMultiByte, arg_mutli_byte, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readObject, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readShort, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readUnsignedByte, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readUnsignedInt, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readUnsignedShort, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readUTF, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, readUTFBytes, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, available, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, toString, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, uncompress, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeBoolean, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeByte, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeBytes, arg_bytes, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeDouble, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeFloat, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeInt, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeMultiByte, arg_mutli_byte, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeObject, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeShort, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeUnsignedInt, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeUTF, arg_value, ZEND_ACC_PUBLIC)
    PHP_ME(ByteArray, writeUTFBytes, arg_value, ZEND_ACC_PUBLIC)
    PHP_FE_END  /* Must be the last line in bytearray_functions[] */
};
/* }}} */

/* {{{ bytearray_module_entry
 */
zend_module_entry bytearray_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "bytearray",
    bytearray_functions,
    PHP_MINIT(bytearray),
    PHP_MSHUTDOWN(bytearray),
    PHP_RINIT(bytearray),       /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(bytearray),   /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(bytearray),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BYTEARRAY
ZEND_GET_MODULE(bytearray)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("bytearray.max_block",      "20", PHP_INI_ALL, OnUpdateLong, max_block, zend_bytearray_globals, bytearray_globals)
    STD_PHP_INI_ENTRY("bytearray.block_size",    "512", PHP_INI_ALL, OnUpdateLong, block_size, zend_bytearray_globals, bytearray_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_bytearray_init_globals
 */
/*
static void php_bytearray_init_globals(zend_bytearray_globals *bytearray_globals)
{
    bytearray_globals->max_block = 36;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bytearray)
{
#ifdef ZTS
    ts_allocate_id(
            &bytearray_globals_id,
            sizeof(zend_bytearray_globals),
            NULL, NULL);
#endif
	
    REGISTER_INI_ENTRIES();

	/*
     * 注册类
     */
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ByteArray", bytearray_functions);//第二个参数为类名，第三个参数为函数表
    bytearray_ce = zend_register_internal_class(&ce TSRMLS_CC);//注册类

    /**
     * 注册私有变量
     */
    zend_declare_property_null(bytearray_ce, ZEND_STRL("_data_res"), ZEND_ACC_PRIVATE TSRMLS_CC); // 二进制数据
    zend_declare_property_null(bytearray_ce, ZEND_STRL("_data_count"), ZEND_ACC_PRIVATE TSRMLS_CC); // 数据块数量 
    zend_declare_property_null(bytearray_ce, ZEND_STRL("_read_index"), ZEND_ACC_PRIVATE TSRMLS_CC); // 读的位置
    zend_declare_property_null(bytearray_ce, ZEND_STRL("_write_index"), ZEND_ACC_PRIVATE TSRMLS_CC); // 写的位置

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(bytearray)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(bytearray)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(bytearray)
{
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(bytearray)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "bytearray support", "enabled");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

PHP_METHOD(ByteArray, __construct) {
    /**
     * 函数构造时创建一个资源
     */
    zval *argv;
    zval *data;
    long write = 0;
    long count = 1;
    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());

    // data的存储位置
    ALLOC_INIT_ZVAL(data);
    // 读取参数
    if( ZEND_NUM_ARGS() > 0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == SUCCESS ){
		convert_to_string(argv);

        write = Z_STRLEN_P(argv);
        count = (long) ceil((double)write / BYTEARRAY_G(block_size));

		// 这里需要设置最大申请的空间，放在Php.ini里面
		if ( count > BYTEARRAY_G(max_block) ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Allowed block of %ld exhausted,tried to allocate %ld", BYTEARRAY_G(max_block), count);
			Z_STRVAL_P(data) = emalloc(sizeof(char *) * BYTEARRAY_G(block_size));
			write = 0;
			count = 1;
		} else {
			Z_STRVAL_P(data) = emalloc(sizeof(char *) * count * BYTEARRAY_G(block_size));
			memcpy(Z_STRVAL_P(data), Z_STRVAL_P(argv), Z_STRLEN_P(argv));
		}
    } else {
		Z_STRVAL_P(data) = emalloc(sizeof(char *) * BYTEARRAY_G(block_size));
    }

    // 写到当前类的属性中
    zend_update_property(ce, getThis(), ZEND_STRL("_data_res"), data TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_data_count"), count TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_write_index"), write TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_read_index"), 0 TSRMLS_CC);
}

PHP_METHOD(ByteArray, __destruct) {
    // 释放资源
    zval *data;

    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    data = zend_read_property(ce, getThis(), ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    
	efree(Z_STRVAL_P(data));
    efree(data);
}

// 压缩
PHP_METHOD(ByteArray, compress){
    char *cres;
    zval *data, *index, *count;
    long level, len;
    int status;

    level = Z_DEFAULT_COMPRESSION;

    if ( (level < -1 ) || (level > 9) ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "compression level (%ld) mouse be within -1...9", level);
        RETURN_FALSE;
    }

    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    data = zend_read_property(ce, getThis(), ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    count = zend_read_property(ce, getThis(), ZEND_STRL("_data_count"), 0 TSRMLS_CC);
    index = zend_read_property(ce, getThis(), ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    len = Z_LVAL_P(index) + (Z_LVAL_P(index) / PHP_ZLIB_MODIFIER ) + 15 + 1; /* room for \0 @ ext/zlib/zlib.c */

    if ( len > Z_LVAL_P(count) * BYTEARRAY_G(block_size) ) {
        Z_LVAL_P(count)++;
    }

    cres = emalloc(sizeof(char *) * Z_LVAL_P(count) * BYTEARRAY_G(block_size));

    if ( level >=0 ) {
        status = compress2(cres, &len, Z_STRVAL_P(data), Z_LVAL_P(index), level);
    } else {
        status = compress(cres, &len, Z_STRVAL_P(data), Z_LVAL_P(index));
    }

    if ( status == Z_OK ) {
        efree(Z_STRVAL_P(data));
		Z_STRVAL_P(data) = cres;

        RETURN_TRUE;
    } else {
        efree(cres);
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "compress error,code: %d, message:%s", status, zError(status));
        RETURN_FALSE;
    }
}

PHP_METHOD(ByteArray, readBoolean){
    bool b;

    if ( 1 != bytearray_read_bytes(getThis(), &b, sizeof(bool) TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    RETURN_BOOL(b);
}


PHP_METHOD(ByteArray, readByte){
    long d;
    char *b = emalloc(sizeof(char));

    if ( 1 != bytearray_read_bytes(getThis(), b, 1 TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    d = *b & 0xFF;
    efree(b);

    RETURN_LONG(d);
}

PHP_METHOD(ByteArray, readBytes){
    long offset = -1,length = 0;
    char *bytes;
    zval *data, *index, *windex, *self, *val;
    zend_class_entry *ce;

    self = getThis();
    ce = Z_OBJCE_P(self);
    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_read_index"), 0 TSRMLS_CC);
    windex = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    // 读取参数
    if( ZEND_NUM_ARGS() > 0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &length, &offset) == FAILURE ){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "read bytes params failed!");
        RETURN_FALSE;
    }

    if(offset == -1) {
        offset = Z_LVAL_P(index);
    }

    if ( length < 1 ) {
        length = Z_LVAL_P(windex) + length;
    }

    if ( length < 1 ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "read bytes length failed!");
        RETURN_FALSE;
    }

    if ( offset + length > Z_LVAL(*windex) ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "read index overflow!");
        RETURN_FALSE;
    }
    
    bytes = emalloc(sizeof(char) * length);
    memset(bytes, 0x0, length);
    memcpy(bytes, Z_STRVAL_P(data) + offset, length);

    Z_LVAL_P(index) = offset + length;

    ALLOC_INIT_ZVAL(val);
    ZVAL_STRINGL(val, bytes, length, 1);

    efree(bytes);

    RETURN_ZVAL(val, 0, 1);
}

PHP_METHOD(ByteArray, readDouble){
    double d;

    if ( 1 != bytearray_read_bytes(getThis(), &d, sizeof(double) TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    RETURN_DOUBLE(d);
}

PHP_METHOD(ByteArray, readFloat){
}

PHP_METHOD(ByteArray, readInt){
    char *shortBytes = emalloc(sizeof(char) * 4);
    int d;

    if ( 1 != bytearray_read_bytes(getThis(), shortBytes, 4 TSRMLS_CC) ) {
        RETURN_FALSE;
    }

    d = bytearray_bytes_to_int(shortBytes);

    efree(shortBytes);

    RETURN_LONG(d);
}

PHP_METHOD(ByteArray, readMultiByte){

}

PHP_METHOD(ByteArray, readObject){

}

PHP_METHOD(ByteArray, readShort){
    char *shortBytes = emalloc(sizeof(char) * 2);
    short d;

    if ( 1 != bytearray_read_bytes(getThis(), shortBytes, 2 TSRMLS_CC) ) {
        RETURN_NULL();
    }

    d = bytearray_bytes_to_short(shortBytes);

    efree(shortBytes);

    RETURN_LONG(d);
}

PHP_METHOD(ByteArray, readUnsignedByte){

}

PHP_METHOD(ByteArray, readUnsignedInt){

}

PHP_METHOD(ByteArray, readUnsignedShort){

}

// 字符串
PHP_METHOD(ByteArray, readUTF){
    char* shortByte = emalloc(sizeof(char) * 2);
    char *utfBytes;
    long len;
    zval *val;

    if ( 1 != bytearray_read_bytes(getThis(), shortByte, 2 TSRMLS_CC) ) {
        efree(shortByte);
        RETURN_NULL();
    }

    len = bytearray_bytes_to_short(shortByte);
    efree(shortByte);

    utfBytes = emalloc(sizeof(char) * len);

    if ( 1 != bytearray_read_bytes(getThis(), utfBytes, len TSRMLS_CC) ) {
        efree(utfBytes);
        RETURN_NULL();
    }

    ALLOC_INIT_ZVAL(val);
    ZVAL_STRINGL(val, utfBytes, len, 1);

    efree(utfBytes);

    RETURN_ZVAL(val, 0, 1);
}

PHP_METHOD(ByteArray, readUTFBytes){

}

PHP_METHOD(ByteArray, available){
	zval *rIndex, *wIndex;
	zval *self = getThis();
    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);

    rIndex = zend_read_property(ce, self, ZEND_STRL("_read_index"), 0 TSRMLS_CC);
    wIndex = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

	if (Z_LVAL_P(rIndex) < Z_LVAL_P(wIndex)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_METHOD(ByteArray, toString){
    zval *data, *index, *val;
    zval *self = getThis();

    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);

    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    ALLOC_INIT_ZVAL(val);
    ZVAL_STRINGL(val, Z_STRVAL_P(data), Z_LVAL_P(index), 1);

    RETURN_ZVAL(val, 0, 1);
}

// 解压
PHP_METHOD(ByteArray, uncompress){
    char *cres, *s1 = NULL, *s2 = NULL;
    zval *data, *index, *count;
    int len;
    int status;
    unsigned int factor = 1, maxfactor=16;
    long limit = 0;
    unsigned long plength = 0, length;

    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    data = zend_read_property(ce, getThis(), ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    count = zend_read_property(ce, getThis(), ZEND_STRL("_data_count"), 0 TSRMLS_CC);
    index = zend_read_property(ce, getThis(), ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    plength = limit;
    /**
     * zlib::uncompress() wants to know the output data length
     * if none was gived as a paramter
     * we try from input length * 2 up to input length * 2 ^ 15
     * doubling it whenever it wasn't big enough
     * that should be enough for all real life cases
     * @ ext/zlib/zlib.c
     */
    do{
        length = plength ? plength : (unsigned long)Z_LVAL_P(index) * (1 << factor++);
        s2 = (char *)erealloc(s1, length);
        status = uncompress(s2, &length, Z_STRVAL_P(data), Z_LVAL_P(index));
        s1 = s2;
    }while((status == Z_BUF_ERROR) && (!plength) && (factor < maxfactor));

    if ( status == Z_OK ) {
        Z_LVAL_P(count) = (long)ceil((double)length / BYTEARRAY_G(block_size));
        cres = emalloc(sizeof(char) * Z_LVAL_P(count) * BYTEARRAY_G(block_size));

        memcpy(cres, s2, length);

		efree(Z_STRVAL_P(data));
		Z_STRVAL_P(data) = cres;

		Z_LVAL_P(index) = length;

        efree(s2);

        RETURN_TRUE;
    } else {
        efree(s2);
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "uncompress error,code: %d, message:%s", status, zError(status));
        RETURN_FALSE;
    }
}

PHP_METHOD(ByteArray, writeBoolean){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

    int size = sizeof(bool);
    char *b = emalloc(sizeof(char) * size);
	convert_to_boolean(argv);
    *b = Z_BVAL_P(argv);
    bytearray_write_bytes(getThis(), b, size TSRMLS_CC);
    efree(b);
}

PHP_METHOD(ByteArray, writeByte){
    zval *argv;
    char *b = emalloc(sizeof(char));
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }


	if (Z_TYPE_P(argv) != IS_LONG) {
		convert_to_long(argv);
	}

    *b = Z_LVAL_P(argv) & 0xFF;

    bytearray_write_bytes(getThis(), b, 1 TSRMLS_CC);
    efree(b);
}

PHP_METHOD(ByteArray, writeBytes){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

	convert_to_string(argv);

    // 写字符
    bytearray_write_bytes(getThis(), Z_STRVAL_P(argv), Z_STRLEN_P(argv) TSRMLS_CC);
}

PHP_METHOD(ByteArray, writeDouble){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

	convert_to_double(argv);

    bytearray_write_bytes(getThis(), &Z_DVAL_P(argv), sizeof(double) TSRMLS_CC);
}

PHP_METHOD(ByteArray, writeFloat){

}

PHP_METHOD(ByteArray, writeInt){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

	convert_to_long(argv);

    char *shortBytes = emalloc(sizeof(char) * 4);
    bytearray_int_to_bytes((int)Z_LVAL_P(argv), shortBytes);
    bytearray_write_bytes(getThis(), shortBytes, 4 TSRMLS_CC);
    efree(shortBytes);
}

PHP_METHOD(ByteArray, writeMultiByte){

}

PHP_METHOD(ByteArray, writeObject){

}

PHP_METHOD(ByteArray, writeShort){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

	if (Z_TYPE_P(argv) != IS_LONG) {
		convert_to_long(argv);
	}

    char *shortBytes = emalloc(sizeof(char) * 2);
    bytearray_short_to_bytes(Z_LVAL_P(argv), shortBytes);
    bytearray_write_bytes(getThis(), shortBytes, 2 TSRMLS_CC);
    efree(shortBytes);
}

PHP_METHOD(ByteArray, writeUnsignedInt){

}

PHP_METHOD(ByteArray, writeUTF){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

	convert_to_string(argv);

    // 写字符长度
    char *shortByte = emalloc(sizeof(char) * 2);
    memset(shortByte, 0x0, 2);
    bytearray_short_to_bytes(Z_STRLEN_P(argv), shortByte);
    bytearray_write_bytes(getThis(), shortByte, 2 TSRMLS_CC);
    efree(shortByte);

    // 写字符
    bytearray_write_bytes(getThis(), Z_STRVAL_P(argv), Z_STRLEN_P(argv) TSRMLS_CC);
}

PHP_METHOD(ByteArray, writeUTFBytes){

}

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

void bytearray_short_to_bytes(short n, char *bytes){
    memset(bytes,0,2);
    bytes[0] =(char)((0xff00 &n) >> 8);
    bytes[1] = (char)(0xff & n);
}

void bytearray_int_to_bytes(int i, char *bytes){
    memset(bytes,0,sizeof(char) *  4);
    bytes[0] =(char)((0xff000000 & i) >> 24);
    bytes[1] = (char) ((0xff0000 & i) >> 16);
    bytes[2] = (char) ((0xff00 & i) >> 8);
    bytes[3] = (char) (0xff & i);
}
short bytearray_bytes_to_short(byte *bytes){
    return (((byte)bytes[0]) << 8) + bytes[1];
}

int bytearray_bytes_to_int(byte *bytes){
    int addr = bytes[3] & 0xFF;
    addr |= ((bytes[2] << 8) & 0xFF00);
    addr |= ((bytes[1] << 16) & 0xFF0000);
    addr |= ((bytes[0] << 24) & 0xFF000000);
    return addr;
}

int bytearray_write_bytes(zval *self, void *bytes, long size TSRMLS_DC) {
    zval *data, *index, *count;
    zend_class_entry *ce;
	long block;
    ce = Z_OBJCE_P(self);
    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);
    count = zend_read_property(ce, self, ZEND_STRL("_data_count"), 0 TSRMLS_CC);

    // 内存不足时
    if (Z_LVAL_P(index) + size > Z_LVAL_P(count) * BYTEARRAY_G(block_size)) {
		block = (long)ceil((double)size/BYTEARRAY_G(block_size));
		// 这里需要设置最大申请的空间，放在Php.ini里面
		if ( Z_LVAL_P(count)  + block > BYTEARRAY_G(max_block) ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Allowed block of %ld exhausted", BYTEARRAY_G(max_block));
			return 0;
		}

        Z_LVAL_P(count) += block;

        Z_STRVAL_P(data) = erealloc(Z_STRVAL_P(data), sizeof(char *) * Z_LVAL_P(count) * BYTEARRAY_G(block_size));
    }

    memcpy(Z_STRVAL_P(data) + Z_LVAL_P(index), bytes, size);

    Z_LVAL_P(index) += size;

    return 1;
}

int bytearray_read_bytes(zval *self, void *bytes, long size TSRMLS_DC) {
    zval *data, *index, *windex;
	long current;

    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);
    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_read_index"), 0 TSRMLS_CC);
    windex = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

	current = Z_LVAL_P(index) + size;
    if ( current > Z_LVAL_P(windex) ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "read index overflow!tried to read %ld", current);
        return 0;
    }
    
    memset(bytes, 0x0, size);
    memcpy(bytes, Z_STRVAL_P(data) + Z_LVAL_P(index), size);

    Z_LVAL(*index) += size;

    return 1;
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
