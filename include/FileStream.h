#pragma once

#include <fstream> // для чтения файла и записи в файл 
#include <string>
#include "Option.h"
#include "Stream.h"

// читает файл по строке, целиком в память не грузит. одна непустая строка = один эл
template<typename T>
class FileReadStream : public ReadOnlyStream<T> {
    std::string fileName;
    T (*deserialize)(const std::string&); // указатель на функцию "string" -> T
    std::ifstream file;
    Option<std::string> nextLine; // строка прочитанная заранее, чтоб знать есть ли следующий эл

    void ReadAhead(); //прочитать следующую непустую строчку 
protected:
    bool HasElementAt(int) const override;
    T ReadAt(int) override;
    void OnOpen() override;
    void OnClose() override;
public:
    FileReadStream(const std::string& fileName, T (*deserialize)(const std::string&));
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
};

// пишет каждый эл отдельной строкой
template<typename T>
class FileWriteStream : public WriteOnlyStream<T> {
    std::string fileName;
    std::string (*serialize)(const T&);
    std::ofstream file;
    bool wasOpened; // первый Open создаёт файл заново, следующие дописывают в конец
protected:
    void WriteItem(const T& item) override;
    void OnOpen() override;
    void OnClose() override;
public:
    FileWriteStream(const std::string& fileName, std::string (*serialize)(const T&));
};

#include "FileStream.tpp"
