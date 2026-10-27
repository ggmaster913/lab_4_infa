#pragma once

#include "ICollection.h"

template<typename T>
class Sequence : public ICollection<T> {
public:
    virtual ~Sequence() = default;

    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    const T& Get(int index) const override = 0;
    virtual Sequence<T>* GetSubSequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;
    int GetCount() const override; // то же, что GetLength, только под именем из ICollection
    Sequence<T>* Copy() const override = 0;

    virtual Sequence<T>* Append(const T& item ) = 0;
    virtual Sequence<T>* Prepend(const T& item ) = 0;
    virtual Sequence<T>* InsertAt(const T& item , int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* list) = 0;

    virtual Sequence<T>* Where(bool (*predicate)(const T&)) const = 0;

    // reduce и map не virtual: шаблоны не могут быть виртуальными, да и реализация одна для всех наследников
    template<typename T2>
    T2 Reduce(T2 (*f)(const T2&, const T&), const T2& initial) const;

    const T& operator[](int index) const;

    // тело в MutableArraySequence.tpp, тк тут MutableArraySequence ещё не известен
    template<typename T2>
    Sequence<T2>* Map(T2 (*f)(const T&)) const;
};

#include "Sequence.tpp"
