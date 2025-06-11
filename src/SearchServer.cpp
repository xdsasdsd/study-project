#include "SearchServer.h"
#include <sstream>
#include <algorithm>
#include <set>
#include <cmath>

std::vector<std::string> SearchServer::SplitIntoWords(const std::string& text) {
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

std::vector<RelativeIndex> SearchServer::ProcessQuery(const std::string& query) {
    auto queryWords = SplitIntoWords(query);

    std::set<std::string> uniqueWords(queryWords.begin(), queryWords.end());

    std::vector<std::string> sortedUniqueWords(uniqueWords.begin(), uniqueWords.end());
    std::sort(sortedUniqueWords.begin(), sortedUniqueWords.end(),
              [this](const std::string& a, const std::string& b) {
                  return _index.GetWordCount(a).size() < _index.GetWordCount(b).size();
              });

    if (sortedUniqueWords.empty()) {
        return {};
    }

    auto rarestWordEntries = _index.GetWordCount(sortedUniqueWords[0]);

    if (rarestWordEntries.empty()) {
        return {};
    }

    std::map<size_t, float> documentAbsRelevance;

    for (const auto& entry : rarestWordEntries) {
        documentAbsRelevance[entry.doc_id] = entry.count;
    }

    for (size_t i = 1; i < sortedUniqueWords.size(); ++i) {
        auto wordEntries = _index.GetWordCount(sortedUniqueWords[i]);

        if (wordEntries.empty()) {
            continue;
        }

        std::map<size_t, float> updatedRelevance;

        for (const auto& entry : wordEntries) {
            if (documentAbsRelevance.count(entry.doc_id) > 0) {
                updatedRelevance[entry.doc_id] = documentAbsRelevance[entry.doc_id] + entry.count;
            }
        }

        documentAbsRelevance = std::move(updatedRelevance);

        if (documentAbsRelevance.empty()) {
            return {};
        }
    }

    std::vector<std::pair<size_t, float>> relevanceVec(documentAbsRelevance.begin(), documentAbsRelevance.end());

    if (relevanceVec.empty()) {
        return {};
    }

    float maxAbsRelevance = 0.0f;
    for (const auto& [doc_id, abs_rank] : relevanceVec) {
        maxAbsRelevance = std::max(maxAbsRelevance, abs_rank);
    }

    if (maxAbsRelevance == 0) {
        return {};
    }

    std::vector<RelativeIndex> result;
    for (const auto& [doc_id, abs_rank] : relevanceVec) {
        float relRank = abs_rank / maxAbsRelevance;
        result.push_back({doc_id, relRank});
    }


    std::sort(result.begin(), result.end(),
              [](const RelativeIndex& a, const RelativeIndex& b) {
                  if (std::abs(a.rank - b.rank) < 1e-6) {
                      return a.doc_id < b.doc_id;
                  }
                  return a.rank > b.rank;
              });

    return result;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;
    results.reserve(queries_input.size());

    for (const auto& query : queries_input) {
        auto queryResult = ProcessQuery(query);
        results.push_back(queryResult);
    }

    return results;
}