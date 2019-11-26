#ifndef KV_TEST_H
#define KV_TEST_H

#include "kv_api.h"

enum{
	KV_SET,
	KV_GET,
	KV_DEL,
	KV_LIST,
	KV_UNITTEST,
};

void kv_test( void *context );

#endif