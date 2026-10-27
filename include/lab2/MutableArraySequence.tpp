#pragma once

#include "MutableArraySequence.h"

// map всегда собирает MutableArraySequence. какой тип у исходной, шаблонный метод не знает
template<typename T>
template<typename T2>
Sequence<T2>* Sequence<T>::Map(T2 (*f)(const T&)) const {
    MutableArraySequence<T2>* result = new MutableArraySequence<T2>();
    int len = this->GetLength();
    for (int i = 0; i < len; i++){
        result->Append(f(this->Get(i)));
    }
    return result;
}
