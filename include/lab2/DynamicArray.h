#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>
#include "ICollection.h"

template <typename T>
class DynamicArray : public ICollection<T> {
    T* data;
    int size;     // сколько элементов реально есть
    int capacity; // под сколько элементов выделена память, всегда >= size
public:
    explicit DynamicArray(int size) : size(size), capacity(size){ //запрет неявного преобразования типов. чтоб выдавало ошибку при, (PrintArray(6))
        if (size < 0){
            throw std::invalid_argument("size не может быть меньше нуля");
        }
        data = new T[size]();
    }

    DynamicArray(const T* items, int count) : size(count), capacity(count){
        if (count < 0 || (count > 0 && items == nullptr)){
            throw std::invalid_argument("неправильные данные для копирования элементов массива");
        }
        data = new T[count];
        for (int i = 0; i < count; i++){
            data[i] = items[i];
        }
    }

    DynamicArray(const DynamicArray<T>& other) : size(other.size), capacity(other.size){ // копируем только занятую часть
        data = new T[other.size];
        for (int i = 0; i < other.size; i++){
            data[i] = other.data[i];
        } 
    }

    ~DynamicArray(){
        delete[] data;
    }

    const T& Get(int index) const override { // чтоб нельзя было сделать arr.Get(5) = 8;
        if (index < 0 || index >= size) throw std::out_of_range("неверный индекс");
        return data[index];
    }

    int GetSize() const {
        return size;
    }

    int GetCapacity() const {
        return capacity;
    }

    void Set(int index, const T& value){
        if (index < 0 || index >= size) throw std::out_of_range("неверный индекс");
        data[index] = value;
    }

    // меняет размер до newSize; память выделяется с запасом (вдвое), поэтому Append в среднем O(1)
    void Resize(int newSize){
        if (newSize < 0) throw std::invalid_argument("некорректные данные для изменения размера массива");
        if (newSize <= capacity) { // место есть, память не трогаем
            for (int i = size; i < newSize; i++) data[i] = T(); // новые ячейки пустые, там мог остаться мусор после уменьшения
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

    DynamicArray<T>& operator=(const DynamicArray<T>& other){
        if (this == &other) return *this;

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

    T& operator[](int index) {
        if (index < 0 || index >= this->size){
            throw std::out_of_range("Индекс вне диапазона");
        }

        return this->data[index];
    }

    int GetCount() const override {
        return GetSize();
    }


};
#endif