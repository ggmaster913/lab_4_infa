#pragma once

#include "ArraySequence.h"

template<typename T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    ArraySequence<T>* Instance() override {
        return this->Clone();
    }

    ArraySequence<T>* CreateInstance(const DynamicArray<T>& subArray) const override{
        return new ImmutableArraySequence<T>(subArray);
    }

public:
    // T& operator[] нет специально, чтоб нельзя было поменять эл
    // конструкторы не наследуются, поэтому передаём их в ArraySequence явно
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(const T* data, int count) : ArraySequence<T>(data, count) {}
    ImmutableArraySequence(const DynamicArray<T>& array) : ArraySequence<T>(array) {}
    ImmutableArraySequence(const LinkedList<T>& list) : ArraySequence<T>(list) {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequence<T>(other) {} // нужен для Clone() тк не operator= ... = delete;

    // присваивание запрещено, иначе a = b перезаписало бы иммутабл объект
    ImmutableArraySequence<T>& operator=(const ImmutableArraySequence<T>&) = delete;

};
