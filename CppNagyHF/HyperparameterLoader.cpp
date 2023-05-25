#include "HyperparameterLoader.hpp"

#include <iostream>
#include <fstream>
#include <exception>

namespace model {

    NeatTrainer model::HyperparameterLoader::operator()(const std::string& fileName) const
    {
        
        std::ifstream file(fileName);

        if (!file) {
            std::cerr << "ERROR: file not found!" << std::endl;
            throw std::exception("File not found!");
        }

        bool inName = false;
        bool inValue = false;
        bool inValueString = false;

        std::unordered_map<std::string, std::string> JSONMap;

        std::vector<std::string> nameStack;
        std::string name;
        std::string value;

        auto processValue = [&]() {

            std::string key; // will look like outer.inner.inner2.value

            for (const auto& stackItem : nameStack)
                key += stackItem + ".";
            key += name;

            std::cout << "JSON[" << key << "]=" << value << std::endl;

            JSONMap[key] = value;

            value = "";
            name = "";
        };

        while (file.eof() == false) {

            char c = file.get();

            if (inValue == false && inName == false && std::isspace(c))
                continue;

            std::cout << "Handling: " << c << std::endl;

            if (c == ':') {
                inValue = true;
                value = "";
                continue;
            }

            if (c == '}') {
                inValue = false;

                if (value != "")
                    processValue();

                if (nameStack.size() > 0) {
                    name = nameStack.back();
                    nameStack.pop_back();
                }
                continue;
            }

            if (c == '{') {
                inValue = false;
                if (name != "") {
                    nameStack.push_back(name);
                    name = "";
                }
                continue;
            }

            if (c == '\"') {
                if (inValue && inValueString == false) {
                    inValueString = true;
                    value += '"';
                    continue;
                }

                if (inValue && inValueString) {
                    inValueString = false;
                    inValue = false;
                    value += '"';

                    processValue();
                    continue;
                }

                if (inName) {
                    inName = false;
                    continue;
                }

                // not in value, not in name --> start of name
                inName = true;
                continue;
            }

            if (inValue && inValueString == false && c == ',') {
                inValue = false;

                if (value != "")
                    processValue();

                value = "";
            }

            if (inValue == false && c == ',') {
                value = "";
                name = "";
                continue;
            }

            if (inName) {
                name += c;
                continue;
            }

            if (inValue) {
                value += c;
                continue;
            }

        }

        return NeatTrainer();
    }

    std::string HyperparameterLoader::Trim(const std::string& str)
    {
        size_t startIndex = 0;
        size_t endIndex = 0;

        while (std::isspace(str[startIndex]))
            startIndex++;

        while (std::isspace(str[endIndex - 1]) && endIndex > 0)
            endIndex--;

        if (endIndex == 0)
            return "";

        return str.substr(startIndex, endIndex - startIndex);
    }

    template <>
    std::optional<double> HyperparameterLoader::Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
        if (JSONMap.find(key) == JSONMap.end())
            return {};

        return std::stod(Trim(JSONMap.at(key)));
    }

    template <>
    std::optional<long long> HyperparameterLoader::Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
        if (JSONMap.find(key) == JSONMap.end())
            return {};

        return std::stoll(Trim(JSONMap.at(key)));
    }

    template <>
    std::optional<unsigned long long> HyperparameterLoader::Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
        if (JSONMap.find(key) == JSONMap.end())
            return {};

        return std::stoull(Trim(JSONMap.at(key)));
    }

    template <>
    std::optional<std::string> HyperparameterLoader::Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
        if (JSONMap.find(key) == JSONMap.end())
            return {};

        auto value = Trim(JSONMap.at(key));

        if (value == "")
            return {};

        if (value[0] != '\"')
            return {};

        if (value.back() != '\"')
            return {};

        return value.substr(1, value.size() - 2);
    }

    template <>
    std::optional<bool> HyperparameterLoader::Get(const std::unordered_map<std::string, std::string>& JSONMap, const std::string& key) {
        if (JSONMap.find(key) == JSONMap.end())
            return {};

        auto value = Trim(JSONMap.at(key));

        if (value == "true")
            return true;
        if (value == "false")
            return false;

        return {};
    }
}
