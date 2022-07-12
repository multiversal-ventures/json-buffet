# Json-buffet

json-buffet is a little tool to help parse streaming json. Directly view and pick only the values you wish to consume.

## Installation

You need a recent C++ toolchain and CMake to use json-buffet

```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
$ cat ../test.json | ./json-buffet
```

## Usage

Just pipe any json file as input to json-buffet.
JsonBuffet is the main class that you can use to get callbacks to paths/objects of your interest.


```C++
// Include the JsonBuffet class here
int main(int argc, char *argv[])
{
    rapidjson::IStreamWrapper isw(std::cin);
    JsonBuffet handler([](rapidjson::SizeType offset, rapidjson::SizeType length, const std::string& key, const RapidJsonValue& value) -> bool {
        std::cout << "value: " << value << std::endl;
        std::cout << "range: " << offset << "," << offset + length << std::endl;
        return true;
    },
    {"in_network", "", "negotiated_rates","", "provider_groups", "", "npi"});
    rapidjson::Reader reader;
    reader.Parse<rapidjson::kParseIterativeFlag | rapidjson::kParseNumbersAsStringsFlag>(isw, handler);
    return 0;
}
```

The Path components are either keys of an object/subobject or "" (empty string) to represent the elements of an Array.
For eg.

The path `{"in_network", "", "negotiated_rates","", "provider_groups", "", "npi"}` gives us an output like:

```
$ json-buffet < /path/to/test.json
value: [1111111111, 2222222222, 3333333333, 4444444444, 5555555555, ]
range: 555,615
....
```

The path `{"in_network", "", "negotiated_rates","", "provider_groups", "", "npi", ""}` gives us an output like:
```
value: 1111111111
range: 556,566
value: 2222222222
range: 568,578
value: 3333333333
range: 580,590
value: 4444444444
range: 592,602
value: 5555555555
range: 604,614
...
```

The path `{"in_network", "", "negotiated_rates","", "provider_groups", "", "tin"}` gives us an output like:
```
value: {type: ein, value: 11-1111111, }
range: 632,700
value: {type: ein, value: 22-2222222, }
range: 803,871
value: {type: npi, value: 6666666666, }
range: 1239,1307
...
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
