#pragma once

#include "LazySequence.h"

// проверка равенства
template<typename T>
bool AreEqual(LazySequence<T>* first, LazySequence<T>* second, int count);

// (a + b) + c = a + (b + с)
template<typename T>
bool CheckAssociativity(LazySequence<T>* a, LazySequence<T>* b, LazySequence<T>* c, int count);

// пустой + a = a + пустой = a
template<typename T>
bool CheckIdentity(LazySequence<T>* a, int count);

// a + b = b + a
template<typename T>
bool CheckCommutativity(LazySequence<T>* a, LazySequence<T>* b, int count);

// длина (a + b) = длина(a) + длина(b)
template<typename T>
bool CheckLengthAddition(LazySequence<T>* a, LazySequence<T>* b);

#include "LazyAlgebra.tpp"
