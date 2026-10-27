#pragma once

#include "FileStream.h"
#include <stdexcept>

// --- FileReadStream ---

template<typename T>
void FileReadStream<T>::ReadAhead() {
    nextLine = Option<std::string>();
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            nextLine = Option<std::string>(line);
            return;
        }
    }
}

template<typename T>
bool FileReadStream<T>::HasElementAt(int) const {
    return nextLine.HasValue(); // файл читаем только подряд, index всегда текущий
}

template<typename T>
T FileReadStream<T>::ReadAt(int) {
    T item = deserialize(nextLine.GetValue());
    ReadAhead();
    return item;
}

template<typename T>
void FileReadStream<T>::OnOpen() {
    file.close();
    file.clear();
    file.open(fileName);
    if (!file.is_open()) throw std::invalid_argument("не удалось открыть файл " + fileName);
    ReadAhead();
}

template<typename T>
void FileReadStream<T>::OnClose() {
    file.close();
    nextLine = Option<std::string>();
}

template<typename T>
FileReadStream<T>::FileReadStream(const std::string& fileName, T (*deserialize)(const std::string&)) :
    fileName(fileName), deserialize(deserialize) {
    if (deserialize == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
}

template<typename T>
bool FileReadStream<T>::IsCanSeek() const {
    return false;
}

template<typename T>
bool FileReadStream<T>::IsCanGoBack() const {
    return false;
}

// --- FileWriteStream ---

template<typename T>
void FileWriteStream<T>::WriteItem(const T& item) {
    file << serialize(item) << '\n';
    if (!file) throw std::invalid_argument("не удалось записать в файл " + fileName);
}

template<typename T>
void FileWriteStream<T>::OnOpen() {
    if (file.is_open()) return;
    if (wasOpened) file.open(fileName, std::ios::app);
    else file.open(fileName);
    if (!file.is_open()) throw std::invalid_argument("не удалось открыть файл " + fileName);
    wasOpened = true;
}

template<typename T>
void FileWriteStream<T>::OnClose() {
    file.close();
}

template<typename T>
FileWriteStream<T>::FileWriteStream(const std::string& fileName, std::string (*serialize)(const T&)) :
    fileName(fileName), serialize(serialize), wasOpened(false) {
    if (serialize == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
}
