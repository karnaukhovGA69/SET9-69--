#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

constexpr int kAsciiRadix = 128;
constexpr int kAlphabetSize = 74;

int CharAt(const std::string& value, int depth) {
  if (depth >= static_cast<int>(value.size())) {
    return -1;
  }
  return static_cast<unsigned char>(value[depth]);
}

void StringQuickSortImpl(std::vector<std::string>* data, int left, int right,
                         int depth) {
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

    if (current < pivot) {
      std::swap((*data)[less], (*data)[current_index]);
      ++less;
      ++current_index;
    } else if (current > pivot) {
      std::swap((*data)[current_index], (*data)[greater]);
      --greater;
    } else {
      ++current_index;
    }
  }

  StringQuickSortImpl(data, left, less - 1, depth);

  if (pivot >= 0) {
    StringQuickSortImpl(data, less, greater, depth + 1);
  }

  StringQuickSortImpl(data, greater + 1, right, depth);
}

void MsdRadixSortWithQuickSortImpl(std::vector<std::string>* data,
                                   std::vector<std::string>* buffer, int left,
                                   int right, int depth) {
  if (left >= right) {
    return;
  }

  if (right - left + 1 < kAlphabetSize) {
    StringQuickSortImpl(data, left, right, depth);
    return;
  }

  std::vector<int> count(kAsciiRadix + 2, 0);

  for (int i = left; i <= right; ++i) {
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
      MsdRadixSortWithQuickSortImpl(data, buffer, bucket_left, bucket_right,
                                    depth + 1);
    }
  }
}

void MsdRadixSortWithQuickSort(std::vector<std::string>* data) {
  if (data->empty()) {
    return;
  }

  std::vector<std::string> buffer(data->size());
  MsdRadixSortWithQuickSortImpl(data, &buffer, 0,
                                static_cast<int>(data->size()) - 1, 0);
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = 0;
  std::cin >> n;

  std::vector<std::string> data(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> data[i];
  }

  MsdRadixSortWithQuickSort(&data);

  for (const std::string& value : data) {
    std::cout << value << '\n';
  }

  return 0;
}
