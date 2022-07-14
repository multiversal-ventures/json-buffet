#include "jsonbuffet.h"

bool JsonBuffet::Key(rapidjson::SizeType offset, const char *str, rapidjson::SizeType length, bool copy) {
    mCurrentPath.back() = std::string(str, length);
    return true;
}

bool JsonBuffet::String(rapidjson::SizeType offset, const Ch *str, rapidjson::SizeType length, bool copy) {
    if (isCollectingResult()) {
        return collectResult(RapidJsonValue(str, length, mCurrentResult.back().GetAllocator()));
    } else if (isCurrentPathTarget()) {
        return mCallback(offset, length, mCurrentPath.back(), RapidJsonValue(str, length));
    } else {
        // Do nothing
    }
    return true;
}

bool JsonBuffet::RawNumber(rapidjson::SizeType offset, const Ch *str, rapidjson::SizeType length, bool copy) {
    return String(offset, str, length, copy);
}

bool JsonBuffet::StartObject(rapidjson::SizeType offset) {
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

bool JsonBuffet::EndObject(rapidjson::SizeType offset, rapidjson::SizeType memberCount) {
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

bool JsonBuffet::StartArray(rapidjson::SizeType offset) {
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

bool JsonBuffet::EndArray(rapidjson::SizeType offset, rapidjson::SizeType elementCount) {
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

bool JsonBuffet::isCurrentPathTarget() const {
    if (mCurrentPath.size() != mTargetPath.size()) return false;

    for (int i = mTargetPath.size() - 1; i >= 0; i--) {
        if (mTargetPath[i] != mCurrentPath[i] && !mTargetPath[i].empty())
            return false;
    }

    return true;
}

bool JsonBuffet::isCollectingResult() const {
    return !mCurrentResult.empty();
}

bool JsonBuffet::collectResult(const RapidJsonValue &value) {
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

std::ostream &operator<<(std::ostream &stream, const std::vector<std::string> &vec) {
    stream << "[";
    for (const auto& item : vec) {
        stream << item << ", ";
    }
    stream << "]";
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const RapidJsonValue &value) {
    if (value.IsString()) {
        stream << '"' << value.GetString() << '"';
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
    } else if (value.IsNull()) {
        stream << "null";
    } else if (value.IsArray()) {
        stream << "[";
        auto currentValue = value.Begin();
        if (currentValue != value.End()) {
            stream << *currentValue;
            currentValue++;
        }
        for (; currentValue != value.End(); currentValue++) {
            stream << ", ";
            stream << *currentValue;
        }
        stream << "]";
    } else if (value.IsObject()){
        stream << "{";
        auto member = value.MemberBegin();
        if (member != value.MemberEnd()) {
            stream << '"' << member->name.GetString() << "\": " << member->value;
            member++;
        }
        for (; member != value.MemberEnd(); member++) {
            stream << ", ";
            stream << '"' << member->name.GetString() << "\": " << member->value;
        }
        stream << "}";
    } else {
        stream << "!!!!Invalid json value!!!!";
    }

    return stream;
}
