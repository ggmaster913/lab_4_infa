#pragma once

#include "ICollection.h"

template <typename T>
class DynamicArray : public ICollection<T> {
    T* data;
    int size;     // сколько элементов реально есть
    int capacity; // под сколько элементов выделена память, всегда >= siz
public:
    explicit DynamicArray(int size); //запрет неявного преобразования типов. чтоб выдавало ошибку при, (PrintArray(6))
    DynamicArray(const T* items, int count);
    DynamicArray(const DynamicArray<T>& other);
    ~DynamicArray();

    const T& Get(int index) const override; // чтоб нельзя было сделать arr.Get(5) = 8;
    int GetSize() const;
    int GetCapacity() const;
    void Set(int index, const T& value);

    void Resize(int newSize);

    // a = b = c
    DynamicArray<T>& operator=(const DynamicArray<T>& other);

    T& operator[](int index);
    const T& operator[](int index) const;

    int GetCount() const override;
    DynamicArray<T>* Copy() const override;
};

#include "DynamicArray.tpp"
