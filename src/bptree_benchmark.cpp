#include "../third_party/benchmark/include/benchmark/benchmark.h"

#include "bptree.hpp"

#include <iostream>
#include <memory>
#include <random>
#include <vector>

uint64_t hashFunc(int a) { return a; }

std::vector<int> dataset(1e5);
std::map<int, std::shared_ptr<BPT::BPTree<int>>> samples;
std::vector<int> degrees{8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
bool datasetInitiated(false);

static void InitiateDatasetAndSamples() {
  if (!datasetInitiated) {
    int maxItem = 1e6;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-maxItem / 2, maxItem / 2);

    std::generate(dataset.begin(), dataset.end(), [&] { return dis(gen); });

    // Samples
    for (int degree : degrees) {
      std::cout << "Generating BPT deg " << degree << std::endl;
      samples[degree] = std::make_shared<BPT::BPTree<int>>(degree, hashFunc);
      for (int i(0); i < maxItem; i++) {
        samples[degree]->AddItem(dataset[i]);
      }
    }

    datasetInitiated = true;
  }
}

static void BM_TreeInsertion(benchmark::State& state) {
  InitiateDatasetAndSamples();
  BPT::BPTree<int> bpt(state.range(0), hashFunc);
  double iterations(0);

  for (auto _ : state) {
    bpt.AddItem(dataset[iterations]);
    iterations++;
  }

  state.counters["Insertion/s"] =
      benchmark::Counter(iterations, benchmark::Counter::kIsRate);
}

static void BM_TreeSearch(benchmark::State& state) {
  InitiateDatasetAndSamples();
  BPT::BPTree<int> bpt(state.range(0), hashFunc);
  bpt.DeepCopyFrom(samples[state.range(0)].get());

  double iterations(0);
  double missed(0);

  for (auto _ : state) {
    try {
      iterations++;
      bpt.SearchItem(dataset[iterations]);
    } catch (std::out_of_range& _) {
      missed++;
    }
  }

  state.counters["Search/s"] =
      benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  state.counters["Missed %"] = benchmark::Counter(100 * missed / iterations);
}

static void BM_TreeDelete(benchmark::State& state) {
  InitiateDatasetAndSamples();
  BPT::BPTree<int> bpt(state.range(0), hashFunc);
  bpt.DeepCopyFrom(samples[state.range(0)].get());

  double iterations(0);

  for (auto _ : state) {
    iterations++;
    bpt.DeleteItem(dataset[iterations]);
  }

  state.counters["Deletion/s"] =
      benchmark::Counter(iterations, benchmark::Counter::kIsRate);
}

// Register the function as a benchmark
BENCHMARK(BM_TreeInsertion)->RangeMultiplier(2)->Range(8, 8 << 10);

BENCHMARK(BM_TreeSearch)->RangeMultiplier(2)->Range(8, 8 << 10);

BENCHMARK(BM_TreeDelete)->RangeMultiplier(2)->Range(8, 8 << 10);