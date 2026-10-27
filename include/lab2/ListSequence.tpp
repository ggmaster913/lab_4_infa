#pragma once

#include "ListSequence.h"
#include <stdexcept>

template<typename T>
ListSequence<T>* ListSequence<T>::AppendInternal(const T& item){
    items->Append(item);
    return this;
}

template<typename T>
ListSequence<T>* ListSequence<T>::PrependInternal(const T& item){
    items->Prepend(item);
    return this;
}

template<typename T>
ListSequence<T>* ListSequence<T>::InsertAtInternal(const T& item, int index){
    items->InsertAt(item, index);
    return this;
}

template<typename T>
ListSequence<T>* ListSequence<T>::ConcatInternal(const Sequence<T>* list){
    if (list == nullptr) throw std::invalid_argument("неправильный аргумент");
    int otherSize = list->GetLength();
    for (int i = 0; i < otherSize; i++) {
        items->Append(list->Get(i));
    }
    return this;
}

template<typename T>
ListSequence<T>::ListSequence(){
    items = new LinkedList<T>();
}

template<typename T>
ListSequence<T>::ListSequence(const T* data, int count){
    items = new LinkedList<T>(data, count);
}

template<typename T>
ListSequence<T>::ListSequence(const ListSequence<T>& other){
    items = new LinkedList<T>(*(other.items));
}

template<typename T>
ListSequence<T>::ListSequence(const LinkedList<T>& other){
    items = new LinkedList<T>(other);
}

template<typename T>
ListSequence<T>::ListSequence(const DynamicArray<T>& other){
    int count = other.GetSize();
    items = new LinkedList<T>();
    for(int i = 0; i < count; i++){
        items->Append(other.Get(i));
    }
}

template<typename T>
ListSequence<T>& ListSequence<T>::operator=(const ListSequence<T>& other){
    if (this == &other) return *this;

    *items = *(other.items);

    return *this;
}

template<typename T>
ListSequence<T>::~ListSequence() {
    delete items;
}

template<typename T>
const T& ListSequence<T>::GetFirst() const {
    return items->GetFirst();
}

template<typename T>
const T& ListSequence<T>::GetLast() const {
    return items->GetLast();
}

template<typename T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

template<typename T>
ListSequence<T>* ListSequence<T>::Copy() const {
    return this->Clone();
}

template<typename T>
const T& ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template<typename T>
ListSequence<T>* ListSequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item);
}

template<typename T>
ListSequence<T>* ListSequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template<typename T>
ListSequence<T>* ListSequence<T>::InsertAt(const T& item, int index) {
    // проверяем до Instance(), иначе у иммутабл копия утечёт при исключении
    if (index < 0 || index >= items->GetLength()) throw std::out_of_range("неправильный индекс");
    return this->Instance()->InsertAtInternal(item, index);
}

template<typename T>
ListSequence<T>* ListSequence<T>::Concat(const Sequence<T>* other){
    if (other == nullptr) throw std::invalid_argument("неправильный аргумент"); // до Instance(), как в InsertAt
    return this->Instance()->ConcatInternal(other);
}

template<typename T>
ListSequence<T>* ListSequence<T>::GetSubSequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || startIndex >= items->GetLength() ||
        endIndex < 0 || endIndex >= items->GetLength() || startIndex > endIndex) {
        throw std::out_of_range("неправильный индекс");
    }

    LinkedList<T>* subPtr = items->GetSubList(startIndex, endIndex);
    ListSequence<T>* result = this->CreateInstance(*subPtr);
    delete subPtr; // CreateInstance скопировал список, временный больше не нужен
    return result;
}

template<typename T>
Sequence<T>* ListSequence<T>::Where(bool (*predicate)(const T&)) const {
    LinkedList<T> temp;
    for (int i = 0; i < items->GetLength(); i++){
        if (predicate(items->Get(i))) temp.Append(items->Get(i));
    }
    return this->CreateInstance(temp);
}
