#pragma once

#include "Sequence.h"

template<typename T>
template<typename T2>
T2 Sequence<T>::Reduce(T2 (*f)(const T2&, const T&), const T2& initial) const {
    T2 accum = initial;
    int len = this->GetLength();
    for (int i = 0; i < len; i++){
        accum = f(accum, this->Get(i)); 
    }
    return accum;
}

template<typename T>
int Sequence<T>::GetCount() const {
    return this->GetLength();
}

template<typename T>
const T& Sequence<T>::operator[](int index) const {
    return this->Get(index);
}
