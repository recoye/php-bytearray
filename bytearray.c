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

/* If you declare any globals in php_bytearray.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(bytearray)
*/

/* True global resources - no need for thread safety here */
static int le_bytearray;

static int le_bytearray_descriptor;

ZEND_BEGIN_ARG_INFO(arg_value, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arg_bytes, 0)
    ZEND_ARG_OBJ_INFO(0, bytes, "ByteArray", 0)
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
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("bytearray.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_bytearray_globals, bytearray_globals)
    STD_PHP_INI_ENTRY("bytearray.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_bytearray_globals, bytearray_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_bytearray_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_bytearray_init_globals(zend_bytearray_globals *bytearray_globals)
{
    bytearray_globals->global_value = 0;
    bytearray_globals->global_string = NULL;
}
*/
/* }}} */


zend_class_entry *bytearray_ce;
/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bytearray)
{
    /* If you have INI entries, uncomment these lines 
    REGISTER_INI_ENTRIES();
    */

    /*
     * 注册资源类型
     */
    le_bytearray_descriptor = zend_register_list_destructors_ex(NULL, NULL, PHP_BYTEARRAY_RES_NAME, module_number);

    /**
     * 注册类
     */
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "ByteArray", bytearray_functions);//第二个参数为类名，第三个参数为函数表
    bytearray_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);//注册类

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
    /* uncomment this line if you have INI entries
    UNREGISTER_INI_ENTRIES();
    */
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

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

PHP_METHOD(ByteArray, __construct) {
    /**
     * 函数构造时创建一个资源
     */
    zval *argv;
    char *res ;
    zval *data;
    long write = 0;
    long count = 1;
    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());

    // data的存储位置
    ALLOC_INIT_ZVAL(data);
    // 读取参数
    if( ZEND_NUM_ARGS() > 0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == SUCCESS ){
        write = Z_STRLEN_P(argv);
        count = (long) ceil((double)write / PHP_BYTEARRAY_RES_SIZE);
        res = emalloc(sizeof(char *) * count * PHP_BYTEARRAY_RES_SIZE);
        memcpy(res, Z_STRVAL_P(argv), Z_STRLEN_P(argv));
    } else {
        res = emalloc(sizeof(char *) * PHP_BYTEARRAY_RES_SIZE);
    }

    // 注册的资源
    ZEND_REGISTER_RESOURCE(data, res, le_bytearray_descriptor);

    // 写到当前类的属性中
    zend_update_property(ce, getThis(), ZEND_STRL("_data_res"), data TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_data_count"), count TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_write_index"), write TSRMLS_CC);
    zend_update_property_long(ce, getThis(), ZEND_STRL("_read_index"), 0 TSRMLS_CC);
}

PHP_METHOD(ByteArray, __destruct) {
    // 释放资源
    char *res;
    zval *data;

    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    data = zend_read_property(ce, getThis(), ZEND_STRL("_data_res"), 0 TSRMLS_CC);

    ZEND_FETCH_RESOURCE(res, char *, &data, -1, PHP_BYTEARRAY_RES_NAME, le_bytearray_descriptor);
    
    efree(data);
    efree(res);
}

PHP_METHOD(ByteArray, compress){
}

PHP_METHOD(ByteArray, readBoolean){
    bool b;

    if ( 1 != bytearray_read_bytes(getThis(), &b, sizeof(bool) TSRMLS_CC) ) {
        RETURN_NULL();
    }

    RETURN_BOOL(b);
}


PHP_METHOD(ByteArray, readByte){
    long d;
    char *b = emalloc(sizeof(char));

    if ( 1 != bytearray_read_bytes(getThis(), b, 1 TSRMLS_CC) ) {
        RETURN_NULL();
    }

    d = *b & 0xFF;
    efree(b);

    RETURN_LONG(d);
}

PHP_METHOD(ByteArray, readBytes){

}

PHP_METHOD(ByteArray, readDouble){
    double d;

    if ( 1 != bytearray_read_bytes(getThis(), &d, sizeof(double) TSRMLS_CC) ) {
        RETURN_NULL();
    }

    RETURN_DOUBLE(d);
}

PHP_METHOD(ByteArray, readFloat){
}

