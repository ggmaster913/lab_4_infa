#pragma once

#include "Generator.h"
#include "Ordinal.h"
#include "OperationGenerators.h"
#include "lab2/DynamicArray.h"
#include "lab2/Sequence.h"

template<typename T>
class LazySequence {
    IGenerator<T>* generator;
    DynamicArray<T> cache; //посчитанные эл
    int materializedCount; //сколько ячеек кэша реально заполнено
    Ordinal length;
public:
    LazySequence();
    LazySequence(const T* items, int count);
    explicit LazySequence(const Sequence<T>* sequence);
    LazySequence(T (*rule)(const Sequence<T>*), const Sequence<T>* initial);
    // генератор теперь наш, удалим в деструкторе
    LazySequence(IGenerator<T>* generator, const Ordinal& length);
    LazySequence(const LazySequence<T>& other);
    ~LazySequence();

    Ordinal GetLength() const;
    int GetMaterializedCount() const;

    T Get(int index);
    // к ω и дальше нельзя, перечисление туда не дойдёт
    T Get(const Ordinal& index);

    // a = b = c
    LazySequence<T>& operator=(const LazySequence<T>& other);

    T GetFirst();
    T GetLast();

    LazySequence<T>* Concat(const LazySequence<T>* other) const;
    LazySequence<T>* Append(const T& item) const;
    LazySequence<T>* Prepend(const T& item) const;
    LazySequence<T>* GetSubSequence(int startIndex, int endIndex) const;
    LazySequence<T>* InsertAt(const T& item, int index) const;
    LazySequence<T>* InsertAt(const LazySequence<T>* other, int index) const;

    template<typename T2>
    LazySequence<T2>* Map(T2 (*function)(const T&)) const;

    LazySequence<T>* Where(bool (*predicate)(const T&)) const;

    template<typename T2>
    T2 Reduce(T2 (*function)(const T2&, const T&), const T2& initial);
};

#include "LazySequence.tpp"
