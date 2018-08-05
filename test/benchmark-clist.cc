#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include <vector>

#include "benchmark/benchmark.h"

#define _CLIST_BLOCK_SIZE 512
#define CLIST_BLOCK_SIZE _CLIST_BLOCK_SIZE
#include "clist.h"
#define CLIST_BLOCK_SIZE _CLIST_BLOCK_SIZE

#define BM(name, body) static void BM_##name(benchmark::State& state) { \
		clist L; \
		(void) L; \
		std::vector<void*> foo; \
		(void) foo; \
		for (auto _ : state) { \
			body \
		} \
		benchmark::DoNotOptimize(L); \
		benchmark::DoNotOptimize(foo); \
	} \
	BENCHMARK(BM_##name)

#define BM_Init(name, init, body) static void BM_##name(benchmark::State& state) { \
		clist L; \
		(void) L; \
		std::vector<void*> foo; \
		(void) foo; \
		init \
		for (auto _ : state) { \
			body \
		} \
		benchmark::DoNotOptimize(L); \
		benchmark::DoNotOptimize(foo); \
	} \
	BENCHMARK(BM_##name)

#define BM_InitD(name, init, body, dtor) static void BM_##name(benchmark::State& state) { \
		clist L; \
		(void) L; \
		std::vector<void*> foo; \
		(void) foo; \
		init \
		for (auto _ : state) { \
			body \
		} \
		benchmark::DoNotOptimize(L); \
		benchmark::DoNotOptimize(foo); \
		dtor \
	} \
	BENCHMARK(BM_##name)

BM(Clist_Initialize, {
	clist_init(&L);
	clist_free(&L);
	benchmark::ClobberMemory();
});

BM(CPPVector_Initialize, {
	foo.reserve(CLIST_BLOCK_SIZE);
	benchmark::ClobberMemory();
});

BM(Clist_Add1, {
	clist_init(&L);
	benchmark::DoNotOptimize(clist_add(&L, (void *) 42));
	benchmark::ClobberMemory();
	clist_free(&L);
});

BM(CPPVector_Add1, {
	foo.reserve(CLIST_BLOCK_SIZE);
	benchmark::DoNotOptimize((foo.push_back((void*) 42),0));
	benchmark::ClobberMemory();
});

BM(Clist_Add1024, {
	clist_init(&L);

	for (size_t i = 0; i < 1024; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
		benchmark::ClobberMemory();
	}
	benchmark::ClobberMemory();
	clist_free(&L);
});

BM(CPPVector_Add1024, {
	foo.reserve(CLIST_BLOCK_SIZE);

	for (size_t i = 0; i < 1024; i++) {
		foo.push_back((void*) i);
		benchmark::ClobberMemory();
	}
});

BM(Clist_Add65k, {
	clist_init(&L);

	for (size_t i = 0; i < 65536; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
		benchmark::ClobberMemory();
	}

	clist_free(&L);
})->Unit(benchmark::kMicrosecond);

BM(CPPVector_Add65k, {
	foo.reserve(CLIST_BLOCK_SIZE);

	for (size_t i = 0; i < 65536; i++) {
		foo.push_back((void*) i);
		benchmark::ClobberMemory();
	}
})->Unit(benchmark::kMicrosecond);

BM(Clist_Add1mil, {
	clist_init(&L);

	for (size_t i = 0; i < 1000000; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
		benchmark::ClobberMemory();
	}

	clist_free(&L);
})->Unit(benchmark::kMicrosecond);

BM(CPPVector_Add1mil, {
	foo.reserve(CLIST_BLOCK_SIZE);

	for (size_t i = 0; i < 1000000; i++) {
		foo.push_back((void*) i);
		benchmark::ClobberMemory();
	}
})->Unit(benchmark::kMicrosecond);

BM(Clist_Add100mil, {
	clist_init(&L);

	for (size_t i = 0; i < 100000000; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
	}
	benchmark::ClobberMemory();

	clist_free(&L);
})->Unit(benchmark::kMicrosecond);

BM(CPPVector_Add100mil, {
	foo.reserve(CLIST_BLOCK_SIZE);

	for (size_t i = 0; i < 100000000; i++) {
		foo.push_back((void*) i);
	}
	benchmark::ClobberMemory();
})->Unit(benchmark::kMicrosecond);

BM(CList_Count, {
	clist_init(&L);
	benchmark::DoNotOptimize(clist_count(&L) == 0);
	clist_free(&L);
});

BM(CPPVector_Count, {
	foo.reserve(CLIST_BLOCK_SIZE);
	benchmark::DoNotOptimize(foo.size() != 0);
});

BM(Clist_Get1, {
	clist_init(&L);

	if (clist_add(&L, (void*) 42) == CLIST_ERR) {
		state.SkipWithError("list add failed (check errno)");
	}
	benchmark::ClobberMemory();

	benchmark::DoNotOptimize(clist_get(&L, 0) != (void *) 42);

	clist_free(&L);
});

BM(CPPVector_Get1, {
	foo.reserve(CLIST_BLOCK_SIZE);

	foo.push_back((void*) 42);
	benchmark::ClobberMemory();

	benchmark::DoNotOptimize(foo[0] != (void*) 42);
});

BM_InitD(CList_Get1024, {
	clist_init(&L);

	for (size_t i = 0; i < 1024; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
	}
}, {
	for (size_t i = 0; i < 1024; i++) {
		if (*clist_get(&L, i) != (void *) i) {
			state.SkipWithError("invalid get");
		}
	}
}, {
	clist_free(&L);
});

BM_Init(CPPVector_Get1024, {
	for (size_t i = 0; i < 1024; i++) {
		foo.push_back((void *) i);
	}
}, {
	for (size_t i = 0; i < 1024; i++) {
		if (foo[i] != (void *) i) {
			state.SkipWithError("invalid get");
		}
	}
});

BM_InitD(CList_Get1mil, {
	clist_init(&L);

	for (size_t i = 0; i < 1000000; i++) {
		if (clist_add(&L, (void *) i) == CLIST_ERR) {
			state.SkipWithError("list add failed (check errno)");
		}
	}
}, {
	for (size_t i = 0; i < 1000000; i++) {
		if (*clist_get(&L, i) != (void *) i) {
			state.SkipWithError("invalid get");
		}
	}
}, {
	clist_free(&L);
});

BM_Init(CPPVector_Get1mil, {
	for (size_t i = 0; i < 1000000; i++) {
		foo.push_back((void *) i);
	}
}, {
	for (size_t i = 0; i < 1000000; i++) {
		if (foo[i] != (void *) i) {
			state.SkipWithError("invalid get");
		}
	}
});

BENCHMARK_MAIN();