PHP_METHOD(ByteArray, readInt){
    int d;

    if ( 1 != bytearray_read_bytes(getThis(), &d, sizeof(int) TSRMLS_CC) ) {
        RETURN_NULL();
    }

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

    utfBytes = emalloc(sizeof(char) * len);

    if ( 1 != bytearray_read_bytes(getThis(), utfBytes, len TSRMLS_CC) ) {
        efree(shortByte);
        efree(utfBytes);
        RETURN_NULL();
    }

    ALLOC_INIT_ZVAL(val);
    ZVAL_STRINGL(val, utfBytes, len, 1);

    efree(shortByte);
    efree(utfBytes);

    RETURN_ZVAL(val, 0, 1);
}

PHP_METHOD(ByteArray, readUTFBytes){

}

PHP_METHOD(ByteArray, toString){
    char *res;
    zval *data, *index, *val;
    zval *self = getThis();

    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);

    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    ZEND_FETCH_RESOURCE_NO_RETURN(res, char *, &data, -1, PHP_BYTEARRAY_RES_NAME, le_bytearray_descriptor);

    ALLOC_INIT_ZVAL(val);
    ZVAL_STRINGL(val, res, Z_LVAL(*index), 1);

    RETURN_ZVAL(val, 0, 1);
}

PHP_METHOD(ByteArray, uncompress){

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

    *b = Z_LVAL_P(argv) & 0xFF;

    bytearray_write_bytes(getThis(), b, 1 TSRMLS_CC);
    efree(b);
}

PHP_METHOD(ByteArray, writeBytes){

}

PHP_METHOD(ByteArray, writeDouble){
    zval *argv;
    // 读取参数
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &argv) == FAILURE )
    {
        RETURN_NULL();
    }

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

    bytearray_write_bytes(getThis(), &Z_LVAL_P(argv), sizeof(int) TSRMLS_CC);
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

void bytearray_int_to_byte(int i, char *bytes){
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
    char *res;
    zval *data, *index, *count;
    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);
    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);
    count = zend_read_property(ce, self, ZEND_STRL("_data_count"), 0 TSRMLS_CC);

    ZEND_FETCH_RESOURCE_NO_RETURN(res, char *, &data, -1, PHP_BYTEARRAY_RES_NAME, le_bytearray_descriptor);

    // 内存不足时
    if (Z_LVAL(*index) + size > Z_LVAL(*count) * PHP_BYTEARRAY_RES_SIZE) {
        Z_LVAL(*count) += (long)ceil((double)size/PHP_BYTEARRAY_RES_SIZE);
        res = erealloc(res, sizeof(char *) * Z_LVAL(*count) * PHP_BYTEARRAY_RES_SIZE);

        // 注册的资源
        ZEND_REGISTER_RESOURCE(data, res, le_bytearray_descriptor);

        // 写到当前类的属性中
        zend_update_property(ce, self, ZEND_STRL("_data_res"), data TSRMLS_CC);
        zend_update_property(ce, self, ZEND_STRL("_data_count"), count TSRMLS_CC);
    }

    memcpy(res + Z_LVAL(*index), bytes, size);

    Z_LVAL(*index) += size;

    // 更新位置
    zend_update_property(ce, self, ZEND_STRL("_write_index"), index TSRMLS_CC);

    return 1;
}

int bytearray_read_bytes(zval *self, void *bytes, long size TSRMLS_DC) {
    char *res;
    zval *data, *index, *windex;

    zend_class_entry *ce;
    ce = Z_OBJCE_P(self);
    data = zend_read_property(ce, self, ZEND_STRL("_data_res"), 0 TSRMLS_CC);
    index = zend_read_property(ce, self, ZEND_STRL("_read_index"), 0 TSRMLS_CC);
    windex = zend_read_property(ce, self, ZEND_STRL("_write_index"), 0 TSRMLS_CC);

    if ( Z_LVAL(*index) + size > Z_LVAL(*windex) ) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "read index overflow!");
        return 0;
    }

    ZEND_FETCH_RESOURCE_NO_RETURN(res, char *, &data, -1, PHP_BYTEARRAY_RES_NAME, le_bytearray_descriptor);
    
    memset(bytes, 0x0, size);
    memcpy(bytes, res + Z_LVAL(*index), size);

    Z_LVAL(*index) += size;

    // 更新位置
    zend_update_property(ce, self, ZEND_STRL("_read_index"), index TSRMLS_CC);

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
