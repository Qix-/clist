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

	NOTE: If you're on a system where the use of posix_memalign(3)
	      or aligned_alloc(3) cannot be used with realloc(3) then
	      define CLIST_SAFE_REALLOC. Note that this has a potential
	      performance penalty since the memory is no longer aligned
	      and the block allocations will be forced to fall back
	      to malloc(3).

	NOTE: If you're using C++, you probably also want to define
	      CLIST_REF prior to including this header.

	NOTE: clist_swap() is available if CLIST_MEMSWAP(p1,p2,n) is
	      defined beforehand. It should return 0 for success.
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
#	define CLIST_SHOULD_CLASSIFY 1
#else
#	define CLIST(thing) clist_##thing
#	define CLIST_T clist
#	define CLIST_NAME /* so it undef's later on */
#	define CLIST_
#	define CLIST__
#	define CLIST_T_
#	define CLIST_T__
#	define CLIST_SHOULD_CLASSIFY 0
#endif

#ifndef CLIST_TYPE
#	define CLIST_TYPE void*
#endif

#if defined(__cplusplus) && !defined(CLIST_NO_REF)
#	define CLIST_REF
#endif

#ifndef CLIST_REF
#	define CLIST_REF
#	define CLIST_REF_PTR *
#	define CLIST_REF_ADDROF &
#else
#	undef CLIST_REF
#	define CLIST_REF &
#	define CLIST_REF_PTR &
#	define CLIST_REF_ADDROF
#endif

#ifndef CLIST_BLOCK_SIZE
#	define CLIST_BLOCK_SIZE 512
#endif

#ifndef CLIST_BLOCK_GROWTH_RATE
	/* yes, 4 - you'll eat lots of memory but allocate less */
#	define CLIST_BLOCK_GROWTH_RATE 4
#endif

#ifndef _POSIX_SOURCE
#	define _POSIX_SOURCE
#endif

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#	define CLIST_HAS_UNISTD
#	include <unistd.h>
#endif
#if defined(_WIN32) || defined(_WIN32_)
#	define CLIST_HAS_WINDOWS
#	include <windows.h>
#endif

#ifdef __cplusplus
#	include <new>

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

	/* note that `expr` must be either 0 or 1 for this to work - the 1/0 are not
	   booleans but instead literal integral values.*/
	/* https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-_005f_005fbuiltin_005fexpect */
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

#	define CLIST_ERR ((size_t) -1)
#	define CLIST_MAX_INDEX ((size_t) -2) /* inclusive */

#	ifndef CLIST_PAGE_SIZE
#		if defined(CLIST_HAS_UNISTD) && _POSIX_VERSION >= 200112L
#			define CLIST_PAGE_SIZE(_ptr) do { \
					*(_ptr) = sysconf(_SC_PAGESIZE); \
				} while (0)
#		elif defined(CLIST_HAS_WINDOWS)
#			define CLIST_PAGE_SIZE(_ptr) do { \
					SYSTEM_INFO __cl_psz_sysInfo; \
					GetSystemInfo(&__cl_psz_sysInfo); \
					*(_ptr) = __cl_psz_sysInfo.dwPageSize; \
				} while (0)
#		else
#			define CLIST_PAGE_SIZE(_ptr) do { \
					/* sane default :P */ \
					*(_ptr) = 4096; \
				} while (0)
#		endif
#	endif
#endif

/* usage: CLIST_ALLOC(&dest_ptr, size) (it is a statement, not an expression) */
#if defined(CLIST_ALLOC) || defined(CLIST_REALLOC) || defined(CLIST_FREE)
#	ifndef CLIST_ALLOC
#		error "Either CLIST_REALLOC or CLIST_FREE were defined but CLIST_ALLOC was not. All three (or none of them) must be defined."
#	endif
#	ifndef CLIST_REALLOC
#		error "Either CLIST_ALLOC or CLIST_FREE were defined but CLIST_REALLOC was not. All three (or none of them) must be defined."
#	endif
#	ifndef CLIST_FREE
#		error "Either CLIST_ALLOC or CLIST_REALLOC were defined but CLIST_FREE was not. All three (or none of them) must be defined."
#	endif
#else
#	if !defined(CLIST_SAFE_REALLOC) && (defined(CLIST_HAS_UNISTD) && _POSIX_VERSION >= 200112L)
#		define CLIST_ALLOC(_ptrptr, _size) do { \
				size_t __cla_pgsz; \
				size_t __cla_rlsz; \
				int __cla_res; \
				CLIST_PAGE_SIZE(&__cla_pgsz); \
				__cla_rlsz = (_size); \
				__cla_rlsz += __cla_rlsz % __cla_pgsz; \
				__cla_res = posix_memalign((_ptrptr), __cla_pgsz, __cla_rlsz); \
				if (CLIST_UNLIKELY(__cla_res != 0)) { \
					(*(_ptrptr)) = NULL; \
					errno = __cla_res; \
				} \
			} while (0)
