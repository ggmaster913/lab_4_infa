#pragma once

#include "ListSequence.h"

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    ListSequence<T>* Instance() override {
        return this->Clone();
    }

    ListSequence<T>* CreateInstance(const LinkedList<T>& subArray) const override{
        return new ImmutableListSequence<T>(subArray);
    }

public:
    // конструкторы не наследуются, поэтому передаём их в ListSequence явно
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(const T* data, int count) : ListSequence<T>(data, count) {}
    ImmutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}
    ImmutableListSequence(const DynamicArray<T>& array) : ListSequence<T>(array) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequence<T>(other) {} // нужен для Clone() тк не operator= ... = delete;

    // присваивание запрещено, иначе a = b перезаписало бы иммутабл объект
    ImmutableListSequence<T>& operator=(const ImmutableListSequence<T>&) = delete;

};
