#ifndef MUTABLEARRAYSEQUENCE_H
#define MUTABLEARRAYSEQUENCE_H

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
    using ArraySequence<T>::ArraySequence; //перетаскиваем конструкторы из ArraySequence только теперь для MutableArraySequence 
    using Sequence<T>::operator[];

    T& operator[](int index){
        return (*this->items)[index];
    }
};

template<typename T>
template<typename T2>
Sequence<T2>* Sequence<T>::Map(T2 (*f)(const T&)) const {
    MutableArraySequence<T2>* result = new MutableArraySequence<T2>();
    int len = this->GetLength();
    for (int i = 0; i < len; i++){
        result->Append(f(this->Get(i)));
    }
    return result;
}

#endif