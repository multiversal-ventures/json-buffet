#include <unordered_map>
#include <unordered_set>
#include "jsonbuffet.h"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

typedef std::pair<rapidjson::SizeType, rapidjson::SizeType> ByteRange;

int main(int argc, char *argv[]) {
    if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " < /path/to/test.json" << std::endl;
        std::cerr << "Note: This is strictly a static program meant to parse json files like tests/test.json" << std::endl;
        return -1;
    }

    std::unordered_map<std::string, std::unordered_set<ByteRange, pair_hash>> npi_index;

    rapidjson::IStreamWrapper isw(std::cin);
    JsonBuffet buffet([&npi_index](rapidjson::SizeType offset, rapidjson::SizeType length, const std::string& key, const RapidJsonValue& value) -> bool {
        auto provider_groups = value.FindMember("provider_groups");
        if (provider_groups != value.MemberEnd()) {
            auto provider_groups_array = provider_groups->value.GetArray();
            for (auto provider = provider_groups_array.Begin(); provider != provider_groups_array.End(); provider++) {
                auto npi = provider->FindMember("npi");
                if (npi != provider->MemberEnd()) {
                    auto npi_array = npi->value.GetArray();

                    for (auto item = npi_array.Begin(); item != npi_array.End(); item++) {
                        auto key = std::string(item->GetString());
                        auto it = npi_index.find(key);
                        if (it != npi_index.end()) {
                            it->second.insert({offset, length});
                        } else {
                            npi_index[key] = {{offset, length}};
                        }
                    }
                }
            }
        }

        return true;
    },
    {"in_network", "", "negotiated_rates",""});
    buffet.Consume(isw);

    // Print out the CSV of the <npi index>, (start, length), (start, length),
    for (auto const& npi_item: npi_index) {
        for (const auto& range: npi_item.second) {
            std::cout << npi_item.first << "," << std::get<0>(range) << "," << std::get<1>(range) << std::endl;
        }
    }
    return 0;
}
