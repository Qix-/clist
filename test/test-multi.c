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
	clist_int_d double_int_list;
	clist_int_s single_int_list;

	q_assert(clist_int_init_s(&single_int_list) == 0);
	clist_int_free_s(&single_int_list);

	q_assert(clist_int_init_d(&double_int_list) == 0);
	clist_int_free_d(&double_int_list);

	clist_sample_d double_sample_list;
	clist_sample_s single_sample_list;

	q_assert(clist_sample_init_s(&single_sample_list) == 0);
	clist_sample_free_s(&single_sample_list);

	q_assert(clist_sample_init_d(&double_sample_list) == 0);
	clist_sample_free_d(&double_sample_list);

	return 0;
}

q_start();
q_test(test_multi_init);
q_end();