#	elif !defined(CLIST_SAFE_REALLOC) && (defined(_ISOC11_SOURCE) || CLIST_C11)
		/* yes, if you're using this library with -std=c11 on a non-posix system, you
		   get a bit of an optimization with memory aligned data */
#		define CLIST_ALLOC(_ptrptr, _size) do { \
				size_t __cla_pgsz; \
				size_t __cla_rlsz; \
				CLIST_PAGE_SIZE(&__cla_pgsz); \
				__cla_rlsz = (_size); \
				__cla_rlsz += __cla_rlsz % __cla_pgsz; \
				*(_ptrptr) = aligned_alloc(__cla_pgsz, __cla_rlsz); \
			} while (0)
#	else /* CLIST_SAFE_REALLOC */
		/* not much else we can do */
#		define CLIST_ALLOC(_ptrptr, _size) do { \
				size_t __cla_pgsz; \
				size_t __cla_rlsz; \
				CLIST_PAGE_SIZE(&__cla_pgsz); \
				__cla_rlsz = (_size); \
				__cla_rlsz += __cla_rlsz % __cla_pgsz; \
				*(_ptrptr) = malloc(__cla_rlsz); \
			} while (0)
#	endif

	/* no standardized aligned reallocs, unfortunately :/ */
	/* feel free to define your own */
#	define CLIST_REALLOC(_success_out, _ptrptr, _size) do { \
			void *__cla_np; \
			size_t __cla_pgsz; \
			size_t __cla_rlsz; \
			CLIST_PAGE_SIZE(&__cla_pgsz); \
			__cla_rlsz = (_size); \
			__cla_rlsz += __cla_rlsz % __cla_pgsz; \
			__cla_np = realloc(*(_ptrptr), __cla_rlsz); \
			if (CLIST_UNLIKELY(__cla_np == NULL)) { \
				*(_success_out) = 0; \
			} else { \
				*(_success_out) = 1; \
				*(_ptrptr) = __cla_np; \
			} \
		} while (0)

#	define CLIST_FREE(_ptr) free((_ptr))
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

#	if defined(__GNUC__)
#		define CLIST_INLINE __inline__
#	elif defined(_MSC_VER)
#		define CLIST_INLINE __forceinline
#	else
#		define CLIST_INLINE inline
#	endif

#	ifndef CLIST_MEMCPY
#		define CLIST_MEMCPY memcpy
#	endif
#	ifndef CLIST_MEMSET
#		define CLIST_MEMSET memset
#	endif
#endif

/*
	DATATYPES
*/

typedef CLIST_TYPE CLIST(type);
#define CLIST_STACK(list) ((CLIST(type)*)(&((list)->stack_block[0])))
#undef CLIST_TYPE /* make sure we use the typedef and not the type itself */

#define CLIST_BLOCK_SIZE_BYTES (CLIST_BLOCK_SIZE * sizeof(CLIST(type)))

typedef struct CLIST_T {
	size_t count;
	size_t blocks;
	CLIST(type) *block;
	char stack_block[CLIST_BLOCK_SIZE_BYTES];
} CLIST_T;

/*
	METHODS
*/

CLIST_API void CLIST(init) (CLIST_T *list) {
	CLIST_ASSERT(list != NULL);
	list->count = 0;
	list->blocks = 0;
}

CLIST_API int CLIST(init_capacity) (CLIST_T *list, size_t n_elems) {
	CLIST_ASSERT(list != NULL);

	list->count = n_elems;

	if (CLIST_LIKELY(n_elems < CLIST_BLOCK_SIZE)) {
		list->blocks = 0;
		list->count = n_elems;
		CLIST_MEMSET(list->stack_block, 0, n_elems * sizeof(CLIST(type)));
		return 0;
	}

	list->blocks = (n_elems / CLIST_BLOCK_SIZE) + 1;

	CLIST_ALLOC((void **) &list->block, list->blocks * CLIST_BLOCK_SIZE_BYTES);
	if (CLIST_UNLIKELY((void*) list->block == NULL)) {
		/* repair list */
		list->blocks = 0;
		list->block = CLIST_STACK(list);
		list->count = 0;
		return 1;
	}

	return 0;
}

