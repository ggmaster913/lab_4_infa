#pragma once

#include "OperationGenerators.h"
#include <stdexcept>

// --- ConcatGenerator ---

template<typename T>
ConcatGenerator<T>::ConcatGenerator(IGenerator<T>* first, IGenerator<T>* second) : first(first), second(second) {
    // генераторы уже наши, удаляем чтоб не утекли
    if (first == nullptr || second == nullptr) {
        delete this->first;
        delete this->second;
        throw std::invalid_argument("часть конкатенации не может быть nullptr");
    }
}

template<typename T>
ConcatGenerator<T>::~ConcatGenerator() {
    delete first;
    delete second;
}

template<typename T>
bool ConcatGenerator<T>::HasNext() const {
    return first->HasNext() || second->HasNext();
}

template<typename T>
T ConcatGenerator<T>::GetNext() {
    if (first->HasNext()) return first->GetNext();
    return second->GetNext();
}

template<typename T>
IGenerator<T>* ConcatGenerator<T>::CloneFromStart() const {
    return new ConcatGenerator<T>(first->CloneFromStart(), second->CloneFromStart());
}

// --- SkipGenerator ---

template<typename T>
void SkipGenerator<T>::SkipIfNeeded() const {
    if (skipped == true) return;
    for (int i = 0; i < skipCount; i++) {
        if (source->HasNext()) source->GetNext();
    }
    skipped = true;
}

template<typename T>
SkipGenerator<T>::SkipGenerator(IGenerator<T>* source, int skipCount) : source(source), skipCount(skipCount), skipped(false) {
    if (source == nullptr) throw std::invalid_argument("указатель не может быть пустым");
    if (skipCount < 0) {
        delete source;
        throw std::invalid_argument("skipCount не может быть меньше нуля");
    }
}

template<typename T>
SkipGenerator<T>::~SkipGenerator() {
    delete source;
}

template<typename T>
bool SkipGenerator<T>::HasNext() const {
    SkipIfNeeded();
    return source->HasNext();
}

template<typename T>
T SkipGenerator<T>::GetNext() {
    SkipIfNeeded();
    return source->GetNext();
}

template<typename T>
IGenerator<T>* SkipGenerator<T>::CloneFromStart() const {
    return new SkipGenerator<T>(source->CloneFromStart(), skipCount);
}

// --- TakeGenerator ---

template<typename T>
TakeGenerator<T>::TakeGenerator(IGenerator<T>* source, int takeCount) : source(source), takeCount(takeCount), given(0) {
    if (source == nullptr) throw std::invalid_argument("указатель не может быть равен нулю");
    if (takeCount < 0) {
        delete source;
        throw std::invalid_argument("takeCount не может быть меньше нуля");
    }
}

template<typename T>
TakeGenerator<T>::~TakeGenerator() {
    delete source;
}

template<typename T>
bool TakeGenerator<T>::HasNext() const {
    return given < takeCount && source->HasNext();
}

template<typename T>
T TakeGenerator<T>::GetNext() {
    if (!this->HasNext()) throw std::out_of_range("нет следующего элемента");
    given++;
    return source->GetNext();
}

template<typename T>
IGenerator<T>* TakeGenerator<T>::CloneFromStart() const {
    return new TakeGenerator<T>(source->CloneFromStart(), takeCount);
}

// --- MapGenerator ---

template<typename T, typename T2>
MapGenerator<T, T2>::MapGenerator(IGenerator<T>* source, T2 (*function)(const T&)) : source(source), function(function) {
    if (source == nullptr) throw std::invalid_argument("указатель не может быть равен nullptr");
    if (function == nullptr) {
        delete source;
        throw std::invalid_argument("указатель на функцию не может быть nullptr");
    }
}

template<typename T, typename T2>
MapGenerator<T, T2>::~MapGenerator() {
    delete source;
}

template<typename T, typename T2>
bool MapGenerator<T, T2>::HasNext() const {
    return source->HasNext();
}

template<typename T, typename T2>
T2 MapGenerator<T, T2>::GetNext() {
    return function(source->GetNext());
}

template<typename T, typename T2>
IGenerator<T2>* MapGenerator<T, T2>::CloneFromStart() const {
    return new MapGenerator<T, T2>(source->CloneFromStart(), function);
}

// --- WhereGenerator ---

template<typename T>
void WhereGenerator<T>::FindNext() const {
    if (buffer.HasValue()) return;
    while (source->HasNext()) {
        T item = source->GetNext();
        if (predicate(item)) {
            buffer = Option<T>(item);
            return;
        }
    }
}

template<typename T>
WhereGenerator<T>::WhereGenerator(IGenerator<T>* source, bool (*predicate)(const T&)) : source(source), predicate(predicate), buffer() {
    if (source == nullptr) throw std::invalid_argument("указатель не может быть равен nullptr");
    if (predicate == nullptr) {
        delete source;
        throw std::invalid_argument("указатель на функцию не может быть nullptr");
    }
}

template<typename T>
WhereGenerator<T>::~WhereGenerator() {
    delete source;
}

template<typename T>
bool WhereGenerator<T>::HasNext() const {
    FindNext();
    return buffer.HasValue();
}

template<typename T>
T WhereGenerator<T>::GetNext() {
    FindNext();
    if (!buffer.HasValue()) throw std::out_of_range("элементы кончились");
    T result = buffer.GetValue();
    buffer = Option<T>(); // чтоб в след раз искать дальше
    return result;
}

template<typename T>
IGenerator<T>* WhereGenerator<T>::CloneFromStart() const {
    return new WhereGenerator<T>(source->CloneFromStart(), predicate);
}
