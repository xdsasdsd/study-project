#include "../include/InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <cctype>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs.clear();
    freq_dictionary.clear();

    docs = std::move(input_docs);

    std::vector<std::thread> indexing_threads;

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        indexing_threads.emplace_back(&InvertedIndex::IndexDocument, this, doc_id, std::ref(docs[doc_id]));
    }

    for (auto& thread : indexing_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void InvertedIndex::IndexDocument(size_t doc_id, const std::string& content) {
    auto words = SplitIntoWords(content);

    std::map<std::string, size_t> word_count;
    for (const auto& word : words) {
        ++word_count[word];
    }

    std::lock_guard<std::mutex> lock(freq_dictionary_mutex);

    for (const auto& [word, count] : word_count) {
        auto& entries = freq_dictionary[word];
        auto it = std::find_if(entries.begin(), entries.end(),
                               [doc_id](const Entry& entry) { return entry.doc_id == doc_id; });

        if (it != entries.end()) {
            it->count = count;
        } else {
            entries.push_back({doc_id, count});
        }
    }
}

std::vector<std::string> InvertedIndex::SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;

    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        words.push_back(word);
    }

    return words;
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::string lower_word = word;
    std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    auto it = freq_dictionary.find(lower_word);
    if (it != freq_dictionary.end()) {
        return it->second;
    }

    return {};
}