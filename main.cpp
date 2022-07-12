#include <cstdlib>
#include <string>
#define RAPIDJSON_NO_SIZETYPEDEFINE
    namespace rapidjson { typedef ::std::size_t SizeType; }
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

typedef rapidjson::GenericValue<rapidjson::UTF8<char>> RapidJsonValue;
typedef std::function<bool(rapidjson::SizeType, rapidjson::SizeType, const std::string&, const RapidJsonValue&)> Callback;

std::ostream& operator<<(std::ostream &stream, const std::vector<std::string>& vec) {
    stream << "[";
    for (const auto& item : vec) {
        stream << item << ", ";
    }
    stream << "]";
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const RapidJsonValue& value) {
    if (value.IsString()) {
        stream << value.GetString();
    } else if (value.IsBool()) {
        stream << std::boolalpha << value.GetBool();
    } else if (value.IsUint()) {
        stream << value.GetInt();
    } else if (value.IsInt()) {
        stream << value.GetInt();
    } else if (value.IsUint64()) {
        stream << value.GetUint64();
    } else if (value.IsInt64()) {
        stream << value.GetInt64();
    } else if (value.IsFloat() || value.IsLosslessFloat()) {
        stream << value.GetFloat();
    } else if (value.IsDouble() || value.IsLosslessDouble()) {
        stream << value.GetDouble();
    } else if (value.IsArray()) {
        stream << "[";
        for (auto currentValue = value.Begin(); currentValue != value.End(); currentValue++) {
            stream << *currentValue;
            stream << ", ";
        }
        stream << "]";
    } else if (value.IsObject()){
        stream << "{";
        for (auto member = value.MemberBegin(); member != value.MemberEnd(); member++) {
            stream << member->name.GetString() << ": ";
            stream << member->value;
            stream << ", ";
        }
        stream << "}";
    } else {
        stream << "!!!!Invalid json value!!!!";
    }

    return stream;
}


class JsonBuffet: public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, JsonBuffet> {
public:
    JsonBuffet(Callback callback, std::vector<std::string> targetPath = {})
        : mCallback(callback),
          mTargetPath(targetPath)
    {
    }

    bool Key(rapidjson::SizeType offset, const char* str, rapidjson::SizeType length, bool copy) {
        mCurrentPath.back() = std::string(str, length);
        return true;
    }

    bool String(rapidjson::SizeType offset, const Ch* str, rapidjson::SizeType length, bool copy) {
        if (isCollectingResult()) {
            return collectResult(RapidJsonValue(str, length, mCurrentResult.back().GetAllocator()));
        } else if (isCurrentPathTarget()) {
            return mCallback(offset, length, mCurrentPath.back(), RapidJsonValue(str, length));
        } else {
            // Do nothing
        }
        return true;
    }

    bool RawNumber(rapidjson::SizeType offset, const Ch* str, rapidjson::SizeType length, bool copy) {
        return String(offset, str, length, copy);
    }

    bool StartObject(rapidjson::SizeType offset) {
        // We want to collect all items/subitems once we match a path
//        std::cout << "Start Object @ Current Path: " << mCurrentPath << std::endl;
        if (isCurrentPathTarget() || isCollectingResult()) {
            mCurrentResult.push_back(rapidjson::Document());
            mCurrentResult.back().SetObject();
        }

        mCurrentPathOffsets.push_back(offset);
        mCurrentPath.push_back("");
        return true;
    }

    bool EndObject(rapidjson::SizeType offset, rapidjson::SizeType memberCount) {
        if (mCurrentPath.empty()) {
            std::cerr << "Malformed json: Trying to pop an empty path at " << offset << std::endl;
            return false;
        }

//        std::cout << "End Object @ Current Path: " << mCurrentPath << std::endl;
        auto currentPath = std::move(mCurrentPath.back());
        mCurrentPath.pop_back();

        if (isCollectingResult()) {
            auto currentValue = std::move(mCurrentResult.back());
            mCurrentResult.pop_back();

            if (mCurrentResult.empty()) {
                auto start_offset = mCurrentPathOffsets.back();
                auto length = offset - start_offset + 1;
                mCallback(start_offset, length, currentPath, currentValue);
            }
            else {
                collectResult(std::move(currentValue));
            }
        }

        mCurrentPathOffsets.pop_back();
        return true;
    }

