#pragma once

#include "Ordinal.h"
#include "LazySequence.h"
#include "lab2/Sequence.h"

// источником не владеет, источник должен жить дольше потока
template<typename T>
class ReadOnlyStream {
    int position;
    bool isOpen;
protected:
    virtual bool HasElementAt(int index) const = 0;
    virtual T ReadAt(int index) = 0;
    virtual void OnOpen() {} 
    virtual void OnClose() {}
public:
    ReadOnlyStream();
    virtual ~ReadOnlyStream() = default;

    void Open();
    void Close();
    bool IsEndOfStream() const;
    T Read();
    int GetPosition() const;
    virtual bool IsCanSeek() const;
    int Seek(int index);
    virtual bool IsCanGoBack() const;
};

template<typename T>
class SequenceReadStream : public ReadOnlyStream<T> {
    const Sequence<T>* source;
protected:
    bool HasElementAt(int index) const override;
    T ReadAt(int index) override;
public:
    explicit SequenceReadStream(const Sequence<T>* source);
};

template<typename T>
class LazyReadStream : public ReadOnlyStream<T> {
    LazySequence<T>* source;
protected:
    bool HasElementAt(int index) const override;
    T ReadAt(int index) override;
public:
    explicit LazyReadStream(LazySequence<T>* source);
};

template<typename T>
class WriteOnlyStream {
    int position;
    bool isOpen;
protected:
    virtual void WriteItem(const T& item) = 0;
    virtual void OnOpen() {}
    virtual void OnClose() {}
public:
    WriteOnlyStream();
    virtual ~WriteOnlyStream() = default;

    void Open();
    void Close();
    int GetPosition() const;
    int Write(const T& item);
};

template<typename T>
class SequenceWriteStream : public WriteOnlyStream<T> {
    Sequence<T>* target;
protected:
    void WriteItem(const T& item) override;
public:
    explicit SequenceWriteStream(Sequence<T>* target);
};

#include "Stream.tpp"
