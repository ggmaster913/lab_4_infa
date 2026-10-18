#ifndef SEQUENCESTATS_H
#define SEQUENCESTATS_H

#include "Sequence.h"
#include <stdexcept>

template<typename T>
struct MinMaxAvg {
    T min;
    T max;
    double avg;
};

template<typename T>
struct MinMaxAvgAccumulator {
    T min;
    T max;
    T sum;
    double count;
};

template<typename T>
MinMaxAvgAccumulator<T> CombineMinMxAvg(const MinMaxAvgAccumulator<T>& acc, const T& item) {
    if (acc.count == 0) return {item, item, item, 1};

    T newMin = (item < acc.min) ? item : acc.min;
    T newMax = (item > acc.max) ? item : acc.max;
    T newSum = acc.sum + item;

    return {newMin, newMax, newSum, acc.count + 1};
}

template<typename T>
MinMaxAvg<T> ComputerMinMaxAvg(const Sequence<T>* seq) {
    if (seq -> GetLength() == 0) {
        throw std::invalid_argument("нельзя посчитать min/max/avg для пустой последовательности");
    }
    MinMaxAvgAccumulator<T> initial = {T(), T(), T(), 0};
    MinMaxAvgAccumulator<T> result = seq->Reduce(CombineMinMxAvg<T>, initial);

    return {
        result.min,
        result.max,
        static_cast<double>(result.sum/result.count)
    };

}

#endif 