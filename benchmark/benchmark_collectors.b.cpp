#include <aalbatross/utils/streams/collectors.h>
#include <aalbatross/utils/streams/stream.h>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <memory>
#include <new>

using namespace aalbatross::utils::streams;
constexpr size_t MAX = 1000000;

static std::size_t alloc{0};
static std::size_t alloc_size{0};
static std::size_t dealloc{0};
static std::size_t dealloc_size{0};

void reset() {
  alloc = 0;
  dealloc = 0;
  alloc_size = 0;
  dealloc_size = 0;
}

void *operator new(std::size_t sz) {
  alloc += 1;
  alloc_size += sz;
  return std::malloc(sz);
}

void operator delete(void *ptr) noexcept {
  dealloc += 1;
  dealloc_size += sizeof(ptr);
  std::free(ptr);
}

class TestMemoryManager : public benchmark::MemoryManager {
 public:
  void Start() override {
    reset();
  }
  void Stop(Result *result) override {
    result->num_allocs = alloc;
    result->max_bytes_used = alloc_size;
  }
};

static void BM_StreamGroupByOnSingleColumn(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 10);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state) {
    stream.collect(Collectors::groupingBy<size_t>([](auto element) { return element; }));
  }
  state.SetItemsProcessed(MAX);
}

static void BM_StreamGroupByCascadingWithDuplicates(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 10);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::groupingBy<size_t>([](auto element) { return element; }, Collectors::averaging()));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamGroupByCascadingWithNoDuplicates(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::groupingBy<size_t>([](auto element) { return element; }, Collectors::averaging()));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamPartitionByCascadingWithDuplicates(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 10);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::partitioningBy<size_t>([](auto element) { return element % 2 == 0; }, Collectors::averaging()));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamPartitionByCascadingWithNoDuplicates(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::partitioningBy<size_t>([](auto element) { return element % 2 == 0; }, Collectors::averaging()));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamJoiningString(benchmark::State &state) {
  std::vector<std::string> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(std::to_string(i) + "times");
  }
  Stream<std::string> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::joining(",", "{", "}"));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamCounting(benchmark::State &state) {
  std::vector<std::string> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(std::to_string(i) + "times");
  }
  Stream<std::string> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::counting());
  state.SetItemsProcessed(MAX);
}

static void BM_StreamSumming(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::summingLong([](auto element) { return element; }));
  state.SetItemsProcessed(MAX);
}

static void BM_StreamAveraging(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.collect(Collectors::averaging());
  state.SetItemsProcessed(MAX);
}

static void BM_StreamToVector(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 1000);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.toVector();
  state.SetItemsProcessed(MAX);
}

static void BM_StreamToSet(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 1000);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.toSet();
  state.SetItemsProcessed(MAX);
}

static void BM_StreamToList(benchmark::State &state) {
  std::vector<size_t> data;
  for (size_t i = 0; i < MAX; i++) {
    data.emplace_back(i % 1000);
  }
  Stream<size_t> stream(data.begin(), data.end());
  for (auto _ : state)
    stream.toList();
  state.SetItemsProcessed(MAX);
}

// Register the function as a benchmark
BENCHMARK(BM_StreamGroupByOnSingleColumn);
BENCHMARK(BM_StreamGroupByCascadingWithDuplicates);
BENCHMARK(BM_StreamGroupByCascadingWithNoDuplicates);
BENCHMARK(BM_StreamPartitionByCascadingWithDuplicates);
BENCHMARK(BM_StreamPartitionByCascadingWithNoDuplicates);
BENCHMARK(BM_StreamJoiningString);
BENCHMARK(BM_StreamCounting);
BENCHMARK(BM_StreamSumming);
BENCHMARK(BM_StreamAveraging);
BENCHMARK(BM_StreamToVector);
BENCHMARK(BM_StreamToSet);
BENCHMARK(BM_StreamToList);

int main(int argc, char *argv[]) {
  std::unique_ptr<benchmark::MemoryManager> mm(new TestMemoryManager());

  benchmark::RegisterMemoryManager(mm.get());
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::RegisterMemoryManager(nullptr);
}