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
    bool Bool(bool value) {
        std::cout << "Bool(" << value << ")" << std::endl;
        return true;
    }
    bool UInt(unsigned int value) {
        std::cout << "UInt(" << value << ")" << std::endl;
        return true;
    }
    bool UInt64(uint64_t value) {
        std::cout << "UInt64(" << value << ")" << std::endl;
        return true;
    }
    bool Int(int value) {
        std::cout << "Int(" << value << ")" << std::endl;
        return true;
    }
    bool Int64(int64_t value) {
        std::cout << "Int64(" << value << ")" << std::endl;
        return true;
    }
    bool Double(double value ) {
        std::cout << "Double(" << value << ")" << std::endl;
        return true;
    }
    bool 	RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
        std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }
      bool 	String (const Ch *str, rapidjson::SizeType length, bool copy) {
        std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
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
    reader.Parse<rapidjson::kParseIterativeFlag | rapidjson::kParseNumbersAsStringsFlag>(isw, handler);
    return 0;
}
