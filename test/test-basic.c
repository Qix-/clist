#include "qunit.h"

#include "clist.h"

const char *test_basic_init(void) {
	clist_d double_list;
	clist_s single_list;

	q_assert(clist_init_s(&single_list) == 0);
	clist_free_s(&single_list);

	q_assert(clist_init_d(&double_list) == 0);
	clist_free_d(&double_list);

	return 0;
}

q_start();
q_test(test_basic_init);
q_end();
