#include <fstream>
#include <iostream>
#include "jsonbuffet.h"
#include "utils.h"

std::string fileName(int i) {
    return "chunk _" + std::to_string(i) + ".json";
}

int main(int argc, char *argv[]) {
    std::vector<std::string> path;
    size_t chunkSizeThreshold = 104857600;

    if(argc == 2) {
        path = Utils::split(argv[1], '/');
    } else if (argc == 3) {
        chunkSizeThreshold = atoi(argv[1]);
        path = Utils::split(argv[2], '/');
    } else {
        std::cerr << "Usage: " << argv[0] << "[chunksize_threshold=104857600] /path/to/item/of/interest < /path/to/json" << std::endl;
        std::cerr << "Note: You can use empty path components to represent an element of an array." << std::endl;
        std::cerr << R"(eg. item//key/ on  { "item": [ {"key": "value1"}, {"key": "value2"} ] } creates chunks containing value1 value2)" << std::endl;
        return -1;
    }

    int currentChunk = 0;
    std::ofstream file;
    bool isFirstElement = true;
    file.open(fileName(currentChunk));
    file << "[ ";

    rapidjson::IStreamWrapper isw(std::cin);
    JsonBuffet buffet([&](rapidjson::SizeType, rapidjson::SizeType, const std::string&, const RapidJsonValue& value) -> bool {
                        if (file.tellp() > chunkSizeThreshold) {
                            std::cout << "Done writing chunk " << currentChunk << std::endl;
                            file << " ]";
                            file << std::flush;
                            file.close();
                            currentChunk++;
                            file.open(fileName(currentChunk));
                            file << "[ ";
                            isFirstElement = true;
                        }
                        std::stringstream buffer;
                        if (!isFirstElement) {
                            buffer << ", ";
                        }
                        buffer << value;
                        file << buffer.rdbuf();
                        isFirstElement = false;
                        return true;
                    },
                    path);
    buffet.Consume(isw);

    std::cout << "Done writing chunk " << currentChunk << std::endl;
    file << "]";
    file << std::flush;
    file.close();

    return 0;
}
