#pragma once

#include "ArraySequence.h"
#include <stdexcept>

template<typename T>
ArraySequence<T>* ArraySequence<T>::AppendInternal(const T& item ) {
    items->Resize(items->GetSize() + 1);
    items->Set(items->GetSize() - 1, item);
    return this;
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::PrependInternal(const T& item ){
    int oldSize = items->GetSize();
    items->Resize(oldSize + 1);
    for (int i = oldSize; i > 0; i--){ // сдвигаем с конца, иначе затрём следующий эл
        items->Set(i, items->Get(i - 1));
    }
    items->Set(0, item);
    return this;
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::InsertAtInternal(const T& item, int index){
    if (index < 0 || index >= items->GetSize()) throw std::out_of_range("неправильный индекс");
    int oldSize = items->GetSize();
    items->Resize(oldSize + 1);
    for (int i = oldSize; i > index; i--){
        items->Set(i, items->Get(i - 1));
    }
    items->Set(index, item);
    return this;
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::ConcatInternal(const Sequence<T>* list){
    if (list == nullptr) throw std::invalid_argument("неправильный аргумент");
    int oldSize = items->GetSize();
    int otherSize = list->GetLength(); // запоминаем до Resize, иначе при seq.Concat(&seq) длина менялась бы по ходу
    items->Resize(oldSize + otherSize);
    for (int i = 0; i < otherSize; i++){
        items->Set(oldSize + i, list->Get(i));
    }
    return this;
}

template<typename T>
ArraySequence<T>::ArraySequence(){
    items = new DynamicArray<T>(0);
}

template<typename T>
ArraySequence<T>::ArraySequence(const T* data, int count){
    items = new DynamicArray<T>(data, count);
}

template<typename T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) {
    items = new DynamicArray<T>(*(other.items));
}

template<typename T>
ArraySequence<T>::ArraySequence(const DynamicArray<T>& array) {
    items = new DynamicArray<T>(array);
}

template<typename T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list){
    int count = list.GetLength();
    items = new DynamicArray<T>(count);
    for (int i = 0; i < count; i++){
        items->Set(i, list.Get(i));
    }
}

template<typename T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other){
    if (this == &other) return *this;

    *items = *(other.items);// уже готовый оператор= из дайнамикэррэй

    return *this;
}

template<typename T>
ArraySequence<T>::~ArraySequence(){
    delete items;
}

template<typename T>
const T& ArraySequence<T>::GetFirst() const {
    return items->Get(0);
}

template<typename T>
const T& ArraySequence<T>::GetLast() const {
    return items->Get(items->GetSize() - 1);
}

template<typename T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::Copy() const {
    return this->Clone();
}

template<typename T>
const T& ArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::Append(const T& item) {
    return this->Instance()->AppendInternal(item); // мутабл добавит в себя, иммутабл в копию и вернёт её
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::Prepend(const T& item) {
    return this->Instance()->PrependInternal(item);
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) {
    // проверяем до Instance(), иначе у иммутабл копия утечёт при исключении
    if (index < 0 || index >= items->GetSize()) throw std::out_of_range("неправильный индекс");
    return this->Instance()->InsertAtInternal(item, index);
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::Concat(const Sequence<T>* list) {
    if (list == nullptr) throw std::invalid_argument("неправильный аргумент"); // до Instance(), как в InsertAt
    return this->Instance()->ConcatInternal(list);
}

template<typename T>
ArraySequence<T>* ArraySequence<T>::GetSubSequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || startIndex >= items->GetSize() ||
        endIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex) {
        throw std::out_of_range("неправильный индекс");
    }

    int subLength = endIndex - startIndex + 1;
    DynamicArray<T> sub(subLength);
    for (int i = 0; i < subLength; i++) {
        sub.Set(i, items->Get(startIndex + i));
    }

    return this->CreateInstance(sub);
}

// один проход: предикат вызывается один раз на эл, а рост по одному с ёмкостью в среднем O(1)
template<typename T>
Sequence<T>* ArraySequence<T>::Where(bool (*predicate)(const T&)) const {
    DynamicArray<T> temp(0);
    for (int i = 0; i < items->GetSize(); i++){
        const T& value = items->Get(i);
        if (predicate(value)) {
            temp.Resize(temp.GetSize() + 1);
            temp.Set(temp.GetSize() - 1, value);
        }
    }
    return this->CreateInstance(temp);
}
