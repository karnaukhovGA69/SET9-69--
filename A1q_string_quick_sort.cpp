#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

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

void StringQuickSort(std::vector<std::string>* data) {
  if (!data->empty()) {
    StringQuickSortImpl(data, 0, static_cast<int>(data->size()) - 1, 0);
  }
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

  StringQuickSort(&data);

  for (const std::string& value : data) {
    std::cout << value << '\n';
  }

  return 0;
}
