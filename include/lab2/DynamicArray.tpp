#pragma once

#include "DynamicArray.h"
#include <stdexcept>

template <typename T>
DynamicArray<T>::DynamicArray(int size) : size(size), capacity(size){
    if (size < 0){
        throw std::invalid_argument("size не может быть меньше нуля");
    }
    data = new T[size](); 
}

template <typename T>
DynamicArray<T>::DynamicArray(const T* items, int count) : size(count), capacity(count){
    if (count < 0 || (count > 0 && items == nullptr)){
        throw std::invalid_argument("неправильные данные для копирования элементов массива");
    }
    data = new T[count];
    for (int i = 0; i < count; i++){
        data[i] = items[i];
    }
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : size(other.size), capacity(other.size){ 
    data = new T[other.size];
    for (int i = 0; i < other.size; i++){
        data[i] = other.data[i];
    }
}

template <typename T>
DynamicArray<T>::~DynamicArray(){
    delete[] data;
}

template <typename T>
const T& DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size) throw std::out_of_range("неверный индекс");
    return data[index];
}

template <typename T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template <typename T>
int DynamicArray<T>::GetCapacity() const {
    return capacity;
}

template <typename T>
void DynamicArray<T>::Set(int index, const T& value){
    if (index < 0 || index >= size) throw std::out_of_range("неверный индекс");
    data[index] = value;
}

template <typename T>
void DynamicArray<T>::Resize(int newSize){
    if (newSize < 0) throw std::invalid_argument("некорректные данные для изменения размера массива");
    if (newSize <= capacity) { 
        for (int i = size; i < newSize; i++) data[i] = T(); // знач по умолчанию 
        size = newSize;
        return;
    }
    int newCapacity = capacity * 2;
    if (newCapacity < newSize) newCapacity = newSize; // при capacity == 0 удвоение не поможет
    T* newData = new T[newCapacity]();
    for (int i = 0; i < size; i++) newData[i] = data[i];
    delete[] data;
    data = newData;
    size = newSize;
    capacity = newCapacity;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other){
    if (this == &other) return *this; // это просто оптимизация, ошибки или утечки не будет

    T* newData = new T[other.size];

    for (int i = 0; i < other.size; i++){
        newData[i] = other.data[i];
    }

    delete[] data;

    data = newData;
    size = other.size;
    capacity = other.size;

    return *this;
}

template <typename T>
T& DynamicArray<T>::operator[](int index) {
    if (index < 0 || index >= this->size){
        throw std::out_of_range("Индекс вне диапазона");
    }

    return this->data[index];
}

template <typename T>
const T& DynamicArray<T>::operator[](int index) const {
    if (index < 0 || index >= this->size){
        throw std::out_of_range("Индекс вне диапазона");
    }

    return this->data[index];
}

template <typename T>
int DynamicArray<T>::GetCount() const {
    return GetSize();
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::Copy() const { //тупо вызов конструктора копирваония 
    return new DynamicArray<T>(*this);
}
