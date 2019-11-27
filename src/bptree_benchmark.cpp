#include "../third_party/benchmark/include/benchmark/benchmark.h"

#include "bptree.hpp"

#include <random>

uint64_t hashFunc(int a) {
  return a;
}

static void BM_TreeInsertion(benchmark::State& state) {
  BPT::BPTree<int> bpt(state.range(0), hashFunc);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(-1e6, 1e6);
  double iterations(0);

  for (auto _ : state) {
    int r = dis(gen);
    bpt.AddItem(r);
    iterations++;
  }

  state.counters["Insertion/s"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
}

// Register the function as a benchmark
BENCHMARK(BM_TreeInsertion)->RangeMultiplier(2)->Range(8, 8<<10);