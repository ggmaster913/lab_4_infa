#pragma once

#include "ArraySequence.h"

template<typename T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }

    ArraySequence<T>* Instance() override {
        return this;
    }

    ArraySequence<T>* CreateInstance(const DynamicArray<T>& subArray) const override{
        return new MutableArraySequence<T>(subArray);
    }

public:
    // конструкторы не наследуются, поэтому передаём их в ArraySequence явно
    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(const T* data, int count) : ArraySequence<T>(data, count) {}
    MutableArraySequence(const DynamicArray<T>& array) : ArraySequence<T>(array) {}
    MutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}

    // const версию пишем явно, иначе из за T& operator[] не перенеслось из Sequence тк есть уже оператор 
    const T& operator[](int index) const {
        return this->Get(index);
    }

    T& operator[](int index){
        return (*this->items)[index];
    }

};

#include "MutableArraySequence.tpp"
