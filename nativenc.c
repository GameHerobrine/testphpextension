/* nativenc extension for PHP */

#include "mtrandom.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_nativenc.h"
#include "nativenc_arginfo.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

static zend_object_handlers mtrandom_object_handlers;
static zend_object_handlers bytearray_object_handlers;

typedef struct _z_mtrandom_t {
	mtrandom_t native;
	zend_object std;
} z_mtrandom_t;

typedef struct _z_bytearray_t{
	char* arr;
	zend_long count;
	zend_object std;
} z_bytearray_t;

#define GET_NMTRANDOM(zv) ((z_mtrandom_t*)((char*)(Z_OBJ_P(zv)) - XtOffsetOf(z_mtrandom_t, std)))
#define GET_BYTEARRAY(zv) ((z_bytearray_t*)((char*)(Z_OBJ_P(zv)) - XtOffsetOf(z_bytearray_t, std)))

#define GETOFFSET(obj, dt) ((dt*)((char*)obj - XtOffsetOf(dt, std)))

zend_object *mtrandom_new(zend_class_entry *ce){
	z_mtrandom_t *mtr = zend_object_alloc(sizeof(z_mtrandom_t), ce);
	zend_object_std_init(&mtr->std, ce);
	mtr->std.handlers = &mtrandom_object_handlers;
	return &mtr->std;
}

zend_object *bytearray_new(zend_class_entry *ce){
	z_bytearray_t *mtr = zend_object_alloc(sizeof(z_bytearray_t), ce);
	zend_object_std_init(&mtr->std, ce);
	mtr->std.handlers = &bytearray_object_handlers;
	return &mtr->std;
}
void bytearray_free(zend_object* obj){
	z_bytearray_t* ptr = GETOFFSET(obj, z_bytearray_t);
	free(ptr->arr);
}
zval* bytearray_read_dimension(zend_object *obj, zval *offset, int type, zval *rv){
	z_bytearray_t* ptr = bytearray_object_handlers.offset + (void*)obj;
	int value;
	
	switch(zval_get_type(offset)){
		case IS_TRUE:
			value = 1;
			break;
		case IS_FALSE:
			value = 0;
			break;
		case IS_LONG:
			value = offset->value.lval;
			break;
		default:
			ZVAL_LONG(rv, 0);
			return rv;
	}
	
	if(value >= 0 && value < ptr->count){
		ZVAL_LONG(rv, ptr->arr[value] & 0xff);
		return rv;
	}
	ZVAL_LONG(rv, 0);
	return rv;
}

static zend_class_entry* bytearray_class_entry = NULL;
PHP_METHOD(Bytearray, __construct){
	z_bytearray_t* arr = GET_BYTEARRAY(ZEND_THIS);
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(count); //TODO allow initializing using php array?
	ZEND_PARSE_PARAMETERS_END();
	
	arr->count = count;
	arr->arr = malloc(count);
}

