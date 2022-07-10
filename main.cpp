#include <cstdlib>
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson {
typedef ::std::size_t SizeType;
}
#include <csignal>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <vector>

uint8_t max_pre_depth = 2; // Save json from pre_depth 3

struct ExtractionMarker {
  ExtractionMarker() {}
  size_t offset_start;
  size_t offset_end;
};

typedef size_t OFFSET_START;
typedef size_t OFFSET_END;
typedef size_t DEPTH;

struct MyHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler> {
  uint64_t open_object_count_ = 0;
  uint64_t open_array_count_ = 0;
  bool found_term = false;
  char *search_term;
  std::string result;

  std::map<DEPTH, std::vector<OFFSET_START>> depth_markers;
  std::map<OFFSET_START, OFFSET_END> extract_markers;
  /*
    { 0 <-- find the last 0 position and use that as map

    } 0
  */

  // <Starting Offset Position, Ending Offset Position>
  std::vector<OFFSET_START> extraction_markers;

  MyHandler(char *_search_term) { search_term = _search_term; }

  OFFSET_START fetch_extraction_marker(size_t depth) {
    auto it = depth_markers.find(depth);
    if (it == depth_markers.end()) { // no object open found at this depth
    //   std::cerr << "ASDKOLANSDLKASDNLKASD" << depth << " ASDASDASDASD"
    //             << std::endl;
      return 0; // TODO: Handle with pointer logic
    } else {
      auto em = it->second.back();
    //   std::cerr << "ASDKOLANSDLKASDNLKASD  " << depth << " ASDASDASDASD  " << em
    //             << std::endl;
      return em;
    }
  }

  void add_extraction_marker(OFFSET_START e) {

    if (!(find(extraction_markers.begin(), extraction_markers.end(), e) !=
          extraction_markers.end())) {
      extraction_markers.push_back(e);
      render_vector(extraction_markers);
    }
  }

  void start_object_offset(OFFSET_START offset_start) {

    auto it = depth_markers.find(open_object_count_);
    if (it == depth_markers.end()) { // no object open found at this depth
      std::vector<OFFSET_START> object_markers;
      object_markers.push_back(offset_start);
      depth_markers.insert(std::make_pair(open_object_count_, object_markers));
    } else {
      std::vector<OFFSET_START> object_markers = it->second;
      object_markers.push_back(offset_start);
      it->second = object_markers;
    }
  }

  void end_object_offset(OFFSET_END offset_end) {

    auto it = depth_markers.find(open_object_count_);
    std::cerr << "DEPTH ADDING: " << open_object_count_ << std::endl;
    if (it == depth_markers.end()) { // no object open found at this depth
      std::cerr << "SHOULDN'T HAPPEN for " << open_object_count_ << std::endl;
    } else {

      OFFSET_START object_start = it->second.back();
      std::cerr << "DEPTH ADDING: " << open_object_count_ << " " << object_start
                << " " << offset_end << std::endl;
      extract_markers.insert(std::make_pair(object_start, offset_end + 1));
    }
  }

  bool StartObject(size_t offset) {

    start_object_offset(offset + 1);
    open_object_count_++;
    std::cerr << "StartObject(" << open_object_count_ << ") at " << offset
              << std::endl;
    return true;
  }

  bool EndObject(size_t offset, rapidjson::SizeType memberCount) {

    --open_object_count_;
    end_object_offset(offset + 1);
    // render_map(depth_markers);
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
      size_t depth = open_object_count_;
      size_t parent = 0;
      if (depth > max_pre_depth) {
        parent = depth - max_pre_depth;
      }
      auto e = fetch_extraction_marker(parent);
      std::cerr << e << std::endl;
      add_extraction_marker(e);

      std::cerr << "FOUND: " << str << "\nDEPTH: Obj(" << depth << ")"
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
    if (strcmp(search_term, str) == 0) {
      found_term = true; // do this in worker so we can do multiple terms later
      size_t depth = open_object_count_;
      size_t parent = 0;
      if (depth > max_pre_depth) {
        parent = depth - max_pre_depth;
      }
      auto e = fetch_extraction_marker(parent);
      std::cerr << e << std::endl;
      add_extraction_marker(e);

      std::cerr << "FOUND: " << str << "\nDEPTH: Obj(" << depth << ")"
                << std::endl
                << depth_markers.size() << std::endl;

      //   extraction_markers.insert(
      //       std::make_pair(depth, &object_markers.at(depth)));
    }
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

  void render_map(std::map<size_t, std::vector<OFFSET_START>> mm) {

    for (auto it = mm.begin(); it != mm.end(); ++it) {
      std::cerr << it->first << " => ";
      render_vector(it->second);
      std::cerr << std::endl;
    }
  }

  void render_vector_ptr(std::vector<OFFSET_START *> markers) {
    std::cerr << "[" << std::endl;
    for (auto it = markers.begin(); it != markers.end(); ++it) {

      auto em = *it;
      printf("Address of x is %p\n", (void *)em);
      std::cerr << "(" << *em << ")" << std::endl;
    }

    std::cerr << "]" << std::endl;
  }

  void render_vector(std::vector<OFFSET_START> markers) {
    std::cerr << "[" << std::endl;
    for (auto it = markers.begin(); it != markers.end(); ++it) {

      auto em = *it;
      std::cerr << "(" << em << ")" << std::endl;
    }

    std::cerr << "]" << std::endl;
  }

  OFFSET_END fetch_offset_end(OFFSET_START offset_start) {
    auto it = extract_markers.find(offset_start);

    if (it != extract_markers.end()) { // no object open found at this depth
      return it->second;
    }
    return 0;
  }

  void Result() {

    render_map(depth_markers);

    render_vector(extraction_markers);

    // extract JSON items from offset results
    std::cerr << "Extraction at contains (" << extraction_markers.size()
              << ")" << std::endl;

    for (auto offset_start_ptr : extraction_markers) {
      auto offset_start = offset_start_ptr;
      auto offset_end = fetch_offset_end(offset_start);
      std::cout << "" << offset_start << ", "
                << offset_end - offset_start <<  std::endl;
    }
    std::cout << std::endl;
  }
};


MyHandler* HANDLER = NULL;

void signalHandler( int signum ) {
   std::cerr << "Interrupt signal (" << signum << ") received. Printing result early" << std::endl;

   // cleanup and close up stuff here  
   // terminate program  
   if (HANDLER != NULL )
   HANDLER->Result();

   exit(signum);  
}



int main(int argc, char *argv[]) {
  // std::cerr << argc << std::endl;
  char *search_term = (char *)"7777777777";
  if (argc >= 2) {
    search_term = argv[1];
  }
  if (argc >= 2) {
    max_pre_depth = atoi(argv[2]);
  }
  rapidjson::IStreamWrapper isw(std::cin);
  MyHandler handler((char *)search_term);
  HANDLER = &handler;
  rapidjson::Reader reader;

  signal(SIGINT, signalHandler);  
  // wait
  reader.Parse<rapidjson::kParseIterativeFlag |
               rapidjson::kParseNumbersAsStringsFlag>(isw, handler);

 

  handler.Result();

  return 0;
}
