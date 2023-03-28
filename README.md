# Spreader

A fast, portable spreadsheet logic library

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-BSD-brightgreen.svg)](https://opensource.org/licenses/BSD-3-Clause)


Spreader is a portable C++ library that implements spreadsheet logic - reading and writing data and formulas into cells, automatic recalculation, copying and moving cells, adding and deleting rows and columns and so on. It does not implement any spreadsheet UI,
though the hope is that eventually it will be possible to write a UI based on it. 
It is currently in alpha stage.

Wrapper libraries in JavaScript and Python are available at [spreader.js](https://github.com/gershnik/spreader.js) and 
[spreader.py](https://github.com/gershnik/spreader.py) respectively.

## Build

You will need CMake 3.24 or better as well as C++ compiler that supports C++20. The following compilers are known to work
- GCC 11.3 or above
- Xcode 14 or above 
- Visual Studio 2022 or above 

To get the library as a dependency:

```cmake
include(FetchContent)
FetchContent_Declare(spreader
    GIT_REPOSITORY git@github.com:gershnik/spreader.git
    GIT_TAG        <desired tag like v1.2 or commit sha>
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(spreader)
```

If you have Bison 3.8 or better and/or Flex 2.6 or better available on your machine CMake build will use them to generate code 
for formula parser. Otherwise it will use pre-built files.

## Quick start

```cpp

#include <spreader/sheet.h>

using namespace Spreader;

//Create a new sheet
auto sheet = Sheet();
//Set A2 cell to contain plain value. 
sheet.setValueCell(Point{.x=0, .y=1}, "Hello ");
//You can also use regular cell notation. 
//The result of parsePoint is std::optional<Point>
sheet.setValueCell(sheet.parsePoint("B3").value(), "World!");
//Set a cell to contain formula
sheet.setFormulaCell(Point{.x=0, .y=0}, "A2 & B3");
//Read calculated formula result
auto val = s.getValue(Point{.x=0, .y=0});
//prints "Hello Wolrd!" 
std::cout << val;
```








