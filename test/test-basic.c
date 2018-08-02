#include "qunit.h"

#include "clist.h"

const char *test_basic_init(void) {
	clist L;

	q_assert(clist_init(&L) == 0);
	q_assert(clist_count(&L) == 0);
	clist_free(&L);

	return 0;
}

q_start();
q_test(test_basic_init);
q_end();
