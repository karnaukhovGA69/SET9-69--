#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

constexpr int kAsciiRadix = 128;

int CharAt(const std::string& value, int depth) {
  if (depth >= static_cast<int>(value.size())) {
    return -1;
  }
  return static_cast<unsigned char>(value[depth]);
}

void MsdRadixSortImpl(std::vector<std::string>* data,
                      std::vector<std::string>* buffer, int left, int right,
                      int depth) {
  if (left >= right) {
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
      MsdRadixSortImpl(data, buffer, bucket_left, bucket_right, depth + 1);
    }
  }
}

void MsdRadixSort(std::vector<std::string>* data) {
  if (data->empty()) {
    return;
  }

  std::vector<std::string> buffer(data->size());
  MsdRadixSortImpl(data, &buffer, 0, static_cast<int>(data->size()) - 1, 0);
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

  MsdRadixSort(&data);

  for (const std::string& value : data) {
    std::cout << value << '\n';
  }

  return 0;
}
