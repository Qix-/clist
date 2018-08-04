#ifdef NDEBUG
	/* TODO better assert instead of std assert */
#	undef NDEBUG
#	define _CLIST_NDEBUG
#endif

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

#ifdef _CLIST_NDEBUG
#	define NDEBUG 1
#	undef _CLIST_NDEBUG
#endif

void TEST_multi_init(void) {
	{
		clist_int L;

		clist_int_init(&L);
		assert(clist_int_count(&L) == 0);
		clist_int_free(&L);
	}

	{
		clist_sample L;

		clist_sample_init(&L);
		assert(clist_sample_count(&L) == 0);
		clist_sample_free(&L);
	}
}
