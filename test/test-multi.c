#include "qunit.h"

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

const char *test_multi_init(void) {
	{
		clist_int L;

		q_assert(clist_int_init(&L) == 0);
		q_assert(clist_int_count(&L) == 0);
		clist_int_free(&L);
	}

	{
		clist_sample L;

		q_assert(clist_sample_init(&L) == 0);
		q_assert(clist_sample_count(&L) == 0);
		clist_sample_free(&L);
	}

	return 0;
}

q_start();
q_test(test_multi_init);
q_end();