CLIST_API void CLIST(free) (CLIST_T *list) {
	CLIST_ASSERT(list != NULL);
	CLIST_ASSERT(list->blocks == 0 || list->block != NULL);

	if (list->blocks > 1) {
		free(list->block);
	}
}

CLIST_API size_t CLIST(count) (const CLIST_T *list) {
	CLIST_ASSERT(list != NULL);
	return list->count;
}

CLIST_API bool CLIST(empty) (const CLIST_T *list) {
	CLIST_ASSERT(list != NULL);
	return list->count == 0;
}

CLIST_API int CLIST(expand) (CLIST_T *list, size_t block_idx) {
	CLIST_ASSERT(list != NULL);

	/* this is the case when a high capacity has been set */
	if (CLIST_UNLIKELY(block_idx < list->blocks)) {
		return 0;
	}

	if (CLIST_LIKELY(block_idx >= 2)) {
		int realloc_success;

		CLIST_ASSERT(list->block != NULL);
		CLIST_ASSERT(list->block != CLIST_STACK(list));
		CLIST_ASSERT(list->blocks >= 2);

		CLIST_REALLOC(
			&realloc_success,
			(void **) &list->block,
			(
				list->blocks * CLIST_BLOCK_GROWTH_RATE
			) * CLIST_BLOCK_SIZE_BYTES
		);

		if (CLIST_UNLIKELY(!realloc_success)) {
			/* blocks are unmodified */
			/* errno already set */
			return 1;
		}

		list->blocks *= CLIST_BLOCK_GROWTH_RATE;
	} else if (CLIST_UNLIKELY(block_idx == 0)) {
		CLIST_ASSERT(list->blocks == 0);

		list->block = CLIST_STACK(list);
		list->blocks = 1;
	} else if (CLIST_UNLIKELY(block_idx == 1)) {
		CLIST_ASSERT(list->block == CLIST_STACK(list));
		CLIST_ASSERT(list->blocks == 1);

		CLIST_ALLOC((void **) &list->block, CLIST_BLOCK_GROWTH_RATE * CLIST_BLOCK_SIZE_BYTES);

		if (CLIST_UNLIKELY(list->block == NULL)) {
			/* repair the list */
			list->block = CLIST_STACK(list);
			/* errno already set */
			return 1;
		}

		CLIST_MEMCPY(list->block, CLIST_STACK(list), CLIST_BLOCK_SIZE_BYTES);
		list->blocks = CLIST_BLOCK_GROWTH_RATE;
	}

	return 0;
}

CLIST_API CLIST(type) CLIST_REF_PTR CLIST(get) (const CLIST_T *list, size_t index) {
	CLIST_ASSERT(list != NULL);
	CLIST_ASSERT(index < list->count);
	CLIST_ASSERT(index <= CLIST_MAX_INDEX);
	CLIST_ASSERT(index != CLIST_ERR);
	return CLIST_REF_ADDROF list->block[index];
}

CLIST_API size_t CLIST(add) (CLIST_T *list, const CLIST(type) CLIST_REF val) {
	size_t idx = list->count++;
	size_t block = idx / CLIST_BLOCK_SIZE;

	CLIST_ASSERT(list != NULL);
	CLIST_ASSERT(list->count <= CLIST_MAX_INDEX);

	if (CLIST_UNLIKELY(idx > CLIST_MAX_INDEX)) {
		errno = EOVERFLOW;
		return CLIST_ERR;
	}

	if (CLIST_UNLIKELY(block == list->blocks)) {
		if (CLIST_UNLIKELY(CLIST(expand)(list, block) != 0)) {
			return CLIST_ERR;
		}
	}

#ifdef __cplusplus
	new (&list->block[idx]) CLIST(type)(val);
#else
	list->block[idx] = val;
#endif
	return idx;
}

