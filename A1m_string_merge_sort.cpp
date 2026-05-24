#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

bool StringLessOrEqual(const std::string& lhs, const std::string& rhs) {
  int index = 0;
  const int min_size = static_cast<int>(std::min(lhs.size(), rhs.size()));

  while (index < min_size && lhs[index] == rhs[index]) {
    ++index;
  }

  if (index == min_size) {
    return lhs.size() <= rhs.size();
  }

  return static_cast<unsigned char>(lhs[index]) <=
         static_cast<unsigned char>(rhs[index]);
}

void StringMergeSortImpl(std::vector<std::string>* data,
                         std::vector<std::string>* buffer, int left,
                         int right) {
  if (right - left <= 1) {
    return;
  }

  const int middle = left + (right - left) / 2;

  StringMergeSortImpl(data, buffer, left, middle);
  StringMergeSortImpl(data, buffer, middle, right);

  int left_index = left;
  int right_index = middle;
  int buffer_index = left;

  while (left_index < middle && right_index < right) {
    if (StringLessOrEqual((*data)[left_index], (*data)[right_index])) {
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

void StringMergeSort(std::vector<std::string>* data) {
  std::vector<std::string> buffer(data->size());
  StringMergeSortImpl(data, &buffer, 0, static_cast<int>(data->size()));
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

  StringMergeSort(&data);

  for (const std::string& value : data) {
    std::cout << value << '\n';
  }

  return 0;
}
