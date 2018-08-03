#include <assert.h>

typedef struct sample {
	int foo;
	int bar;
} sample;

#define CLIST_NAME int
#define CLIST_TYPE int
#include "clist_type.h"

#define CLIST_NAME sample
#define CLIST_TYPE struct sample
#include "clist_type.h"

void TEST_multi_init(void) {
	{
		clist_int L;

		assert(clist_int_init(&L) == 0);
		assert(clist_int_count(&L) == 0);
		clist_int_free(&L);
	}

	{
		clist_sample L;

		assert(clist_sample_init(&L) == 0);
		assert(clist_sample_count(&L) == 0);
		clist_sample_free(&L);
	}
}
