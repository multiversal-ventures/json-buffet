#include <cstdlib>
#define RAPIDJSON_NO_SIZETYPEDEFINE
    namespace rapidjson { typedef ::std::size_t SizeType; }
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <iostream>

struct MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
    bool StartObject(size_t offset) { std::cout << "StartObject() at " << offset << std::endl; return true; }
    bool Key(rapidjson::SizeType offset, const char* str, rapidjson::SizeType length, bool copy) {
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ") at " << offset << std::endl;
        return true;
    }
    bool EndObject(size_t offset, rapidjson::SizeType memberCount) { std::cout << "EndObject(" << memberCount << ") at " << offset << std::endl; return true; }
    bool StartArray(rapidjson::SizeType offset) { std::cout << "StartArray() at " << offset << std::endl; return true; }
    bool EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount) { std::cout << "EndArray(" << elementCount << ") at " << offset << std::endl; return true; }
};

int main(int argc, char *argv[])
{
    rapidjson::IStreamWrapper isw(std::cin);
    MyHandler handler;
    rapidjson::Reader reader;
    reader.Parse<rapidjson::kParseIterativeFlag>(isw, handler);
    return 0;
}
