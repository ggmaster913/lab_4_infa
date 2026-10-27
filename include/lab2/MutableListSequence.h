#pragma once

#include "ListSequence.h"

template<typename T>
class MutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }

    ListSequence<T>* Instance() override {
        return this;
    }

    ListSequence<T>* CreateInstance(const LinkedList<T>& subArray) const override{
        return new MutableListSequence<T>(subArray);
    }

public:
    // конструкторы не наследуются, поэтому передаём их в ListSequence явно
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(const T* data, int count) : ListSequence<T>(data, count) {}
    MutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    MutableListSequence(const DynamicArray<T>& array) : ListSequence<T>(array) {}

    // const версию пишем явно, иначе из за T& operator[] не перенеслось из Sequence тк есть уже оператор 
    const T& operator[](int index) const {
        return this->Get(index);
    }

    T& operator[](int index){
        return (*this->items)[index];
    }
};
