#pragma once

#include "Stream.h"
#include <stdexcept>

// --- ReadOnlyStream ---

template<typename T>
ReadOnlyStream<T>::ReadOnlyStream() : position(0), isOpen(false) {}

template<typename T>
void ReadOnlyStream<T>::Open() {
    OnOpen();
    isOpen = true;
    position = 0;
}

template<typename T>
void ReadOnlyStream<T>::Close() {
    OnClose();
    isOpen = false;
}

template<typename T>
bool ReadOnlyStream<T>::IsEndOfStream() const {
    return !HasElementAt(position);
}

template<typename T>
T ReadOnlyStream<T>::Read() {
    if (!isOpen) throw std::out_of_range("поток закрыт");
    if (IsEndOfStream()) throw std::out_of_range("достигнут конец потока");
    T item = ReadAt(position);
    position++;
    return item;
}

template<typename T>
int ReadOnlyStream<T>::GetPosition() const {
    return position;
}

template<typename T>
bool ReadOnlyStream<T>::IsCanSeek() const {
    return true;
}

template<typename T>
int ReadOnlyStream<T>::Seek(int index) {
    if (!isOpen) throw std::out_of_range("поток закрыт");
    if (!IsCanSeek()) throw std::out_of_range("поток не умеет Seek");
    if (index < 0 || !HasElementAt(index)) throw std::out_of_range("позиция недостижима");
    position = index;
    return position;
}

template<typename T>
bool ReadOnlyStream<T>::IsCanGoBack() const {
    return true;
}

// --- SequenceReadStream ---

template<typename T>
bool SequenceReadStream<T>::HasElementAt(int index) const {
    return index < source->GetLength();
}

template<typename T>
T SequenceReadStream<T>::ReadAt(int index) {
    return source->Get(index);
}

template<typename T>
SequenceReadStream<T>::SequenceReadStream(const Sequence<T>* source) : source(source){
    if (source == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
}

// --- LazyReadStream ---

template<typename T>
bool LazyReadStream<T>::HasElementAt(int index) const {
    return Ordinal(index) < source->GetLength(); // у бесконечной всегда true
}

template<typename T>
T LazyReadStream<T>::ReadAt(int index) {
    return source->Get(index);
}

template<typename T>
LazyReadStream<T>::LazyReadStream(LazySequence<T>* source) : source(source) {
    if (source == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
}

// --- WriteOnlyStream ---

template<typename T>
WriteOnlyStream<T>::WriteOnlyStream() : position(0), isOpen(false) {}

template<typename T>
void WriteOnlyStream<T>::Open() {
    OnOpen();
    isOpen = true; // позицию не сбрасываем, записанное не пропало
}

template<typename T>
void WriteOnlyStream<T>::Close() {
    OnClose();
    isOpen = false;
}

template<typename T>
int WriteOnlyStream<T>::GetPosition() const {
    return position;
}

template<typename T>
int WriteOnlyStream<T>::Write(const T& item) {
    if (!isOpen) throw std::out_of_range("поток закрыт");
    WriteItem(item);
    position++; // после WriteItem, чтоб при ошибке не сдвинуть
    return position;
}

// --- SequenceWriteStream ---

template<typename T>
void SequenceWriteStream<T>::WriteItem(const T& item) {
    // иммутабл вернул новый объект, target не наш - удаляем копию и ругаемся
    Sequence<T>* result = target->Append(item);
    if (result != target) {
        delete result;
        throw std::invalid_argument("поток записи работает только с изменяемой последовательностью");
    }
}

template<typename T>
SequenceWriteStream<T>::SequenceWriteStream(Sequence<T>* target) : target(target) {
    if (target == nullptr) throw std::invalid_argument("указатель не может быть nullptr");
}
