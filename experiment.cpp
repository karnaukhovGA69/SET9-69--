#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

constexpr int kAsciiRadix = 128;
constexpr int kAlphabetSize = 74;
constexpr int kMinLength = 10;
constexpr int kMaxLength = 200;
constexpr int kMinSize = 100;
constexpr int kMaxSize = 3000;
constexpr int kStep = 100;
constexpr int kRepeats = 5;

struct SortStats {
  long long symbol_operations = 0;
};

struct ExperimentResult {
  long long total_time_us = 0;
  long long total_symbol_operations = 0;
};

enum class DatasetType {
  kRandom,
  kReversed,
  kAlmostSorted,
  kCommonPrefix,
};

std::string DatasetName(DatasetType type) {
  switch (type) {
    case DatasetType::kRandom:
      return "random";
    case DatasetType::kReversed:
      return "reversed";
    case DatasetType::kAlmostSorted:
      return "almost_sorted";
    case DatasetType::kCommonPrefix:
      return "common_prefix";
  }

  return "unknown";
}

int CharAt(const std::string& value, int depth) {
  if (depth >= static_cast<int>(value.size())) {
    return -1;
  }

  return static_cast<unsigned char>(value[depth]);
}

int CompareStringsFrom(const std::string& lhs, const std::string& rhs,
                       int start_pos, SortStats* stats) {
  int pos = start_pos;
  const int min_size = static_cast<int>(std::min(lhs.size(), rhs.size()));

  while (pos < min_size) {
    ++stats->symbol_operations;

    const unsigned char lhs_char = static_cast<unsigned char>(lhs[pos]);
    const unsigned char rhs_char = static_cast<unsigned char>(rhs[pos]);

    if (lhs_char < rhs_char) {
      return -1;
    }

    if (lhs_char > rhs_char) {
      return 1;
    }

    ++pos;
  }

  if (lhs.size() < rhs.size()) {
    return -1;
  }

  if (lhs.size() > rhs.size()) {
    return 1;
  }

  return 0;
}

int CompareStrings(const std::string& lhs, const std::string& rhs,
                   SortStats* stats) {
  return CompareStringsFrom(lhs, rhs, 0, stats);
}

int CompareChars(int lhs, int rhs, SortStats* stats) {
  ++stats->symbol_operations;

  if (lhs < rhs) {
    return -1;
  }

  if (lhs > rhs) {
    return 1;
  }

  return 0;
}

bool IsSorted(const std::vector<std::string>& data) {
  for (int i = 1; i < static_cast<int>(data.size()); ++i) {
    if (data[i] < data[i - 1]) {
      return false;
    }
  }

  return true;
}

class StringGenerator {
 public:
  explicit StringGenerator(std::uint32_t seed)
      : rng_(seed),
        alphabet_("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz"
                  "0123456789"
                  "!@#%:;^&*()-") {}

  std::vector<std::string> Generate(DatasetType type, int size) {
    switch (type) {
      case DatasetType::kRandom:
        return GenerateRandom(size);
      case DatasetType::kReversed:
        return GenerateReversed(size);
      case DatasetType::kAlmostSorted:
        return GenerateAlmostSorted(size);
      case DatasetType::kCommonPrefix:
        return GenerateCommonPrefix(size);
    }

    return {};
  }

 private:
  std::string RandomString(int length) {
    std::uniform_int_distribution<int> char_dist(
        0, static_cast<int>(alphabet_.size()) - 1);

    std::string result;
    result.reserve(length);

    for (int i = 0; i < length; ++i) {
      result.push_back(alphabet_[char_dist(rng_)]);
    }

    return result;
  }

  std::string RandomString(int min_length, int max_length) {
    std::uniform_int_distribution<int> len_dist(min_length, max_length);
    return RandomString(len_dist(rng_));
  }

  std::vector<std::string> GenerateRandom(int size) {
    std::vector<std::string> result;
    result.reserve(size);

    for (int i = 0; i < size; ++i) {
      result.push_back(RandomString(kMinLength, kMaxLength));
    }

    return result;
  }

  std::vector<std::string> GenerateReversed(int size) {
    std::vector<std::string> result = GenerateRandom(size);
    std::sort(result.begin(), result.end(), std::greater<std::string>());
    return result;
  }

  std::vector<std::string> GenerateAlmostSorted(int size) {
    std::vector<std::string> result = GenerateRandom(size);
    std::sort(result.begin(), result.end());

    const int swaps_count = std::max(1, size / 20);
    std::uniform_int_distribution<int> index_dist(0, size - 1);

    for (int i = 0; i < swaps_count; ++i) {
      std::swap(result[index_dist(rng_)], result[index_dist(rng_)]);
    }

    return result;
  }

