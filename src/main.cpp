#include "../include/ConverterJSON.h"
#include "../include/InvertedIndex.h"
#include "../include/SearchServer.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <set>

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void printHelp() {
    printHeader("SEARCH ENGINE - HELP");
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help                      - Show this help message" << std::endl;
    std::cout << "  index                     - Re-index all documents" << std::endl;
    std::cout << "  search <query>            - Search for documents (use quotes for multi-word query)" << std::endl;
    std::cout << "  word <word>               - Show statistics for a specific word" << std::endl;
    std::cout << "  find <word> [docs]        - Find documents containing the word (optional limit)" << std::endl;
    std::cout << "  compare <word1> <word2>   - Compare frequency of two words" << std::endl;
    std::cout << "  stats                     - Show index statistics" << std::endl;
    std::cout << "  process                   - Process all requests from requests.json" << std::endl;
    std::cout << "  exit                      - Exit the program" << std::endl;
}

void performIndexing(ConverterJSON& converter, InvertedIndex& index) {
    printHeader("INDEXING DOCUMENTS");
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        std::vector<std::string> documents = converter.GetTextDocuments();
        std::cout << "Indexing " << documents.size() << " documents..." << std::endl;

        index.UpdateDocumentBase(documents);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "Indexing completed in " << duration.count() << " ms" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during indexing: " << e.what() << std::endl;
    }
}

std::string getDocumentPreview(const std::vector<std::string>& documents, size_t docId, size_t maxLength = 50) {
    if (docId >= documents.size()) {
        return "[Document not found]";
    }

    std::string content = documents[docId];
    if (content.length() > maxLength) {
        return content.substr(0, maxLength) + "...";
    }
    return content;
}

void performSearch(const std::string& query, InvertedIndex& index, SearchServer& server, ConverterJSON& converter) {
    printHeader("SEARCH RESULTS FOR: " + query);

    if (query.empty()) {
        std::cout << "Error: Empty search query" << std::endl;
        return;
    }

    try {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<std::string> queryVec = {query};
        auto results = server.search(queryVec);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::vector<std::string> documents = converter.GetTextDocuments();

        if (results.empty() || results[0].empty()) {
            std::cout << "No documents found for query: " << query << std::endl;
        } else {
            std::cout << "Found " << results[0].size() << " document(s) in " << duration.count() << " ms" << std::endl;
            std::cout << std::setw(10) << "Doc ID" << std::setw(15) << "Relevance" << "  Content Preview" << std::endl;
            std::cout << std::string(70, '-') << std::endl;

            size_t max_responses = converter.GetResponsesLimit();
            size_t display_count = std::min(max_responses, results[0].size());

            for (size_t i = 0; i < display_count; ++i) {
                const auto& result = results[0][i];
                std::cout << std::setw(10) << result.doc_id
                          << std::setw(15) << std::fixed << std::setprecision(6) << result.rank
                          << "  " << getDocumentPreview(documents, result.doc_id) << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during search: " << e.what() << std::endl;
    }
}

void showWordStats(const std::string& word, InvertedIndex& index, const std::vector<std::string>& documents) {
    printHeader("WORD STATISTICS: " + word);

    if (word.empty()) {
        std::cout << "Error: Word is empty" << std::endl;
        return;
    }

    auto entries = index.GetWordCount(word);

    if (entries.empty()) {
        std::cout << "Word '" << word << "' not found in any document" << std::endl;
        return;
    }

    size_t totalCount = 0;
    for (const auto& entry : entries) {
        totalCount += entry.count;
    }

    std::cout << "Word: " << word << std::endl;
    std::cout << "Found in " << entries.size() << " document(s)" << std::endl;
    std::cout << "Total occurrences: " << totalCount << std::endl;
    std::cout << std::endl;

    std::cout << std::setw(10) << "Doc ID" << std::setw(10) << "Count"
              << "  Content Preview" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (const auto& entry : entries) {
        std::cout << std::setw(10) << entry.doc_id
                  << std::setw(10) << entry.count
                  << "  " << getDocumentPreview(documents, entry.doc_id) << std::endl;
    }
}

void findWordInDocuments(const std::string& word, InvertedIndex& index, const std::vector<std::string>& documents, int limit = -1) {
    printHeader("DOCUMENTS CONTAINING: " + word);

    if (word.empty()) {
        std::cout << "Error: Word is empty" << std::endl;
        return;
    }

    auto entries = index.GetWordCount(word);

    if (entries.empty()) {
        std::cout << "Word '" << word << "' not found in any document" << std::endl;
        return;
    }

    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.count > b.count; });

    std::cout << "Word '" << word << "' found in " << entries.size() << " document(s)" << std::endl;
    std::cout << std::endl;

    std::cout << std::setw(10) << "Doc ID" << std::setw(10) << "Count"
              << "  Content Preview" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    int count = 0;
    for (const auto& entry : entries) {
        std::cout << std::setw(10) << entry.doc_id
                  << std::setw(10) << entry.count
                  << "  " << getDocumentPreview(documents, entry.doc_id) << std::endl;

        count++;
        if (limit > 0 && count >= limit) {
            std::cout << "Showing " << count << " of " << entries.size() << " documents (limit reached)" << std::endl;
            break;
        }
    }
}

