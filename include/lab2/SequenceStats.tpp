#pragma once

#include "SequenceStats.h"
#include <stdexcept>

template<typename T>
MinMaxAvgAccumulator<T> CombineMinMaxAvg(const MinMaxAvgAccumulator<T>& acc, const T& item) {
    if (acc.count == 0) return {item, item, item, 1}; // первый эл сразу и min и max

    T newMin = (item < acc.min) ? item : acc.min;
    T newMax = (item > acc.max) ? item : acc.max;
    T newSum = acc.sum + item;

    return {newMin, newMax, newSum, acc.count + 1};
}

template<typename T>
MinMaxAvg<T> ComputeMinMaxAvg(const Sequence<T>* seq) {
    if (seq -> GetLength() == 0) {
        throw std::invalid_argument("нельзя посчитать min/max/avg для пустой последовательности");
    }
    MinMaxAvgAccumulator<T> initial = {T(), T(), T(), 0};
    MinMaxAvgAccumulator<T> result = seq->Reduce(CombineMinMaxAvg<T>, initial); // передаём саму функцию, вызывать её будет Reduce

    return {
        result.min,
        result.max,
        result.sum/result.count
    };
}
