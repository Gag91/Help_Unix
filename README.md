# Help_Unix

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++26](https://img.shields.io/badge/C%2B%2B-26-blue.svg)](https://isocpp.org/)
[![Windows](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/)
[![Build System](https://img.shields.io/badge/BuildSystem-HelpMake-blue)](https://github.com/Xavi99/Help_Make)

A simple cat clone in c++

## Use
```bash
hu file.md file.txt --verbose
```

## Option
- -h, --help         Outputs the help message
- -v, --version      Outputs the program version 
- -l, --license      Outputs the program license
- --verbose          Outputs extra information

## Requirements
- C++26

# Compiling
- Using [HelpMake](https://github.com/Gag91/Help_Make)
```bash
hm -b
```

- Using Gcc
```bash
g++ -std=c++26  src/main.cpp -ohu.exe  -Iinclude -lstdc++exp
```