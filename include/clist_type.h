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

	NOTE: CLIST_OPTIMIZE_SPLICES et al are not undef'd - if you want
	      different storage strategies between types, you'll have to
	      manage the define'ing and undef'ing of the optimization macros.

	      The same goes for CLIST_MALLOC and CLIST_FREE.
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
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIST_META__
#define CLIST_META__
	/* the following is copied from rapidstring - thanks to John Boyer <john.boyer@tutanota.com> */
	/* include-guarded since it only needs to be evaluated once */
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
			if (CLIST_UNLIKELY(!(cond))) return ret; \
		} while (0)

#	define CLIST_ASSERT(cond) assert(cond) /* don't care about the likelihood here since it's debug-only */
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
#undef CLIST_TYPE /* make sure we use the typedef and not the type itself */

typedef struct CLIST(block) {
#ifdef CLIST_OPTIMIZE_SPLICE
	size_t elements;
#endif
	CLIST(type) data[CLIST_BLOCK_SIZE];
} CLIST(block);

typedef struct CLIST_T {
	size_t count;
	size_t block_count;
	CLIST(block) *block_list;
	CLIST(block) stack_block;
} CLIST_T;

/*
	METHODS
*/

CLIST_API int CLIST(init) (CLIST_T *list) {
	CLIST_ASSERT_RETURN(list != NULL, -EINVAL);
	memset(list, 0, sizeof(*list));

	if (NULL != ((void *)0)) {
		/* if NULL is not a zero-pointer then we should honor it. */
		/* otherwise, this will get optimized out. */
		/* this will automatically be included on segmented memory models */
		list->block_list = NULL;
	}

	return 0;
}

CLIST_API void CLIST(free) (CLIST_T *list) {
	size_t i;

	if (CLIST_UNLIKELY(list == NULL)) {
		return;
	}

	if (CLIST_UNLIKELY(list->block_count == 0)) {
		/* nothing left to do */
		return;
	}

	CLIST_ASSERT(list->block_list != NULL);

	for (i = 0; CLIST_LIKELY(i < list->block_count); i++) {
		if (CLIST_LIKELY(&list->block_list[i] != &list->stack_block)) {
			free(&list->block_list[i]);
		}
	}

	if (CLIST_UNLIKELY(&list->block_list[i] != &list->stack_block)) {
		free(list->block_list);
	}
}

CLIST_API size_t CLIST(count) (CLIST_T *list) {
	/* NOTE: doesn't do safe NULL check */
	CLIST_ASSERT(list != NULL);
	return list->count;
}

CLIST_API bool CLIST(empty) (CLIST_T *list) {
	/* NOTE: doesn't do safe NULL check */
	CLIST_ASSERT(list != NULL);
	return list->count == 0;
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
#undef CLIST_BLOCK_SIZE
#undef CLIST_API
