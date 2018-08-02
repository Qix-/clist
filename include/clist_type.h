/*
	Using clist_type.h:

	       #define CLIST_TYPE struct some_type
	       #define CLIST_NAME sometype
	       #include "clist_type.h"

	       #define CLIST_TYPE struct some_other_type
	       #define CLIST_NAME someothertype
	       #include "clist_type.h"

	NOTE: CLIST_TYPE and CLIST_NAME are both undef'd at the end
	      of clist_type.h and thus will not exist after including
	      this header.
*/

/* see header comment - DO NOT PRAGMA ONCE OR INCLUDE GUARD! */

#ifdef CLIST_NAME
	/* yes, this mess is required. */
	/* the C preprocessor is nonsense... */
#	define CLIST(thing) CLIST__(thing, CLIST_NAME)
#	define CLIST__(thing, name) CLIST_(thing, name)
#	define CLIST_(thing, name) clist_ ## name ## _ ## thing
#	define CLIST_T CLIST_T__(CLIST_NAME)
#	define CLIST_T__(name) CLIST_T_(name)
#	define CLIST_T_(name) clist_##name
#else
#	define CLIST(thing) clist_##thing
#	define CLIST_T clist
#	define CLIST_NAME /* so it undef's later on */
#	define CLIST_
#	define CLIST__
#	define CLIST_T_
#	define CLIST_T__
#endif

#ifndef CLIST_TYPE
#	define CLIST_TYPE void*
#endif

#ifndef CLIST_BLOCK_SIZE
#	define CLIST_BLOCK_SIZE 64
#endif

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIST_META__
#define CLIST_META__
/* no pragma once */
/* the following is copied from rapidstring - thanks to John Boyer <john.boyer@tutanota.com> */
#	ifdef __GNUC__
#	define CLIST_GCC_VERSION \
 	       (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	else
#	define CLIST_GCC_VERSION (0)
#	endif

/* GCC version 2.96 required for branch prediction expectation. */
#	if CLIST_GCC_VERSION > 29600
#		define CLIST_EXPECT(expr, val) __builtin_expect((expr), val)
#	else
#		define CLIST_EXPECT(expr, val) (expr)
#	endif

#	define CLIST_LIKELY(expr) CLIST_EXPECT((expr), 1)
#	define CLIST_UNLIKELY(expr) CLIST_EXPECT((expr), 0)

#	ifdef __STDC_VERSION__
#		define CLIST_C99 (__STDC_VERSION__ >= 199901L)
#		define CLIST_C11 (__STDC_VERSION__ >= 201112L)
#	else
#		define CLIST_C99 (0)
#		define CLIST_C11 (0)
#	endif

#	define CLIST_ASSERT_RETURN(cond, ret) do { \
			if (!(cond)) return ret; \
		} while (0)

#	define CLIST_ASSERT(cond) assert(cond)
#endif

#ifndef CLIST_API
#	ifdef CLIST_NOINLINE
/* GCC version 3.1 required for the no inline attribute. */
#		if CLIST_GCC_VERSION > 30100
#			define CLIST_API static __attribute__((noinline))
#		elif defined(_MSC_VER)
#			define CLIST_API static __declspec(noinline)
#		else
#			define CLIST_API static
#		endif
#	elif CLIST_C99
#		define CLIST_API static inline
#	elif defined(__GNUC__)
#		define CLIST_API static __inline__
#	elif defined(_MSC_VER)
#		define CLIST_API static __forceinline
#	else
#		define CLIST_API static
#	endif
#endif

/*
	DATATYPES
*/

typedef CLIST_TYPE CLIST(type);

typedef struct CLIST(block) {
	size_t elements;
	CLIST(type) data[CLIST_BLOCK_SIZE];
	struct CLIST(block) *next;
	struct CLIST(block) *prev;
} CLIST(block);

typedef struct CLIST_T {
	size_t count;
	CLIST(block) *begin;
	CLIST(block) *end;
	CLIST(block) stack_block;
} CLIST_T;

/*
	METHODS
*/

CLIST_API int CLIST(init) (CLIST_T *list) {
	CLIST_ASSERT_RETURN(list != NULL, -EINVAL);

	list->count = 0;
	list->begin = &list->stack_block;
	list->end = list->begin;

	list->stack_block.elements = 0;
	list->stack_block.next = NULL;

	return 0;
}

CLIST_API void CLIST(free) (CLIST_T *list) {
	CLIST(block) *cur;
	CLIST(block) *next;

	if (CLIST_UNLIKELY(list == NULL)) {
		return;
	}

	cur = list->begin;

	do {
		next = cur->next;
		if (CLIST_LIKELY(cur != &list->stack_block)) {
			free(cur);
		}
	} while (CLIST_LIKELY((cur = next) != NULL));
}

CLIST_API size_t CLIST(count) (CLIST_T *list) {
	CLIST_ASSERT_RETURN(list != NULL, -EINVAL);
	return list->count;
}

#ifdef __cplusplus
}
#endif

#undef CLIST
#undef CLIST_
#undef CLIST__
#undef CLIST_T
#undef CLIST_T_
#undef CLIST_T__
#undef CLIST_NAME
#undef CLIST_TYPE
#undef CLIST_BLOCK_SIZE
#undef CLIST_API
