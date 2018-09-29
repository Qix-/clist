# clist

The missing C lists library.

Faster than GNU's `std::vector` in most cases.

## Building

```console
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release                     # release mode
$ cmake .. -DBUILD_TESTING=On -DCMAKE_BUILD_TYPE=Debug    # debug mode (tests only work on unix right now)
$ cmake --build .
```

## Usage

No linking or compilation necessary - Clist is header-only.

Clist is not (currently) thread safe. Feel free to submit a PR.

Any calls for out of bounds will result in an abort. This is normally considered
bad API design but bounds should be checked by the user. This frees up the API
to remain simple and clean.

`clist.h` is include guarded and initiailizes the API with a `void*` type
and an un-namespaced API.

```c
#include "clist.h"

clist L;

/* Initialize a list */
clist_init(&L);

/* Free up a list */
clist_free(&L);

/* Count the elements in a list */
size_t count = clist_count(&L);

/* Check if a list is empty
   (equivalent to (clist_count(&L) == 0)) */
_Bool is_empty = clist_empty(&L);

/* Get an element
   NOTE: aborts on OOB (make sure you're checking bounds)
   NOTE: return type is void* by default - see notes on clist_type.h below */
void *elem = clist_get(&L, idx);

/* Add an element
   NOTE: returns CLIST_ERR if the addition fails - errno holds the error
         otherwise, on success, returns the new index of the added element */
size_t idx = clist_add(&L, some_ptr);
if (idx == CLIST_ERR) {
    abort("error adding a value");
}
```

`clist_type.h` is **not** include guarded and initializes a new set of APIs
with a new type by defining `CLIST_TYPE` with the type you want Clist to hold,
and `CLIST_NAME` for the namespace name.

Both are optional - `CLIST_NAME` defaults to un-namespaced (meaning `clist_add()` directly)
and `CLIST_TYPE` defaults to `void*`. You can define either, both, or neither.

Both definitions are 'consumed' (`#undef`'d) at the end of `clist_type.h`, making it easier
to create several typed clist namespaces.

```c
#define CLIST_TYPE int
#define CLIST_NAME myint
#include "clist_type.h"

#define CLIST_TYPE const char *
#define CLIST_NAME mystr
#include "clist_type.h"

clist_myint_add(&int_list, 10);
clist_mystr_add(&str_list, "Hello");
```

# License
clist is Copyright &copy; 2018 by Josh Junon and released under the MIT License.
