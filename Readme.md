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

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
