#pragma once

#include "LazySequence.h"
#include <stdexcept>

template<typename T>
LazySequence<T>::LazySequence() :
    generator(new ArrayGenerator<T>(DynamicArray<T>(0))), cache(0), materializedCount(0), length(Ordinal()) {}

template<typename T>
LazySequence<T>::LazySequence(const T* items, int count) :
    generator(new ArrayGenerator<T>(DynamicArray<T>(items, count))), cache(0), materializedCount(0), length(Ordinal(count)) {}

// generator объявлен раньше и уже проверил nullptr
template<typename T>
LazySequence<T>::LazySequence(const Sequence<T>* sequence) :
    generator(new ArrayGenerator<T>(sequence)), cache(0), materializedCount(0), length(Ordinal(sequence->GetLength())) {}

template<typename T>
LazySequence<T>::LazySequence(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) :
    generator(new RuleGenerator<T>(rule, initial)), cache(0), materializedCount(0), length(Ordinal::Omega()) {}

template<typename T>
LazySequence<T>::LazySequence(IGenerator<T>* generator, const Ordinal& length) :
    generator(generator), cache(0), materializedCount(0), length(length) {
    if (generator == nullptr) throw std::invalid_argument("нулевой указатель");
}

// кэш не копируем, тк генератор с начала
template<typename T>
LazySequence<T>::LazySequence(const LazySequence<T>& other) :
    generator(other.generator->CloneFromStart()), cache(0), materializedCount(0), length(other.length) {}

template<typename T>
LazySequence<T>::~LazySequence() {
    delete generator;
}

template<typename T>
Ordinal LazySequence<T>::GetLength() const {
    return length;
}

template<typename T>
int LazySequence<T>::GetMaterializedCount() const {
    return materializedCount;
}

template<typename T>
T LazySequence<T>::Get(int index) {
    if (index < 0) throw std::out_of_range("Индекс не может быть меньше 0");
    if (Ordinal(index) >= length) throw std::out_of_range("индекс слишком большой"); //оборачиваем индекс в ординал чтобы можно было сравнить
    while (materializedCount <= index) {
        // растим вдвое, иначе n^2 копирований
        if (materializedCount == cache.GetSize()) {
            if (materializedCount == 0) cache.Resize(1);
            else cache.Resize(cache.GetSize() * 2);
        }
        cache.Set(materializedCount, generator->GetNext());
        materializedCount++;
    }
    return cache.Get(index);
}

template<typename T>
T LazySequence<T>::Get(const Ordinal& index) {
    if (index.IsFinite()) return Get(index.GetFinitePart());
    throw std::out_of_range("невозможно получить бесконечный элемент");
}

template<typename T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other) {
    if (this == &other) return *this;
    // сначала клон, потом delete, чтоб при ошибке объект остался целым
    IGenerator<T>* newGenerator = other.generator->CloneFromStart();
    delete generator;
    generator = newGenerator;
    cache = DynamicArray<T>(0);
    materializedCount = 0;
    length = other.length;
    return *this;
}

template<typename T>
T LazySequence<T>::GetFirst() {
    if (length == Ordinal()) throw std::out_of_range("последовательность пустая");
    return Get(0);
}

template<typename T>
T LazySequence<T>::GetLast() {
    if (length == Ordinal()) throw std::out_of_range("последовательность пустая");
    // у ω нет предыдущего => нет последнего
    if (!length.IsFinite()) throw std::out_of_range("у бесконечной последовательности нет последнего элемента");
    return Get(length.GetFinitePart() - 1);
}

template<typename T>
LazySequence<T>* LazySequence<T>::Concat(const LazySequence<T>* other) const {
    if (other == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
    IGenerator<T>* combined = new ConcatGenerator<T>(generator->CloneFromStart(), other->generator->CloneFromStart());
    return new LazySequence<T>(combined, length + other->length); 
}

template<typename T>
LazySequence<T>* LazySequence<T>::Append(const T& item) const {
    LazySequence<T> single(&item, 1); // &item - адрес одного элемента
    return Concat(&single); 
}

template<typename T>
LazySequence<T>* LazySequence<T>::Prepend(const T& item) const {
    LazySequence<T> single(&item, 1);
    return single.Concat(this); 
}

template<typename T>
LazySequence<T>* LazySequence<T>::GetSubSequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex < startIndex || Ordinal(endIndex) >= length) throw std::out_of_range("некорректные индексы");
    int count = endIndex - startIndex + 1;
    // {0, 1, 2, 3, 4, 5, ...}, st = 2, end = 4, count = 3 -> {2, 3, 4, 5, ...} -> {2, 3, 4};
    IGenerator<T>* tempGen = new TakeGenerator<T>(new SkipGenerator<T>(generator->CloneFromStart(), startIndex), count);
    return new LazySequence<T>(tempGen, Ordinal(count));
}

template<typename T>
LazySequence<T>* LazySequence<T>::InsertAt(const T& item, int index) const {
    LazySequence<T> single(&item, 1);
    return InsertAt(&single, index);
}

template<typename T>
LazySequence<T>* LazySequence<T>::InsertAt(const LazySequence<T>* other, int index) const {
    if (other == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
    if (index < 0 || Ordinal(index) >= length) throw std::out_of_range("некорректный индекс");
    // начало ++ other ++ хвост
    IGenerator<T>* first = new TakeGenerator<T>(generator->CloneFromStart(), index);
    IGenerator<T>* second = new SkipGenerator<T>(generator->CloneFromStart(), index);
    IGenerator<T>* tempGen = new ConcatGenerator<T>(new ConcatGenerator<T>(first, other->generator->CloneFromStart()), second);
    Ordinal rest = length.IsFinite() ? Ordinal(length.GetFinitePart() - index) : length; // у бесконечной хвост той же длины
    return new LazySequence<T>(tempGen, Ordinal(index) + other->length + rest);
}


template<typename T>
template<typename T2>
LazySequence<T2>* LazySequence<T>::Map(T2 (*function)(const T&)) const {
    if (function == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
    IGenerator<T2>* tempGen = new MapGenerator<T, T2>(generator->CloneFromStart(), function);
    return new LazySequence<T2>(tempGen, length);
}

template<typename T>
LazySequence<T>* LazySequence<T>::Where(bool (*predicate)(const T&)) const {
    if (predicate == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
    // у бесконечной кол-во подходящих не узнать, у конечной считаем точно
    Ordinal resultLength;
    if (!length.IsFinite()) {
        resultLength = Ordinal::Omega();
    }
    else {
        int count = 0;
        IGenerator<T>* counter = generator->CloneFromStart();
        while (counter->HasNext()) {
            if (predicate(counter->GetNext())) {
                count++;
            }
        }
        delete counter;
        resultLength = Ordinal(count);
    }
    IGenerator<T>* whereGen = new WhereGenerator<T>(generator->CloneFromStart(), predicate);
    return new LazySequence<T>(whereGen, resultLength);
}

template<typename T>
template<typename T2>
T2 LazySequence<T>::Reduce(T2 (*function)(const T2&, const T&), const T2& initial) {
    if (function == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
    if (!length.IsFinite()) throw std::out_of_range("нельзя свернуть бесконечную последовательность");
    T2 accumulator = initial;
    for (int i = 0; i < length.GetFinitePart(); i++) {
        accumulator = function(accumulator, Get(i));
    }
    return accumulator;
}
