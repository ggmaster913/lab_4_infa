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
    // копия с начала, удаляет вызвавший
    virtual IGenerator<T>* CloneFromStart() const = 0;
    // один раз для всех наследников
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
    explicit ArrayGenerator(const DynamicArray<T>& source) : items(source), position(0) {}
    // items(0) тк размер узнаем после проверки на nullptr
    explicit ArrayGenerator(const Sequence<T>* source) : items(0), position(0){
        if (source == nullptr) throw std::invalid_argument("Неверный указатель");
        items.Resize(source->GetLength());
        for (int i = 0; i < source->GetLength(); i++) items.Set(i, source->Get(i));
    }
    bool HasNext() const override {
        return position < items.GetSize();
    }

    T GetNext() override {
        if (!HasNext()) throw std::out_of_range("следующего элемента нет");
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
    int position; // только для начальных членов
public:
    // initialTerms, window можно не писать
    // тк есть конструкторы по умолч в отличии от дайнамик эррэй 
    RuleGenerator(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) : rule(rule), position(0) {
        if (rule == nullptr) throw std::invalid_argument("неправильный указатель на функцию");
        if (initial == nullptr) throw std::invalid_argument("неправильный указатель на массив первых членов");
        if (initial->GetLength() == 0) throw std::invalid_argument("правилу не из чего следовать");
        
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
        // считаем по правилу от последних k эл
        T next = rule(&window);
        // сдвигаем окно, старый выпадает, новый в конец
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
