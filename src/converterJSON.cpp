#include "../include/ConverterJSON.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

ConverterJSON::ConverterJSON() {
    try {
        ReadConfig();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing ConverterJSON: " << e.what() << std::endl;
        throw;
    }
}

void ConverterJSON::ReadConfig() {
    std::string config_path = "../config.json";
    if (!fs::exists(config_path)) {
        throw std::runtime_error("config file is missing");
    }
    
    try {
        std::ifstream config_file(config_path);
        json config_data;
        config_file >> config_data;

        if (!config_data.contains("config") || config_data["config"].empty()) {
            throw std::runtime_error("config file is empty");
        }

        this->name = config_data["config"]["name"];
        this->version = config_data["config"]["version"];

        if (config_data["config"].contains("max_responses")) {
            this->max_responses = config_data["config"]["max_responses"];
        } else {
            this->max_responses = 5;
        }

        this->file_paths.clear();
        if (config_data.contains("files") && !config_data["files"].empty()) {
            for (const auto& file_path : config_data["files"]) {
                this->file_paths.push_back(file_path);
            }
        } else {
            std::cerr << "Warning: No files specified in config.json" << std::endl;
        }
        
    } catch (const json::exception& e) {
        throw std::runtime_error(std::string("JSON parsing error: ") + e.what());
    }
}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::vector<std::string> documents;
    
    for (const auto& file_path : this->file_paths) {
        try {
            if (!fs::exists(file_path)) {
                std::cerr << "Warning: File not found: " << file_path << std::endl;
                continue;
            }
            
            std::ifstream file(file_path);
            if (!file.is_open()) {
                std::cerr << "Warning: Unable to open file: " << file_path << std::endl;
                continue;
            }
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            documents.push_back(content);
            
        } catch (const std::exception& e) {
            std::cerr << "Error reading file " << file_path << ": " << e.what() << std::endl;
        }
    }
    
    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    return this->max_responses;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::vector<std::string> requests;
    
    if (!fs::exists(requests_path)) {
        std::cerr << "Warning: Requests file not found: " << requests_path << std::endl;
        return requests;
    }
    
    try {
        std::ifstream requests_file(requests_path);
        json requests_data;
        requests_file >> requests_data;
        
        if (!requests_data.contains("requests") || requests_data["requests"].empty()) {
            std::cerr << "Warning: No requests found in file" << std::endl;
            return requests;
        }
        
        for (const auto& request : requests_data["requests"]) {
            requests.push_back(request);
        }
        
    } catch (const json::exception& e) {
        std::cerr << "Error parsing requests.json: " << e.what() << std::endl;
    }
    
    return requests;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    json answers_json;
    answers_json["answers"] = json::object();
    
    for (size_t i = 0; i < answers.size(); ++i) {
        std::string requestId = "request" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1);
        
        if (answers[i].empty()) {
            answers_json["answers"][requestId]["result"] = false;
        } else {
            answers_json["answers"][requestId]["result"] = true;

            if (answers[i].size() > 1) {
                json relevance = json::array();
                
                for (const auto& [doc_id, rank] : answers[i]) {
                    json doc_info;
                    doc_info["docid"] = doc_id;
                    doc_info["rank"] = rank;
                    relevance.push_back(doc_info);
                }
                
                answers_json["answers"][requestId]["relevance"] = relevance;
            } else {
                answers_json["answers"][requestId]["docid"] = answers[i][0].first;
                answers_json["answers"][requestId]["rank"] = answers[i][0].second;
            }
        }
    }
    std::ofstream answers_file(answers_path);
    answers_file << std::setw(4) << answers_json << std::endl;
}

bool ConverterJSON::ConfigFileExists() const {
    return fs::exists(config_path);
}

std::string ConverterJSON::GetName() const {
    return this->name;
}

std::string ConverterJSON::GetVersion() const {
    return this->version;
}