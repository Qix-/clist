#include <assert.h>

#include "clist.h"

void TEST_basic_init(void) {
	clist L;

	assert(clist_init(&L) == 0);
	assert(clist_count(&L) == 0);
	assert(clist_empty(&L));

	clist_free(&L);
}

void TEST_FAIL_get_on_empty_list(void) {
	clist L;
	assert(clist_init(&L) == 0);
	clist_get(&L, 0); /* out of bounds */
}

void TEST_add(void) {
	clist L;

	assert(clist_init(&L) == 0);

	assert(clist_add(&L, (void *) 1) == 0);
	assert(clist_add(&L, (void *) 2) == 1);
	assert(clist_add(&L, (void *) 3) == 2);
	assert(clist_add(&L, (void *) 4) == 3);
	assert(clist_add(&L, (void *) 5) == 4);

	assert(clist_count(&L) == 5);
	assert(!clist_empty(&L));

	assert(*clist_get(&L, 0) == (void *) 1);
	assert(*clist_get(&L, 1) == (void *) 2);
	assert(*clist_get(&L, 2) == (void *) 3);
	assert(*clist_get(&L, 3) == (void *) 4);
	assert(*clist_get(&L, 4) == (void *) 5);

	clist_free(&L);
}

void TEST_add_many(void) {
	size_t i;
	clist L;

	assert(clist_init(&L) == 0);
	assert(clist_empty(&L));

	for (i = 0; i < 65536; i++) {
		assert(clist_add(&L, (void *) i) == i);
	}

	assert(!clist_empty(&L));
	assert(clist_count(&L) == 65536);

	clist_free(&L);
}
