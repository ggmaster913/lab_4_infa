#pragma once

#include "Option.h"
#include <stdexcept>

template<typename T>
Option<T>::Option() : hasValue(false), value() {}

template<typename T>
Option<T>::Option(const T& value) : hasValue(true), value(value) {}

template<typename T>
bool Option<T>::HasValue() const {
    return hasValue;
}

template<typename T>
const T& Option<T>::GetValue() const {
    if (!hasValue) throw std::out_of_range("Элемента нет");
    return value;
}
