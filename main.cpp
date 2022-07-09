#include <cstdlib>
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson {
typedef ::std::size_t SizeType;
}
#include <fstream>
#include <iostream>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <vector>

const uint8_t max_pre_depth = 3; // Save json from pre_depth 3

struct ExtractionMarker {
  ExtractionMarker() {}
  size_t offset_start;
  size_t offset_end;
};

struct MyHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
  uint64_t open_object_count_ = 0;
  uint64_t open_array_count_ = 0;
  bool found_term = false;
  char *search_term;
  std::string result;

  std::map<size_t, std::vector<ExtractionMarker>> depth_markers;
  /*
    { 0 <-- find the last 0 position and use that as map

    } 0
  */

  // <Starting Offset Position, Ending Offset Position>
  std::map<size_t, ExtractionMarker *> extraction_markers;

  MyHandler(char *_search_term) { search_term = _search_term; }

  void start_object_offset(size_t offset) {

    ExtractionMarker em;
    // std::cout << "Adding object market at: " << object_markers.size() <<
    // std::endl;
    em.offset_start = offset;

    auto it = depth_markers.find(open_object_count_);
    if (it == depth_markers.end()) { // no object open found at this depth
      std::vector<ExtractionMarker> object_markers;
      object_markers.push_back(em);
      depth_markers.insert(std::make_pair(open_object_count_, object_markers));
    } else {
      std::vector<ExtractionMarker> object_markers = it->second;
      object_markers.push_back(em);
      depth_markers.insert(std::make_pair(open_object_count_, object_markers));
    }
  }

  void end_object_offset(size_t offset) {

    ExtractionMarker em;
    em.offset_start = offset;

    auto it = depth_markers.find(open_object_count_);
    if (it == depth_markers.end()) { // no object open found at this depth
      std::cerr << "SHOULDN'T HAPPEN for " << open_object_count_ << std::endl;
    } else {
      std::vector<ExtractionMarker> object_markers = it->second;
      object_markers.back().offset_end = offset;
        std::cout << "SETTING " << offset << " For " << object_markers.size()
              << std::endl;
        render_vector(object_markers);
      depth_markers.insert(std::make_pair(open_object_count_, object_markers));
          render_map(depth_markers);
    }
  }

  bool StartObject(size_t offset) {

    start_object_offset(offset);
    open_object_count_++;
    std::cerr << "StartObject(" << open_object_count_ << ") at " << offset
              << std::endl;
    return true;
  }

  bool EndObject(size_t offset, rapidjson::SizeType memberCount) {

    --open_object_count_;
    end_object_offset(offset);
        render_map(depth_markers);
    // object_markers.at(open_object_count_).offset_end = offset + 1;

    // Wait until If no other open objects. Tag this with the first
    // startedObject
    std::cerr << "EndObject(" << open_object_count_ << ", " << memberCount
              << ") at " << offset << std::endl;
    // If we have found the term, find the last
    return true;
  }

  bool Key(rapidjson::SizeType offset, const char *str,
           rapidjson::SizeType length, bool copy) {
    std::cerr << "Key(" << str << ", " << length << ", " << std::boolalpha
              << copy << ") at " << offset << std::endl;
    return true;
  }
  bool Bool(bool value) {
    std::cerr << "Bool(" << value << ")" << std::endl;
    return true;
  }
  bool Uint(unsigned int value) {
    std::cerr << "UInt(" << value << ")" << std::endl;
    return true;
  }
  bool Uint64(uint64_t value) {
    std::cerr << "UInt64(" << value << ")" << std::endl;
    return true;
  }
  bool Int(int value) {
    std::cerr << "Int(" << value << ")" << std::endl;
    return true;
  }
  bool Int64(int64_t value) {
    std::cerr << "Int64(" << value << ")" << std::endl;
    return true;
  }
  bool Double(double value) {
    std::cerr << "Double(" << value << ")" << std::endl;
    return true;
  }
  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {

    if (strcmp(search_term, str) == 0) {
      found_term = true; // do this in worker so we can do multiple terms later
      size_t depth = open_object_count_ - 1;
      std::cout << "FOUND: " << str << "\nDEPTH: Obj(" << depth << ")"
                << std::endl
                << depth_markers.size() << std::endl;
      //   extraction_markers.insert(
      //       std::make_pair(depth, &object_markers.at(depth)));
    }
    std::cerr << "Number(" << str << ", " << length << ", " << std::boolalpha
              << copy << ")" << std::endl;
    return true;
  }
  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    std::cerr << "String(" << str << ", " << length << ", " << std::boolalpha
              << copy << ")" << std::endl;
    return true;
  }

  bool StartArray(rapidjson::SizeType offset) {

    std::cerr << "StartArray(" << open_array_count_ << ") at " << offset
              << std::endl;
    open_array_count_++;

    return true;
  }
  bool EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount) {
    --open_array_count_;
    std::cerr << "EndArray(" << open_array_count_ << ", " << elementCount
              << ") at " << offset << std::endl;
    return true;
    return true;
  }

  void render_map(std::map<size_t, std::vector<ExtractionMarker>> mm) {

    for (auto it = mm.begin(); it != mm.end(); ++it) { 
      std::cout << it->first << " => ";
      render_vector(it->second);
      std::cout << std::endl;
    }

  }

  void render_vector(std::vector<ExtractionMarker> markers) {
    std::cout << "[" << std::endl;
     for (auto it = markers.begin(); it != markers.end(); ++it) {

        ExtractionMarker em = *it;
        std::cout << "(" << em.offset_start << " - " << em.offset_end << ")" << std::endl;
      }

    std::cout << "]" << std::endl;
      
  }

  void Result() {

    // extract JSON items from offset results
    std::cout << "Extraction at contains (" << extraction_markers.size()
              << "):";

    

    // std::map<size_t, ExtractionMarker *>::iterator it =
    //     extraction_markers.begin();
    // while (it != extraction_markers.end()) {
    //   ExtractionMarker *em = it->second;
    //   std::cout << "KKKKK(" << it->first << ")[" << em->offset_start << " - "
    //             << em->offset_end << "]" << std::endl;
    //   it++;
    // }
    std::cout << std::endl;
  }
};

int main(int argc, char *argv[]) {
  // std::cout << argc << std::endl;
  char *search_term = (char *)"7777777777";
  if (argc >= 2) {
    search_term = argv[1];
  }
  rapidjson::IStreamWrapper isw(std::cin);
  MyHandler handler((char *)search_term);
  rapidjson::Reader reader;
  // wait
  reader.Parse<rapidjson::kParseIterativeFlag |
               rapidjson::kParseNumbersAsStringsFlag>(isw, handler);

  handler.Result();

  return 0;
}