    bool StartArray(rapidjson::SizeType offset) {
        // We want to collect all items/subitems once we match a path
//        std::cout << "Start Array @ Current Path: " << mCurrentPath << std::endl;
        if (isCurrentPathTarget() || isCollectingResult()) {
            mCurrentResult.push_back(rapidjson::Document());
            mCurrentResult.back().SetArray();
        }
        mCurrentPathOffsets.push_back(offset);
        mCurrentPath.push_back("");
        return true;
    }

    bool EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount) {
        if (mCurrentPath.empty()) {
            std::cerr << "Malformed json: Trying to pop an empty path at " << offset << std::endl;
            return false;
        }

//        std::cout << "End Array @ Current Path: " << mCurrentPath << std::endl;
        auto currentPath = std::move(mCurrentPath.back());
        mCurrentPath.pop_back();

        if (isCollectingResult()) {
            auto currentValue = std::move(mCurrentResult.back());
            mCurrentResult.pop_back();

            if (mCurrentResult.empty()) {
                auto start_offset = mCurrentPathOffsets.back();
                auto length = offset - start_offset + 1;
                mCallback(start_offset, length, currentPath, currentValue);
            }
            else {
                collectResult(std::move(currentValue));
            }
        }

        mCurrentPathOffsets.pop_back();
        return true;
    }

private:

    bool isCurrentPathTarget() const {
        if (mCurrentPath.size() != mTargetPath.size()) return false;

        for (int i = mTargetPath.size() - 1; i >= 0; i--) {
            if (mTargetPath[i] != mCurrentPath[i] && !mTargetPath[i].empty())
                return false;
        }

        return true;
    }

    bool isCollectingResult() const {
        return !mCurrentResult.empty();
    }

    bool collectResult(const RapidJsonValue& value) {
        if (mCurrentResult.back().IsArray()) {
            mCurrentResult.back().GetArray().PushBack(RapidJsonValue(value, mCurrentResult.back().GetAllocator()),
                                                      mCurrentResult.back().GetAllocator());
        } else if (mCurrentResult.back().IsObject()) {
            // TODO: See if we can avoid a copy of the key here
            mCurrentResult.back().AddMember(RapidJsonValue(mCurrentPath.back().data(), mCurrentPath.back().size(), mCurrentResult.back().GetAllocator()),
                                            RapidJsonValue(value, mCurrentResult.back().GetAllocator()),
                                            mCurrentResult.back().GetAllocator());
        } else {
            std::cerr << "Invalid operation: Trying to add a value to a non object/array member at path: " << mCurrentPath << " at result size: " << mCurrentResult.size() << std::endl;
            return false;
        }

        return true;
    }

private:
    Callback mCallback;
    std::vector<std::string> mTargetPath;

    std::vector<std::string> mCurrentPath;
    std::vector<rapidjson::SizeType> mCurrentPathOffsets;
    std::vector<rapidjson::Document> mCurrentResult;
};

int main(int argc, char *argv[]) {
  // std::cerr << argc << std::endl;
    rapidjson::IStreamWrapper isw(std::cin);
    JsonBuffet handler([](rapidjson::SizeType offset, rapidjson::SizeType length, const std::string& key, const RapidJsonValue& value) -> bool {
        auto provider_groups = value.FindMember("provider_groups");
        if (provider_groups != value.MemberEnd()) {
            auto provider_groups_array = provider_groups->value.GetArray();
            for (auto provider = provider_groups_array.Begin(); provider != provider_groups_array.End(); provider++) {
                auto npi = provider->FindMember("npi");
                if (npi != provider->MemberEnd()) {
                    auto npi_array = npi->value.GetArray();
                    bool targetFound = false;

                    for (auto item = npi_array.Begin(); item != npi_array.End(); item++) {
                        if (strcmp(item->GetString(), "7777777777") == 0) {
                            targetFound = true;
                            break;
                        }
                    }

                    if (targetFound) {
                        std::cout << "value: " << value << std::endl;
                        std::cout << "range: " << offset << "," << offset + length << std::endl;
                        break;
                    }
                }
            }
        }

        return true;
    },
    {"in_network", "", "negotiated_rates",""});
    rapidjson::Reader reader;
    reader.Parse<rapidjson::kParseIterativeFlag | rapidjson::kParseNumbersAsStringsFlag>(isw, handler);
    return 0;

  return 0;
}
