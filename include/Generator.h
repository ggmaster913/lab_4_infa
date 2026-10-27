#pragma once

#include "Option.h"
#include "lab2/Sequence.h"
#include "lab2/DynamicArray.h"
#include "lab2/MutableArraySequence.h"

template<typename T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    // копия с начала, удаляет вызвавший
    virtual IGenerator<T>* CloneFromStart() const = 0;
    // один раз для всех наследников
    Option<T> TryGetNext();
};

template<typename T>
class ArrayGenerator : public IGenerator<T> {
    DynamicArray<T> items;
    int position;
public:
    explicit ArrayGenerator(const DynamicArray<T>& source);
    explicit ArrayGenerator(const Sequence<T>* source);
    bool HasNext() const override;
    T GetNext() override;
    IGenerator<T>* CloneFromStart() const override;
};

template<typename T>
class RuleGenerator : public IGenerator<T> {
    T (*rule)(const Sequence<T>*); // принимает последовательность и возвращает след элемент 
    MutableArraySequence<T> initialTerms; // начальные члены
    MutableArraySequence<T> window; // последние элементы
    int position; // только для начальных членов
public:
    RuleGenerator(T (*rule)(const Sequence<T>*), const Sequence<T>* initial);
    bool HasNext() const override;
    T GetNext() override;
    IGenerator<T>* CloneFromStart() const override;
};

#include "Generator.tpp"
