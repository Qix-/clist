#include <cassert>

#include <memswap.h>

struct foo {
	static int allocated;

	bool armed = false;

	foo() : armed(true) {
		++allocated;
	}

	foo(const foo &other) : armed(other.armed) {
		if (armed) ++foo::allocated;
	}

	foo(foo &&other) : armed(other.armed) {
		// nothing should be moved
		assert(false && "move constructor called");
	}

	~foo() {
		if (armed) {
			--allocated;
		}
	}
};

int foo::allocated = 0;

#define CLIST_TYPE foo
#define CLIST_NAME foo
#define CLIST_MEMSWAP memswap
#define CLIST_BLOCK_SIZE 2
#define CLIST_BLOCK_GROWTH_RATE 2
#include "../include/clist_type.h"

int main() {
	assert(foo::allocated == 0);

	{
		clist::foo foo_a;
		clist::foo foo_b;

		assert(foo_a.count() == 0);
		assert(foo_b.count() == 0);
		assert(foo_a.empty());
		assert(foo::allocated == 0);

		foo_a.swap(foo_b);

		assert(foo_a.count() == 0);
		assert(foo_b.count() == 0);
		assert(foo::allocated == 0);

		foo_a.emplace();
		foo_a.emplace();
		foo_a.emplace();

		assert(foo_a.count() == 3);
		assert(foo_b.count() == 0);
		assert(foo::allocated == 3);
		assert(!foo_a.empty());

		foo_b.emplace();

		assert(foo_a.count() == 3);
		assert(foo_b.count() == 1);
		assert(foo::allocated == 4);

		{
			clist::foo foo_c(5);
			assert(foo::allocated == 9);
			foo_c.add(foo());
			assert(foo::allocated == 10);
		}

		assert(foo::allocated == 4);
	}

	assert(foo::allocated == 0);

	return 0;
}
