//
// Created by artyom on 04.10.2021.
//

#ifndef LAB2_PART1_H
#define LAB2_PART1_H

typedef std::chrono::high_resolution_clock Clock;

template<typename Function>
void init_threads(std::vector<int> &arr, const int &threads_count, Function func);

void calculate(std::vector<int> &arr);

void print_if_all_equal(std::vector<int> &arr);

void print_arr(std::vector<int> &arr);

void starter(const int &threads_count);

#endif //LAB2_PART1_H
