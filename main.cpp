#include "jsonbuffet.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <sstream>

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
       tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " /path/to/item/of/interest < /path/to/json" << std::endl;
        std::cerr << "Note: You can use empty path components to represent an element of an array." << std::endl;
        std::cerr << R"(eg. item//key/ on  { "item": [ {"key": "value1"}, {"key": "value2"} ] } fetches value1 and value2)" << std::endl;
        return -1;
    }

    rapidjson::IStreamWrapper isw(std::cin);
    JsonBuffet buffet([](rapidjson::SizeType offset, rapidjson::SizeType length, const std::string& key, const RapidJsonValue& value) -> bool {
        std::cout << "Offset: " << offset << std::endl;
        std::cout << "Length: " << length << std::endl;
        std::cout << value << std::endl;
        return true;
    },
    split(argv[1], '/'));
    buffet.Consume(isw);

    return 0;
}