  std::vector<std::string> GenerateCommonPrefix(int size) {
    constexpr int kPrefixCount = 8;
    constexpr int kPrefixLength = 40;

    std::vector<std::string> prefixes;
    prefixes.reserve(kPrefixCount);

    for (int i = 0; i < kPrefixCount; ++i) {
      prefixes.push_back(RandomString(kPrefixLength));
    }

    std::vector<std::string> result;
    result.reserve(size);

    for (int i = 0; i < size; ++i) {
      std::string value = prefixes[i % kPrefixCount];
      const int total_length =
          kPrefixLength + 1 + (i % (kMaxLength - kPrefixLength));
      value += RandomString(total_length - kPrefixLength);
      result.push_back(value);
    }

    std::shuffle(result.begin(), result.end(), rng_);
    return result;
  }

  std::mt19937 rng_;
  std::string alphabet_;
};

void QuickSortImpl(std::vector<std::string>* data, int left, int right,
                   SortStats* stats) {
  if (left >= right) {
    return;
  }

  int i = left;
  int j = right;
  const std::string pivot = (*data)[left + (right - left) / 2];

  while (i <= j) {
    while (CompareStrings((*data)[i], pivot, stats) < 0) {
      ++i;
    }

    while (CompareStrings(pivot, (*data)[j], stats) < 0) {
      --j;
    }

    if (i <= j) {
      std::swap((*data)[i], (*data)[j]);
      ++i;
      --j;
    }
  }

  if (left < j) {
    QuickSortImpl(data, left, j, stats);
  }

  if (i < right) {
    QuickSortImpl(data, i, right, stats);
  }
}

void QuickSort(std::vector<std::string>* data, SortStats* stats) {
  if (!data->empty()) {
    QuickSortImpl(data, 0, static_cast<int>(data->size()) - 1, stats);
  }
}

void MergeSortImpl(std::vector<std::string>* data,
                   std::vector<std::string>* buffer, int left, int right,
                   SortStats* stats) {
  if (right - left <= 1) {
    return;
  }

  const int middle = left + (right - left) / 2;
  MergeSortImpl(data, buffer, left, middle, stats);
  MergeSortImpl(data, buffer, middle, right, stats);

  int left_index = left;
  int right_index = middle;
  int buffer_index = left;

  while (left_index < middle && right_index < right) {
    if (CompareStrings((*data)[left_index], (*data)[right_index], stats) <= 0) {
      (*buffer)[buffer_index] = (*data)[left_index];
      ++left_index;
    } else {
      (*buffer)[buffer_index] = (*data)[right_index];
      ++right_index;
    }
    ++buffer_index;
  }

  while (left_index < middle) {
    (*buffer)[buffer_index] = (*data)[left_index];
    ++left_index;
    ++buffer_index;
  }

  while (right_index < right) {
    (*buffer)[buffer_index] = (*data)[right_index];
    ++right_index;
    ++buffer_index;
  }

  for (int i = left; i < right; ++i) {
    (*data)[i] = (*buffer)[i];
  }
}

void MergeSort(std::vector<std::string>* data, SortStats* stats) {
  std::vector<std::string> buffer(data->size());
  MergeSortImpl(data, &buffer, 0, static_cast<int>(data->size()), stats);
}

void StringQuickSortImpl(std::vector<std::string>* data, int left, int right,
                         int depth, SortStats* stats) {
  if (left >= right) {
    return;
  }

  const int middle = left + (right - left) / 2;
  std::swap((*data)[left], (*data)[middle]);

  int less = left;
  int greater = right;
  int current_index = left + 1;
  const int pivot = CharAt((*data)[left], depth);

  while (current_index <= greater) {
    const int current = CharAt((*data)[current_index], depth);
    const int cmp = CompareChars(current, pivot, stats);

    if (cmp < 0) {
      std::swap((*data)[less], (*data)[current_index]);
      ++less;
      ++current_index;
    } else if (cmp > 0) {
      std::swap((*data)[current_index], (*data)[greater]);
      --greater;
    } else {
      ++current_index;
    }
  }

  StringQuickSortImpl(data, left, less - 1, depth, stats);

  if (pivot >= 0) {
    StringQuickSortImpl(data, less, greater, depth + 1, stats);
  }

  StringQuickSortImpl(data, greater + 1, right, depth, stats);
}

void StringQuickSort(std::vector<std::string>* data, SortStats* stats) {
  if (!data->empty()) {
    StringQuickSortImpl(data, 0, static_cast<int>(data->size()) - 1, 0, stats);
  }
}

