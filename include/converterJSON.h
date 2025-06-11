#pragma once

#include <string>
#include <vector>
#include <map>


class ConverterJSON {
public:
    ConverterJSON();


    std::vector<std::string> GetTextDocuments();


    int GetResponsesLimit();


    std::vector<std::string> GetRequests();


    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);


    bool ConfigFileExists() const;


    std::string GetName() const;


    std::string GetVersion() const;

private:
    std::string config_path = "config.json";
    std::string requests_path = "requests.json";
    std::string answers_path = "../answers.json";
    std::string name;
    std::string version;
    int max_responses;
    std::vector<std::string> file_paths;


    void ReadConfig();
};