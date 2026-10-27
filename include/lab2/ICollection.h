#pragma once

template<typename T>
class ICollection {
public:
    virtual ~ICollection() = default;
    virtual const T& Get(int index) const = 0;
    virtual int GetCount() const = 0;
    // сам конструктор виртуальным быть не может,
    // а этот метод через указатель на интерфейс создаёт копию того же типа, что и объект
    virtual ICollection<T>* Copy() const = 0; // обьъект сам знает свой тип и вызовет свой конструктор копироавния
    // можно писать вот так ICollection<int>* copy = c->Copy();  
};
