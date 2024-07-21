/* nativenc extension for PHP */

#ifndef PHP_NATIVENC_H
# define PHP_NATIVENC_H

extern zend_module_entry nativenc_module_entry;
# define phpext_nativenc_ptr &nativenc_module_entry

# define PHP_NATIVENC_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_NATIVENC)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_NATIVENC_H */
