#ifndef MUTABLELISTSEQUENCE_H
#define MUTABLELISTSEQUENCE_H

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
    using ListSequence<T>::ListSequence; //перетаскиваем конструкторы из ArraySequence только теперь для MutableArraySequence 
    using Sequence<T>::operator[];
    
    T& operator[](int index){
        return (*this->items)[index];
    }
};

#endif