#pragma once

#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <thread>


struct Entry {
    size_t doc_id, count;
    // Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};


class InvertedIndex {
public:
    InvertedIndex() = default;


    void UpdateDocumentBase(std::vector<std::string> input_docs);


    std::vector<Entry> GetWordCount(const std::string& word);

private:
    std::vector<std::string> docs; // список содержимого документов
    std::map<std::string, std::vector<Entry>> freq_dictionary; // частотный словарь
    std::mutex freq_dictionary_mutex; // мьютекс для безопасной работы с частотным словарем


    void IndexDocument(size_t doc_id, const std::string& content);


    std::vector<std::string> SplitIntoWords(const std::string& text);
};