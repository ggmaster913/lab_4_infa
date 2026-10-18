#ifndef ICOLLECTION_H
#define ICOLLECTION_H

template<typename T>
class ICollection {
public:
    virtual ~ICollection() = default;
    virtual const T& Get(int index) const = 0;
    virtual int GetCount() const = 0;
};

#endif