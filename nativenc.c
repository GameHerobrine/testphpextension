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

typedef struct _mtrandom_t {
	mtrandom_t native;
	zend_object std;
} z_mtrandom_t;

#define GET_NMTRANDOM(zv) ((z_mtrandom_t*)((char*)(Z_OBJ_P(zv)) - XtOffsetOf(z_mtrandom_t, std)))

zend_object *mtrandom_new(zend_class_entry *ce){
	z_mtrandom_t *mtr = zend_object_alloc(sizeof(z_mtrandom_t), ce);
	zend_object_std_init(&mtr->std, ce);
	mtr->std.handlers = &mtrandom_object_handlers;
	return &mtr->std;
}

static zend_class_entry* mtrandom_class_entry = NULL;

PHP_METHOD(MTRandom, __construct){
	z_mtrandom_t *random = GET_NMTRANDOM(ZEND_THIS);
	//zend_long factor = DEFAULT_SCALE_FACTOR;
	zend_long seed = 0; //TODO time
	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(seed)
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
