#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <fstream>
#include <string>
#include <stdexcept>
#include "Option.h"
#include "Stream.h"

// читает файл по строке, целиком в память не грузит. одна непустая строка = один эл
template<typename T>
class FileReadStream : public ReadOnlyStream<T> {
    std::string fileName;
    T (*deserialize)(const std::string&);
    std::ifstream file;
    Option<std::string> nextLine; // читаем на строку вперёд, чтоб знать есть ли следующий эл

    void ReadAhead() {
        nextLine = Option<std::string>();
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                nextLine = Option<std::string>(line);
                return;
            }
        }
    }
protected:
    bool HasElementAt(int) const override {
        return nextLine.HasValue(); // файл читаем только подряд, index всегда текущий
    }
    T ReadAt(int) override {
        T item = deserialize(nextLine.GetValue());
        ReadAhead();
        return item;
    }
    void OnOpen() override {
        file.close();
        file.clear();
        file.open(fileName);
        if (!file.is_open()) throw std::invalid_argument("не удалось открыть файл " + fileName);
        ReadAhead();
    }
    void OnClose() override {
        file.close();
        nextLine = Option<std::string>();
    }
public:
    FileReadStream(const std::string& fileName, T (*deserialize)(const std::string&)) :
        fileName(fileName), deserialize(deserialize) {
        if (deserialize == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
    }
    bool IsCanSeek() const override {
        return false;
    }
    bool IsCanGoBack() const override {
        return false;
    }
};

// пишет каждый эл отдельной строкой
template<typename T>
class FileWriteStream : public WriteOnlyStream<T> {
    std::string fileName;
    std::string (*serialize)(const T&);
    std::ofstream file;
    bool wasOpened; // первый Open создаёт файл заново, следующие дописывают в конец
protected:
    void WriteItem(const T& item) override {
        file << serialize(item) << '\n';
        if (!file) throw std::invalid_argument("не удалось записать в файл " + fileName);
    }
    void OnOpen() override {
        if (file.is_open()) return;
        if (wasOpened) file.open(fileName, std::ios::app);
        else file.open(fileName);
        if (!file.is_open()) throw std::invalid_argument("не удалось открыть файл " + fileName);
        wasOpened = true;
    }
    void OnClose() override {
        file.close();
    }
public:
    FileWriteStream(const std::string& fileName, std::string (*serialize)(const T&)) :
        fileName(fileName), serialize(serialize), wasOpened(false) {
        if (serialize == nullptr) throw std::invalid_argument("указатель на функцию не может быть nullptr");
    }
};

#endif
