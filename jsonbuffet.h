#pragma once
#include <cstdlib>
#define RAPIDJSON_NO_SIZETYPEDEFINE
    namespace rapidjson { typedef ::std::size_t SizeType; }
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>

typedef rapidjson::GenericValue<rapidjson::UTF8<char>> RapidJsonValue;
typedef std::function<bool(rapidjson::SizeType, rapidjson::SizeType, const std::string&, const RapidJsonValue&)> Callback;

std::ostream& operator<<(std::ostream &stream, const std::vector<std::string>& vec);
std::ostream& operator<<(std::ostream &stream, const RapidJsonValue& value);

class JsonBuffet: public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, JsonBuffet> {
public:
    JsonBuffet(Callback callback, std::vector<std::string> targetPath = {})
        : mCallback(callback),
          mTargetPath(targetPath)
    {
    }

    template <typename InputStream>
    rapidjson::ParseResult Consume(InputStream& isw) {
        return mReader.Parse<rapidjson::kParseIterativeFlag | rapidjson::kParseNumbersAsStringsFlag>(isw, *this);
    }

public:
    // Annoying that this is part of our public API but we can live with this for now
    bool Key(rapidjson::SizeType offset, const char* str, rapidjson::SizeType length, bool copy);
    bool String(rapidjson::SizeType offset, const Ch* str, rapidjson::SizeType length, bool copy);
    bool RawNumber(rapidjson::SizeType offset, const Ch* str, rapidjson::SizeType length, bool copy);
    bool StartObject(rapidjson::SizeType offset);
    bool EndObject(rapidjson::SizeType offset, rapidjson::SizeType memberCount);
    bool StartArray(rapidjson::SizeType offset);
    bool EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount);

private:

    bool isCurrentPathTarget() const;
    bool isCollectingResult() const;
    bool collectResult(const RapidJsonValue& value);

private:
    Callback mCallback;
    std::vector<std::string> mTargetPath;

    rapidjson::Reader mReader;
    std::vector<std::string> mCurrentPath;
    std::vector<rapidjson::SizeType> mCurrentPathOffsets;
    std::vector<rapidjson::Document> mCurrentResult;
};
