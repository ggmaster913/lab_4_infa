#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>
#include "ICollection.h"

template <typename T>
class DynamicArray : public ICollection<T> {
    T* data;
    int size;
public:
    explicit DynamicArray(int size) : size(size){ //запрет неявного преобразования типов. чтоб выдавало ошибку при, (PrintArray(6)) 
        if (size < 0){
            throw std::invalid_argument("size не может быть меньше нуля");
        }
        data = new T[size]();
    }

    DynamicArray(const T* items, int count) : size(count){
        if (count < 0 || (count > 0 && items == nullptr)){
            throw std::invalid_argument("не правильные данные для копирования эл массива");
        }
        data = new T[count];
        for (int i = 0; i < count; i++){
            data[i] = items[i];
        }
    }

    DynamicArray(const DynamicArray<T>& other) : size(other.size){
        data = new T[other.size];
        for (int i = 0; i < other.size; i++){
            data[i] = other.data[i];
        } 
    }

    ~DynamicArray(){
        delete[] data;
    }

    const T& Get(int index) const override { // чтоб нельзя было сделать arr.Get(5) = 8;
        if (index < 0 || index >= size) throw std::out_of_range("индек не верный");
        return data[index];
    }

    int GetSize() const {
        return size;
    }

    void Set(int index, const T& value){
        if (index < 0 || index >= size) throw std::out_of_range("индекc не верный");
        data[index] = value;
    }

    // меняет размер ровно до newSize, когда и насколько растить решает ArraySequence
    void Resize(int newSize){
        if (newSize < 0) throw std::invalid_argument("некорректные данные для изменения размера массива");
        if (newSize == size) return;       
        T* newData =  new T[newSize]();
        int tCount = (newSize < size) ? newSize : size;
        for (int i = 0; i < tCount; i++) newData[i] = data[i];
        size = newSize;
        delete[] data;
        data = newData;
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