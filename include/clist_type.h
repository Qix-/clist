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
#else
#	define CLIST(thing) clist_##thing
#	define CLIST_NAME /* so it undef's later on */
#	define CLIST_
#	define CLIST__
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

typedef struct CLIST(block_s) {
	size_t elements;
	CLIST(type) data[CLIST_BLOCK_SIZE];
	struct CLIST(block_s) *next;
} CLIST(block_s);

typedef struct CLIST(block_d) {
	size_t elements;
	CLIST(type) data[CLIST_BLOCK_SIZE];
	struct CLIST(block_d) *next;
	struct CLIST(block_d) *prev;
} CLIST(block_d);

typedef struct CLIST(s) {
	size_t count;
	CLIST(block_s) *begin;
	CLIST(block_s) stack_block;
} CLIST(s);

typedef struct CLIST(d) {
	size_t count;
	CLIST(block_d) *begin;
	CLIST(block_d) *end;
	CLIST(block_d) stack_block;
} CLIST(d);

/*
	METHODS
*/

CLIST_API int CLIST(init_s) (CLIST(s) *list) {
	CLIST_ASSERT_RETURN(list != NULL, -EINVAL);

	list->count = 0;
	list->begin = &list->stack_block;

	list->stack_block.elements = 0;
	list->stack_block.next = NULL;

	return 0;
}

CLIST_API int CLIST(init_d) (CLIST(d) *list) {
	CLIST_ASSERT_RETURN(list != NULL, -EINVAL);

	list->count = 0;
	list->begin = &list->stack_block;
	list->end = list->begin;

	list->stack_block.elements = 0;
	list->stack_block.next = NULL;

	return 0;
}

CLIST_API void CLIST(free_s) (CLIST(s) *list) {
	CLIST(block_s) *cur;
	CLIST(block_s) *next;

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

CLIST_API void CLIST(free_d) (CLIST(d) *list) {
	CLIST(block_d) *cur;
	CLIST(block_d) *next;

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

#ifdef __cplusplus
}
#endif

#undef CLIST
#undef CLIST_
#undef CLIST__
#undef CLIST_NAME
#undef CLIST_TYPE
#undef CLIST_BLOCK_SIZE
#undef CLIST_API
