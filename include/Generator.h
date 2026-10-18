#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdexcept>
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
    virtual IGenerator<T>* CloneFromStart() const = 0;
    Option<T> TryGetNext() {
        if (HasNext()) return Option<T>(GetNext());
        return Option<T>();
    }
};

template<typename T>
class ArrayGenerator : public IGenerator<T> {
    DynamicArray<T> items;
    int position;
public:
    explicit ArrayGenerator(const DynamicArray<T>& sourse) : items(sourse), position(0) {}
    explicit ArrayGenerator(const Sequence<T>* sourse) : items(0), position(0){
        if (sourse == nullptr) throw std::invalid_argument("Неверный указатель");
        items.Resize(sourse->GetLength());
        for (int i = 0; i < sourse->GetLength(); i++) items.Set(i, sourse->Get(i));
    }
    bool HasNext() const override {
        return position < items.GetSize();
    }

    T GetNext() override {
        if (!HasNext()) throw std::out_of_range("следующего элемента нету");
        int i = position++;
        return items.Get(i);
    }

    IGenerator<T>* CloneFromStart() const override {
        return new ArrayGenerator<T>(items);
    }
};

template<typename T>
class RuleGenerator : public IGenerator<T> {
    T (*rule)(const Sequence<T>*);
    MutableArraySequence<T> initialTerms; // начальные члены 
    MutableArraySequence<T> window; // последние элементы
    int position;
public:
    // initialTerms, window можно не писать
    // тк есть конструкторы по умолч в отличии от дайнамик эррэй 
    RuleGenerator(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) : rule(rule), position(0) {
        if (rule == nullptr) throw std::invalid_argument("неправильный указатель на функцию");
        if (initial == nullptr) throw std::invalid_argument("неправильный указатель на массив первых членов");
        if (initial->GetLength() == 0) throw std::invalid_argument("правилу неизчего следовать");
        
        for (int i = 0; i < initial->GetLength(); i++) {
            initialTerms.Append(initial->Get(i));
            window.Append(initial->Get(i));
        }
    }

    bool HasNext() const override { return true; }

    T GetNext() override {
        if (position < initialTerms.GetLength()) {
            return initialTerms.Get(position++);
        } 
        T next = rule(&window);
        for (int i = 0; i < window.GetLength() - 1; i++) {
            window[i] = window[i+1];
        }
        window[window.GetLength() - 1] = next;
        return next;
    }
    
    IGenerator<T>* CloneFromStart() const override {
        return new RuleGenerator<T>(rule, &initialTerms);
    }

};









#endif