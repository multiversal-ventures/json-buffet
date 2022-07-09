#include <cstdlib>
#define RAPIDJSON_NO_SIZETYPEDEFINE
    namespace rapidjson { typedef ::std::size_t SizeType; }
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <iostream>

struct MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
    uint64_t open_object_count_ = 0;
    uint64_t open_array_count_ = 0;
    bool found_term = false;

    bool StartObject(size_t offset) { 
        std::cout << "StartObject(" << open_object_count_++ <<") at " << offset << std::endl; return true; 
    }
    bool Key(rapidjson::SizeType offset, const char* str, rapidjson::SizeType length, bool copy) {
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ") at " << offset << std::endl;
        return true;
    }
    bool Bool(bool value) {
        std::cout << "Bool(" << value << ")" << std::endl;
        return true;
    }
    bool Uint(unsigned int value) {
        std::cout << "UInt(" << value << ")" << std::endl;
        return true;
    }
    bool Uint64(uint64_t value) {
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
    bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
        std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }
    bool String (const Ch *str, rapidjson::SizeType length, bool copy) {
        std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }
    bool EndObject(size_t offset, rapidjson::SizeType memberCount) {
        // Wait until If no other open objects. Tag this with the first startedObject
         std::cout << "EndObject(" << --open_object_count_ << ", "   << memberCount << ") at " << offset << std::endl; return true; 
    }
    bool StartArray(rapidjson::SizeType offset) { std::cout << "StartArray(" << open_array_count_++ << ") at " << offset << std::endl; return true; }
    bool EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount) {
        
         std::cout << "EndArray(" << --open_array_count_ << ", " << elementCount << ") at " << offset << std::endl; return true; 
    }
};

int main(int argc, char *argv[])
{
    rapidjson::IStreamWrapper isw(std::cin);
    MyHandler handler;
    rapidjson::Reader reader;
    // wait 
    reader.Parse<rapidjson::kParseIterativeFlag>(isw, handler);

    return 0;
}