PHP_METHOD(Bytearray, fill){
	z_bytearray_t* arr = GET_BYTEARRAY(ZEND_THIS);
	zend_long value;
	zend_long count;

	count = arr->count;
	
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(value)
		Z_PARAM_OPTIONAL Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();
	
	memset(arr->arr, value, count);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_bytearray_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bytearray_fill, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

static const zend_function_entry bytearray_functions[] = {
	PHP_ME(Bytearray, __construct, arginfo_bytearray_construct, ZEND_ACC_PUBLIC)
	PHP_ME(Bytearray, fill, arginfo_bytearray_fill, ZEND_ACC_PUBLIC)
	PHP_FE_END
};


static zend_class_entry* mtrandom_class_entry = NULL;

PHP_METHOD(MTRandom, __construct){
	z_mtrandom_t *random = GET_NMTRANDOM(ZEND_THIS);

	zend_long seed = 0; //TODO time
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL Z_PARAM_LONG(seed)
	ZEND_PARSE_PARAMETERS_END();

	mtrandom_create(&random->native, seed & 0xffffffff);
}

PHP_METHOD(MTRandom, genRandInt){
	z_mtrandom_t *random = GET_NMTRANDOM(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();

	unsigned int num = mtrandom_getRandInt(&random->native);
	RETURN_LONG(num);
}
PHP_METHOD(MTRandom, nextInt){
	z_mtrandom_t *random = GET_NMTRANDOM(ZEND_THIS);
	zend_long bound = 0;
	int num = 0;

	if(ZEND_NUM_ARGS() <= 0){
		num = mtrandom_nextIntUnbounded(&random->native);
	}else{
		ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL Z_PARAM_LONG(bound);
		ZEND_PARSE_PARAMETERS_END();
		
		num = mtrandom_nextInt(&random->native, bound);
	}

	RETURN_LONG(num);
}
PHP_METHOD(MTRandom, nextFloat){
	z_mtrandom_t *random = GET_NMTRANDOM(ZEND_THIS);
	float num;
	num = mtrandom_nextFloat(&random->native);
	RETURN_DOUBLE(num);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_mtrandom_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mtrandom_genRandInt, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mtrandom_nextFloat, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mtrandom_nextInt, 0, 0, 0)
	ZEND_ARG_INFO(0, bound)
ZEND_END_ARG_INFO()

static const zend_function_entry mtrandom_functions[] = {
	PHP_ME(MTRandom, __construct, arginfo_mtrandom_construct, ZEND_ACC_PUBLIC)
	PHP_ME(MTRandom, genRandInt, arginfo_mtrandom_genRandInt, ZEND_ACC_PUBLIC)
	PHP_ME(MTRandom, nextInt, arginfo_mtrandom_nextInt, ZEND_ACC_PUBLIC)
	PHP_ME(MTRandom, nextFloat, arginfo_mtrandom_nextFloat, ZEND_ACC_PUBLIC)
	PHP_FE_END
};





PHP_FUNCTION(dma_malloc){
	long size;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size);
	ZEND_PARSE_PARAMETERS_END();


	void* ptr = malloc(size);
	RETURN_LONG(ptr);
}

PHP_FUNCTION(dma_free){
	long ptr;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(ptr);
	ZEND_PARSE_PARAMETERS_END();


	free((void*)ptr);
	RETURN_LONG(ptr);
}

PHP_FUNCTION(dma_get){
	long ptr;
	
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(ptr);
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(*((unsigned char*)ptr));
}

PHP_FUNCTION(dma_set){
	long ptr;
	long val;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(ptr);
		Z_PARAM_LONG(val);
	ZEND_PARSE_PARAMETERS_END();

	*((unsigned char*)ptr) = val & 0xff;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_dma_alloc, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dma_dealloc, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dma_deref, 0, 0, 1)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dma_set, 0, 0, 2)
	ZEND_ARG_INFO(0, addr)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


static const zend_function_entry ext_functions[] = {
	PHP_FE(dma_malloc, arginfo_dma_alloc)
	PHP_FE(dma_free, arginfo_dma_dealloc)
	PHP_FE(dma_get, arginfo_dma_deref)
	PHP_FE(dma_set, arginfo_dma_set)

	ZEND_FE_END
};

PHP_RINIT_FUNCTION(nativenc)
{
#if defined(ZTS) && defined(COMPILE_DL_NATIVENC)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(nativenc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "nativenc support", "enabled");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(nativenc)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "MTRandom", mtrandom_functions);
	mtrandom_class_entry = zend_register_internal_class(&ce);
	mtrandom_class_entry->create_object = mtrandom_new;

	memcpy(&mtrandom_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	mtrandom_object_handlers.offset = XtOffsetOf(z_mtrandom_t, std);
	
	
	INIT_CLASS_ENTRY(ce, "Bytearray", bytearray_functions);
	bytearray_class_entry = zend_register_internal_class(&ce);
	bytearray_class_entry->create_object = bytearray_new;

	memcpy(&bytearray_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	bytearray_object_handlers.offset = XtOffsetOf(z_bytearray_t, std);
	bytearray_object_handlers.free_obj = bytearray_free;
	bytearray_object_handlers.read_dimension = bytearray_read_dimension;
	return SUCCESS;
}

zend_module_entry nativenc_module_entry = {
	STANDARD_MODULE_HEADER,
	"nativenc",
	ext_functions, //global funcs
	PHP_MINIT(nativenc), //module init
	NULL, /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(nativenc), /* PHP_RINIT - Request initialization */
	NULL, /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(nativenc), /* PHP_MINFO - Module info */
	PHP_NATIVENC_VERSION, /* Version */
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_NATIVENC
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(nativenc)
#endif