#ifdef CLIST_MEMSWAP
CLIST_API int CLIST(swap) (CLIST_T *list_a, CLIST_T *list_b) {
	size_t tmp_size;
	CLIST(type) *tmp_ptr;

	CLIST_ASSERT(list_a != NULL);
	CLIST_ASSERT(list_b != NULL);

	if (list_a->blocks && list_b->blocks) {
		tmp_size = list_a->blocks;
		tmp_ptr = list_a->block;
		list_a->blocks = list_b->blocks;
		list_a->block = list_b->block;
		list_b->blocks = tmp_size;
		list_b->block = tmp_ptr;
	} else if (!list_a->blocks && !list_b->blocks) {
		return CLIST_MEMSWAP(&list_a->stack_block[0], &list_b->stack_block[0], sizeof(CLIST_BLOCK_SIZE_BYTES));
	} else {
		CLIST_T *stack_list;
		CLIST_T *heap_list;

		if (list_a->blocks) {
			heap_list = list_a;
			stack_list = list_b;
		} else {
			heap_list = list_b;
			stack_list = list_a;
		}

		CLIST_ASSERT(heap_list->blocks > 0);
		CLIST_ASSERT(heap_list->block != CLIST_STACK(heap_list));
		CLIST_ASSERT(stack_list->blocks == 0);
		CLIST_ASSERT(stack_list->block == CLIST_STACK(stack_list));

		stack_list->blocks = heap_list->blocks;
		stack_list->block = heap_list->block;
		heap_list->blocks = 0;
		heap_list->block = CLIST_STACK(heap_list);
		CLIST_MEMCPY(CLIST_STACK(list_a), CLIST_STACK(list_b), sizeof(CLIST_BLOCK_SIZE_BYTES));
	}

	return 0;
}
#endif

#ifdef __cplusplus
}

#	if CLIST_SHOULD_CLASSIFY
namespace clist {

struct CLIST_NAME {
	typedef CLIST(type) type;

	CLIST_INLINE CLIST_NAME() noexcept {
		CLIST(init)(&L);
	}

	CLIST_INLINE CLIST_NAME(size_t capacity) noexcept {
		int res = CLIST(init_capacity)(&L, capacity);
		(void) res;
		CLIST_ASSERT(res == 0);

		for (size_t i = 0; i < capacity; i++) {
			new (&L.block[i]) CLIST(type)();
		}
	}

	template <typename T>
	CLIST_INLINE CLIST_NAME(size_t capacity, const T &init) noexcept {
		int res = CLIST(init_capacity)(&L, capacity);
		(void) res;
		CLIST_ASSERT(res == 0);

		for (size_t i = 0; i < capacity; i++) {
			L.block[i] = init;
		}
	}

	CLIST_INLINE ~CLIST_NAME() noexcept {
		for (size_t i = 0; i < L.count; i++) {
			L.block[i].~CLIST(type)();
		}
		CLIST(free)(&L);
	}

	CLIST_INLINE size_t count() const noexcept {
		return CLIST(count)(&L);
	}

	CLIST_INLINE bool empty() const noexcept {
		return CLIST(empty)(&L);
	}

	CLIST_INLINE CLIST(type) CLIST_REF_PTR get(size_t index) const noexcept {
		return CLIST(get)(&L, index);
	}

	CLIST_INLINE size_t add(const CLIST(type) CLIST_REF val) {
		return CLIST(add)(&L, val);
	}

	template <typename... Args>
	CLIST_INLINE size_t emplace(Args const& ...args) {
		return CLIST(add)(&L, CLIST(type)(args...));
	}

	CLIST_INLINE CLIST(type) CLIST_REF_PTR operator[](size_t index) const noexcept {
		return get(index);
	}

	CLIST_INLINE CLIST(type) CLIST_REF_PTR back() const noexcept {
		return get(count() - 1);
	}

#	ifdef CLIST_MEMSWAP
	CLIST_INLINE void swap(CLIST_NAME &other) noexcept {
		int res = CLIST(swap)(&L, &other.L);
		(void) res;
		CLIST_ASSERT(res == 0);
	}
#	endif

private:
	CLIST_T L;
};

}
#	endif
#endif

#undef CLIST
#undef CLIST_
#undef CLIST__
#undef CLIST_T
#undef CLIST_T_
#undef CLIST_T__
#undef CLIST_NAME
#undef CLIST_API
#undef CLIST_HAS_UNISTD
#undef CLIST_HAS_WINDOWS
#undef CLIST_ALLOC
#undef CLIST_REALLOC
#undef CLIST_FREE
#undef CLIST_BLOCK_SIZE
#undef CLIST_BLOCK_SIZE_BYTES
#undef CLIST_BLOCK_GROWTH_RATE
#undef CLIST_REF
#undef CLIST_REF_PTR
#undef CLIST_REF_ADDROF
#undef CLIST_SHOULD_CLASSIFY
#undef CLIST_STACK
#ifdef CLIST_NO_REF
#	undef CLIST_NO_REF
#endif
