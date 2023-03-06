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

Just pipe any json file as input to json-buffet and provide it with the path you are interested in.

```
$ ./json-buffet
Usage: ./json-buffet /path/to/item/of/interest < /path/to/json
Note: You can use empty path components to represent an element of an array.
eg. item//key/ on  { "item": [ {"key": "value1"}, {"key": "value2"} ] } fetches value1 and value2

$ ./json-buffet in_network// < ../tests/test.json
Offset: 260
Length: 1824
{negotiation_arrangement: bundle, name: Total Knee Replacement, billing_code_type: ICD, billing_code_type_version: 9, billing_code: 81.54, description: Total Knee Replacement, negotiated_rates: [{provider_groups: [{npi: [1111111111, 2222222222, 3333333333, 4444444444, 5555555555], tin: {type: ein, value: 11-1111111}}, {npi: [1111111111, 2222222222, 3333333333, 4444444444, 5555555555], tin: {type: ein, value: 22-2222222}}], negotiated_prices: [{negotiated_type: negotiated, negotiated_rate: 20000.00, expiration_date: 2022-01-01, service_code: [05, 06, 07], billing_class: professional}]}, {provider_groups: [{npi: [6666666666, 7777777777, 8888888888, 9999999999, 4444444444], tin: {type: npi, value: 6666666666}}], negotiated_prices: [{negotiated_type: negotiated, negotiated_rate: 25000.00, expiration_date: 2022-01-01, service_code: [05, 06, 07], billing_class: professional}]}], bundled_codes: [{billing_code_type: CPT, billing_code_type_version: 2020, billing_code: 27447, description: Under Repair, Revision, and/or Reconstruction Procedures on the Femur (Thigh Region) and Knee Joint}, {billing_code_type: CPT, billing_code_type_version: 2020, billing_code: 27446, description: Under Repair, Revision, and/or Reconstruction Procedures on the Femur (Thigh Region) and Knee Joint}]}

$ ./json-buffet in_network//negotiated_rates// < ./tests/test.json
Offset: 510
Length: 619
{provider_groups: [{npi: [1111111111, 2222222222, 3333333333, 4444444444, 5555555555], tin: {type: ein, value: 11-1111111}}, {npi: [1111111111, 2222222222, 3333333333, 4444444444, 5555555555], tin: {type: ein, value: 22-2222222}}], negotiated_prices: [{negotiated_type: negotiated, negotiated_rate: 20000.00, expiration_date: 2022-01-01, service_code: [05, 06, 07], billing_class: professional}]}
Offset: 1130
Length: 447
{provider_groups: [{npi: [6666666666, 7777777777, 8888888888, 9999999999, 4444444444], tin: {type: npi, value: 6666666666}}], negotiated_prices: [{negotiated_type: negotiated, negotiated_rate: 25000.00, expiration_date: 2022-01-01, service_code: [05, 06, 07], billing_class: professional}]}

$ ./json-buffet in_network//negotiated_rates//provider_groups//tin < ./tests/test.json
Offset: 632
Length: 68
{type: ein, value: 11-1111111}
Offset: 803
Length: 68
{type: ein, value: 22-2222222}
Offset: 1251
Length: 68
{type: npi, value: 6666666666}
```

Additional Binaries provided:

* json-chunker:
       Extracts chunks of objects at the specified path into separate files.
       Usage: ./json-chunker[chunksize_threshold=104857600] /path/to/item/of/interest < /path/to/json
       Note: You can use empty path components to represent an element of an array.
       eg. item//key/ on  { "item": [ {"key": "value1"}, {"key": "value2"} ] } creates chunks containing value1 value2

* json-indexer:
       Processes json files like tests/test.json to print out an CSV index of npi values: <npi index>, start, length ...
       Usage: `curl https://path/to/big_json.json | ./json-indexer > index.db`
       You can then use something like: `python ./json-fetcher --db index.db --url http://path/to/big_json.json --key "1111111111"`
       Please note that your server needs to support http range requests for json-fetcher to work

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.


## TODO
[ ] Support Regex paths to allow watching for multiple paths
[ ] Refactor the JsonBuffet code to use iterators instead of callbacks
