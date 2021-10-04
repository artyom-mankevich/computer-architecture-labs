//
// Created by artyom on 04.10.2021.
//

#ifndef LAB2_PART1_H
#define LAB2_PART1_H

typedef std::chrono::high_resolution_clock Clock;

template<typename Function>
void init_threads(int *arr, const int &threads_count, Function func);

void calculate(int *arr);

void print_if_all_equal(const int *arr);

void print_arr(int *arr);

void starter(const int &threads_count);

#endif //LAB2_PART1_H
