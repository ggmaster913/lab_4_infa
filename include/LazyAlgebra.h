#ifndef LAZYALGEBRA_H
#define LAZYALGEBRA_H

#include <stdexcept>
#include "LazySequence.h"

// проверка равенства
template<typename T>
bool AreEqual(LazySequence<T>* first, LazySequence<T>* second, int count) {
    if (first == nullptr || second == nullptr ||  count < 0) throw std::invalid_argument("неправильные аргументы на входе AreEqual");
    int limit = count;
    if (first->GetLength().IsFinite() && first->GetLength().GetFinitePart() < limit) {
        limit = first->GetLength().GetFinitePart();
    }
    if (first->GetLength() != second->GetLength()) return false;
    for (int i = 0; i < limit; i++) {
        if (!(first->Get(i) == second->Get(i))) return false;
    }
    return true;
}

// (a + b) + c = a + (b + с)
template<typename T>
bool CheckAssociativity(LazySequence<T>* a, LazySequence<T>* b, LazySequence<T>* c, int count) {
    if (a == nullptr || b == nullptr ||  c == nullptr || count < 0) throw std::invalid_argument("неправильные аргументы на входе CheckAssociativity");
    LazySequence<T>* ab = a->Concat(b);
    LazySequence<T>* left = ab->Concat(c);
    LazySequence<T>* bc = b->Concat(c);
    LazySequence<T>* right = a->Concat(bc);
    bool result = AreEqual(left, right, count);
    delete ab;
    delete left;
    delete bc;
    delete right;
    return result;
}

// пустой + a = a + пустой = a 
template<typename T>
bool CheckIdentity(LazySequence<T>* a, int count) {
    if (a == nullptr || count < 0) throw std::invalid_argument("неправильные аргументы на входе CheckIdentity");
    LazySequence<T> empty;
    LazySequence<T>* left = empty.Concat(a);
    LazySequence<T>* right = a->Concat(&empty);
    bool result = AreEqual(left, a, count) && AreEqual(right, a, count);
    delete left;
    delete right;
    return result;
}

// a + b = b + a
template<typename T>
bool CheckCommutativity(LazySequence<T>* a, LazySequence<T>* b, int count) {
    if (a == nullptr || b == nullptr ||  count < 0) throw std::invalid_argument("неправильные аргументы на входе CheckCommutativity");
    LazySequence<T>* ab = a->Concat(b);
    LazySequence<T>* ba = b->Concat(a);
    bool result = AreEqual(ab, ba, count);
    delete ab;
    delete ba;
    return result;
}

// длина (a + b) = длина(a) + длина(b)
template<typename T>
bool CheckLengthAddition(LazySequence<T>* a, LazySequence<T>* b) {
    if (a == nullptr || b == nullptr) throw std::invalid_argument("неправильные аргументы на входе CheckLengthAddition");
    LazySequence<T>* ab = a ->Concat(b);
    bool result = ab->GetLength() == a->GetLength() + b->GetLength();
    delete ab;
    return result;
}

#endif