#include <assert.h>

#include "clist.h"

void TEST_basic_init(void) {
	clist L;

	assert(clist_init(&L) == 0);
	assert(clist_count(&L) == 0);
	assert(clist_empty(&L));

	clist_free(&L);
}