void MsdRadixSortImpl(std::vector<std::string>* data,
                      std::vector<std::string>* buffer, int left, int right,
                      int depth, bool use_quick_sort_cutoff,
                      SortStats* stats) {
  if (left >= right) {
    return;
  }

  if (use_quick_sort_cutoff && right - left + 1 < kAlphabetSize) {
    StringQuickSortImpl(data, left, right, depth, stats);
    return;
  }

  std::vector<int> count(kAsciiRadix + 2, 0);

  for (int i = left; i <= right; ++i) {
    ++stats->symbol_operations;
    ++count[CharAt((*data)[i], depth) + 2];
  }

  for (int i = 0; i < kAsciiRadix + 1; ++i) {
    count[i + 1] += count[i];
  }

  const std::vector<int> start = count;

  for (int i = left; i <= right; ++i) {
    const int key = CharAt((*data)[i], depth);
    (*buffer)[left + count[key + 1]] = std::move((*data)[i]);
    ++count[key + 1];
  }

  for (int i = left; i <= right; ++i) {
    (*data)[i] = std::move((*buffer)[i]);
  }

  for (int key = 0; key < kAsciiRadix; ++key) {
    const int bucket_left = left + start[key + 1];
    const int bucket_right = left + start[key + 2] - 1;

    if (bucket_left < bucket_right) {
      MsdRadixSortImpl(data, buffer, bucket_left, bucket_right, depth + 1,
                       use_quick_sort_cutoff, stats);
    }
  }
}

void MsdRadixSort(std::vector<std::string>* data, SortStats* stats) {
  if (data->empty()) {
    return;
  }

  std::vector<std::string> buffer(data->size());
  MsdRadixSortImpl(data, &buffer, 0, static_cast<int>(data->size()) - 1, 0,
                   false, stats);
}

void MsdRadixSortWithQuickSort(std::vector<std::string>* data,
                               SortStats* stats) {
  if (data->empty()) {
    return;
  }

  std::vector<std::string> buffer(data->size());
  MsdRadixSortImpl(data, &buffer, 0, static_cast<int>(data->size()) - 1, 0, true,
                   stats);
}

using SortFunction = void (*)(std::vector<std::string>*, SortStats*);

struct Algorithm {
  std::string name;
  SortFunction sort;
};

class StringSortTester {
 public:
  StringSortTester() {
    algorithms_.push_back({"quick_sort", QuickSort});
    algorithms_.push_back({"merge_sort", MergeSort});
    algorithms_.push_back({"string_quick_sort", StringQuickSort});
    algorithms_.push_back({"string_merge_sort", MergeSort});
    algorithms_.push_back({"msd_radix_sort", MsdRadixSort});
    algorithms_.push_back({"msd_radix_sort_with_quick_sort",
                           MsdRadixSortWithQuickSort});
  }

  void Run() {
    std::ofstream output("results.csv");
    if (!output.is_open()) {
      throw std::runtime_error("Cannot create results.csv");
    }

    output << "dataset,size,algorithm,avg_time_us,avg_symbol_operations\n";

    StringGenerator generator(42);
    const std::vector<DatasetType> dataset_types = {
        DatasetType::kRandom,
        DatasetType::kReversed,
        DatasetType::kAlmostSorted,
        DatasetType::kCommonPrefix,
    };

    for (DatasetType dataset_type : dataset_types) {
      for (int size = kMinSize; size <= kMaxSize; size += kStep) {
        RunDatasetCase(dataset_type, size, &generator, &output);
        std::cout << "finished: " << DatasetName(dataset_type)
                  << ", n = " << size << '\n';
      }
    }
  }

 private:
  void RunDatasetCase(DatasetType dataset_type, int size,
                      StringGenerator* generator, std::ofstream* output) {
    std::vector<ExperimentResult> results(algorithms_.size());

    for (int repeat = 0; repeat < kRepeats; ++repeat) {
      const std::vector<std::string> source =
          generator->Generate(dataset_type, size);

      for (int algorithm_index = 0;
           algorithm_index < static_cast<int>(algorithms_.size());
           ++algorithm_index) {
        std::vector<std::string> data = source;
        SortStats stats;

        const auto start = std::chrono::steady_clock::now();
        algorithms_[algorithm_index].sort(&data, &stats);
        const auto finish = std::chrono::steady_clock::now();

        if (!IsSorted(data)) {
          throw std::runtime_error("Sorting failed: " +
                                   algorithms_[algorithm_index].name);
        }

        const long long elapsed_us =
            std::chrono::duration_cast<std::chrono::microseconds>(finish -
                                                                  start)
                .count();

        results[algorithm_index].total_time_us += elapsed_us;
        results[algorithm_index].total_symbol_operations +=
            stats.symbol_operations;
      }
    }

    for (int algorithm_index = 0;
         algorithm_index < static_cast<int>(algorithms_.size());
         ++algorithm_index) {
      const double avg_time =
          static_cast<double>(results[algorithm_index].total_time_us) /
          static_cast<double>(kRepeats);
      const double avg_operations =
          static_cast<double>(
              results[algorithm_index].total_symbol_operations) /
          static_cast<double>(kRepeats);

      *output << DatasetName(dataset_type) << ',' << size << ','
              << algorithms_[algorithm_index].name << ',' << avg_time << ','
              << avg_operations << '\n';
    }
  }

  std::vector<Algorithm> algorithms_;
};

int main() {
  try {
    StringSortTester tester;
    tester.Run();
    std::cout << "results.csv created\n";
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
