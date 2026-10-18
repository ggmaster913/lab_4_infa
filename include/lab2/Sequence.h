#ifndef SEQUENCE_H
#define SEQUENCE_H

template<typename T>
class Sequence{
public:
    virtual ~Sequence() = default;

    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    virtual const T& Get(int index) const = 0;
    virtual Sequence<T>* GetSubSequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;

    virtual Sequence<T>* Append(const T& item ) = 0;
    virtual Sequence<T>* Prepend(const T& item ) = 0;
    virtual Sequence<T>* InsertAt(const T& item , int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* list) = 0;

    virtual Sequence<T>* Where(bool (*predicate)(const T&)) const = 0;

    template<typename T2>
    T2 Reduce(T2 (*f)(const T2&, const T&), const T2& initial) const {
        T2 accum = initial;
        int len = this->GetLength();
        for (int i = 0; i < len; i++){
            accum = f(accum, this->Get(i));
        }
        return accum;
    }

    const T& operator[](int index) const {
        return this->Get(index);
    }

    template<typename T2>
    Sequence<T2>* Map(T2 (*f)(const T&)) const;
};


#endif 