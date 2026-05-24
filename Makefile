CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

all: A1m A1q A1r A1rq experiment

A1m: A1m_string_merge_sort.cpp
	$(CXX) $(CXXFLAGS) A1m_string_merge_sort.cpp -o A1m

A1q: A1q_string_quick_sort.cpp
	$(CXX) $(CXXFLAGS) A1q_string_quick_sort.cpp -o A1q

A1r: A1r_msd_radix_sort.cpp
	$(CXX) $(CXXFLAGS) A1r_msd_radix_sort.cpp -o A1r

A1rq: A1rq_msd_radix_quick_sort.cpp
	$(CXX) $(CXXFLAGS) A1rq_msd_radix_quick_sort.cpp -o A1rq

experiment: experiment.cpp
	$(CXX) $(CXXFLAGS) experiment.cpp -o experiment

plots:
	python3 plot.py

clean:
	rm -f A1m A1q A1r A1rq experiment results.csv *.png
