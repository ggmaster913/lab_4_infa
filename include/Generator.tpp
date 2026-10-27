#pragma once

#include "Generator.h"
#include <stdexcept>

template<typename T>
Option<T> IGenerator<T>::TryGetNext() {
    if (HasNext()) return Option<T>(GetNext());
    return Option<T>();
}

template<typename T>
ArrayGenerator<T>::ArrayGenerator(const DynamicArray<T>& source) : items(source), position(0) {}

// items(0) тк размер узнаем после проверки на nullptr
template<typename T>
ArrayGenerator<T>::ArrayGenerator(const Sequence<T>* source) : items(0), position(0){
    if (source == nullptr) throw std::invalid_argument("Неверный указатель");
    items.Resize(source->GetLength());
    for (int i = 0; i < source->GetLength(); i++) items.Set(i, source->Get(i));
}

template<typename T>
bool ArrayGenerator<T>::HasNext() const {
    return position < items.GetSize();
}

template<typename T>
T ArrayGenerator<T>::GetNext() {
    if (!HasNext()) throw std::out_of_range("следующего элемента нет");
    int i = position++;
    return items.Get(i);
}

template<typename T>
IGenerator<T>* ArrayGenerator<T>::CloneFromStart() const {
    return new ArrayGenerator<T>(items);
}

// initialTerms, window можно не писать
// тк есть конструкторы по умолч в отличии от дайнамик эррэй
template<typename T>
RuleGenerator<T>::RuleGenerator(T (*rule)(const Sequence<T>*), const Sequence<T>* initial) : rule(rule), position(0) {
    if (rule == nullptr) throw std::invalid_argument("неправильный указатель на функцию");
    if (initial == nullptr) throw std::invalid_argument("неправильный указатель на массив первых членов");
    if (initial->GetLength() == 0) throw std::invalid_argument("правилу не из чего следовать");

    for (int i = 0; i < initial->GetLength(); i++) {
        initialTerms.Append(initial->Get(i));
        window.Append(initial->Get(i));
    }
}

template<typename T>
bool RuleGenerator<T>::HasNext() const {
    return true;
}

template<typename T>
T RuleGenerator<T>::GetNext() {
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

template<typename T>
IGenerator<T>* RuleGenerator<T>::CloneFromStart() const {
    return new RuleGenerator<T>(rule, &initialTerms);
}