void compareWords(const std::string& word1, const std::string& word2, InvertedIndex& index) {
    printHeader("COMPARING WORDS: " + word1 + " vs " + word2);

    if (word1.empty() || word2.empty()) {
        std::cout << "Error: Words cannot be empty" << std::endl;
        return;
    }

    auto entries1 = index.GetWordCount(word1);
    auto entries2 = index.GetWordCount(word2);

    size_t totalCount1 = 0, totalCount2 = 0;
    std::set<size_t> docIds;

    for (const auto& entry : entries1) {
        totalCount1 += entry.count;
        docIds.insert(entry.doc_id);
    }

    for (const auto& entry : entries2) {
        totalCount2 += entry.count;
        docIds.insert(entry.doc_id);
    }

    std::cout << "Word '" << word1 << "' statistics:" << std::endl;
    std::cout << "  Documents: " << entries1.size() << std::endl;
    std::cout << "  Total occurrences: " << totalCount1 << std::endl;

    std::cout << "Word '" << word2 << "' statistics:" << std::endl;
    std::cout << "  Documents: " << entries2.size() << std::endl;
    std::cout << "  Total occurrences: " << totalCount2 << std::endl;

    std::cout << "Both words appear in " << docIds.size() << " unique document(s)" << std::endl;

    std::set<size_t> commonDocIds;
    for (const auto& entry1 : entries1) {
        for (const auto& entry2 : entries2) {
            if (entry1.doc_id == entry2.doc_id) {
                commonDocIds.insert(entry1.doc_id);
                break;
            }
        }
    }

    std::cout << "Documents containing both words: " << commonDocIds.size() << std::endl;

    if (totalCount1 > 0 && totalCount2 > 0) {
        float ratio = static_cast<float>(totalCount1) / totalCount2;
        std::cout << "Frequency ratio (" << word1 << "/" << word2 << "): "
                  << std::fixed << std::setprecision(2) << ratio << std::endl;
    }
}

void showStats(InvertedIndex& index) {
    printHeader("INDEX STATISTICS");

    std::vector<std::string> commonWords = {"the", "a", "is", "of", "and", "in", "to", "it", "that", "for"};

    std::cout << "Statistics for common words:" << std::endl;
    std::cout << std::setw(15) << "Word" << std::setw(15) << "Documents" << std::setw(15) << "Total Count" << std::endl;
    std::cout << std::string(45, '-') << std::endl;

    for (const auto& word : commonWords) {
        auto entries = index.GetWordCount(word);
        size_t totalCount = 0;
        for (const auto& entry : entries) {
            totalCount += entry.count;
        }

        std::cout << std::setw(15) << word
                  << std::setw(15) << entries.size()
                  << std::setw(15) << totalCount << std::endl;
    }
}

void processAllRequests(ConverterJSON& converter, SearchServer& server) {
    printHeader("PROCESSING ALL REQUESTS");

    try {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<std::string> requests = converter.GetRequests();
        std::cout << "Processing " << requests.size() << " requests..." << std::endl;

        auto results = server.search(requests);

        std::vector<std::vector<std::pair<int, float>>> formattedResults;
        for (const auto& queryResult : results) {
            std::vector<std::pair<int, float>> queryFormattedResult;

            for (const auto& item : queryResult) {
                queryFormattedResult.push_back({static_cast<int>(item.doc_id), item.rank});
            }

            formattedResults.push_back(queryFormattedResult);
        }

        converter.putAnswers(formattedResults);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        std::cout << "All requests processed in " << duration.count() << " ms" << std::endl;
        std::cout << "Results saved to answers.json" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error processing requests: " << e.what() << std::endl;
    }
}

std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    bool inQuotes = false;
    std::string current;

    for (char c : input) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

int main() {
    try {
        printHeader("SEARCH ENGINE");
        std::cout << "Type 'help' for a list of commands" << std::endl;

        std::cout << "Current directory: " << std::filesystem::current_path() << std::endl;

        ConverterJSON converter;
        std::cout << "Search engine: " << converter.GetName() << " v" << converter.GetVersion() << std::endl;

        std::vector<std::string> documents = converter.GetTextDocuments();

        InvertedIndex index;
        std::cout << "Indexing " << documents.size() << " documents..." << std::endl;
        index.UpdateDocumentBase(documents);
        std::cout << "Indexing completed successfully" << std::endl;

        SearchServer server(index);

        std::string input;
        bool running = true;

        while (running) {
            std::cout << "\n> ";
            std::getline(std::cin, input);

            if (input.empty()) {
                continue;
            }

            auto tokens = parseCommand(input);
            std::string command = tokens[0];

            std::transform(command.begin(), command.end(), command.begin(),
                          [](unsigned char c) { return std::tolower(c); });

            if (command == "help" || command == "h") {
                printHelp();
            } else if (command == "exit" || command == "quit" || command == "q") {
                std::cout << "Exiting search engine. Goodbye!" << std::endl;
                running = false;
            } else if (command == "index" || command == "reindex") {
                performIndexing(converter, index);
                documents = converter.GetTextDocuments();
            } else if (command == "search" || command == "s") {
                if (tokens.size() < 2) {
                    std::cout << "Error: Search query required" << std::endl;
                    std::cout << "Usage: search <query>" << std::endl;
                } else {
                    std::string query;
                    for (size_t i = 1; i < tokens.size(); ++i) {
                        if (i > 1) query += " ";
                        query += tokens[i];
                    }
                    performSearch(query, index, server, converter);
                }
            } else if (command == "word" || command == "w") {
                if (tokens.size() < 2) {
                    std::cout << "Error: Word required" << std::endl;
                    std::cout << "Usage: word <word>" << std::endl;
                } else {
                    showWordStats(tokens[1], index, documents);
                }
            } else if (command == "find" || command == "f") {
                if (tokens.size() < 2) {
                    std::cout << "Error: Word required" << std::endl;
                    std::cout << "Usage: find <word> [limit]" << std::endl;
                } else {
                    int limit = -1;
                    if (tokens.size() > 2) {
                        try {
                            limit = std::stoi(tokens[2]);
                        } catch (...) {
                            std::cout << "Warning: Invalid limit format, showing all results" << std::endl;
                        }
                    }
                    findWordInDocuments(tokens[1], index, documents, limit);
                }
            } else if (command == "compare" || command == "c") {
                if (tokens.size() < 3) {
                    std::cout << "Error: Two words required for comparison" << std::endl;
                    std::cout << "Usage: compare <word1> <word2>" << std::endl;
                } else {
                    compareWords(tokens[1], tokens[2], index);
                }
            } else if (command == "stats") {
                showStats(index);
            } else if (command == "process") {
                processAllRequests(converter, server);
            } else {
                std::cout << "Unknown command: " << command << std::endl;
                std::cout << "Type 'help' for a list of commands" << std::endl;
            }
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}