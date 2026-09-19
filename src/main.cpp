#include <cerrno>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <vector>

#include "hp/log/logger.hpp"

class Cat {
  private:
    hp::Logger logger;
    bool verbose;

  public:
    Cat(bool verbose) : verbose(verbose) {}

    template <typename T, typename... Args>
    T run(const Args &...rest) const {
        static_assert((std::is_convertible_v<Args, std::filesystem::path> && ...),
                      "run function argument must be convertible to std::filesystem::path!");

        using DecayedT = std::decay_t<T>;
        static_assert(std::is_same_v<DecayedT, bool> || std::is_same_v<DecayedT, std::vector<std::filesystem::path>>,
                      "run function template argument must be bool or std::vector<std::filesystem::path>");

        if (verbose) {
            std::cout << "\n";
            (logger.info(std::format("reading {}", rest.string())), ...);
        }

        char buffer[4096];
        std::vector<std::filesystem::path> failed_files;

        template for (const auto &file_path : {rest...}) {
            std::ifstream file(file_path, std::ios::binary);

            if (!file) [[unlikely]] {
                std::cout << "\n";
                logger.error(std::format("{}: {}\n", file_path.string(), std::strerror(errno)));

                if constexpr (std::is_same_v<DecayedT, std::vector<std::filesystem::path>>) {
                    failed_files.push_back(file_path);
                }
                continue;
            }

            while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
                std::cout.write(buffer, file.gcount());
            }
        }

        if constexpr (std::is_same_v<DecayedT, bool>) {
            return failed_files.empty();
        } else {
            return failed_files;
        }
    }
};

class Unix {
  private:
    hp::Logger logger;
    Cat cat;
    bool verbose;

  public:
    Unix(bool verbose) : verbose(verbose), cat(verbose) {}

    void help() const {
        std::cout << "Usage: hu <file>\n\n"
                  << "Concatenate a file to standard output.\n\n"
                  << "Options:\n\n"
                  << "  -h, --help         Outputs this help message\n"
                  << "  -v, --version      Outputs the program version\n"
                  << "  -l, --license      Outputs the program license\n"
                  << "      --verbose      Outputs extra information\n\n"
                  << "Examples:\n"
                  << "  hu file.txt  Output the contents of file.txt\n\n";
    }

    void version() const {
        std::cout << "Help_Unix (hu) - Unix Tools\n"
                  << "Version: 1.0.0\n"
                  << "Author : Xavi99\n"
                  << "Website: https://github.com\n"
                  << "This program is licensed under the MIT License. See \"--license\" for details.\n\n";
    }

    void license() const {
        std::cout << "Help_Unix (hu) - License Information\n"
                  << R"(MIT License

Copyright (c) 2026 Xavi99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)" << "\n\n";
    }

    template <typename... Args>
    bool run(const Args &...files) const {
        return cat.template run<bool>(files...);
    }
};

int main(int argc, char *argv[]) {
    bool verbose = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--verbose") {
            verbose = true;
        }
    }

    Unix ux(verbose);

    if (argc < 2) {
        ux.help();
        return 0;
    }

    std::string_view argument = argv[1];

    if (argument == "--help" || argument == "-h") {
        ux.help();
        return 0;
    }
    if (argument == "--version" || argument == "-v") {
        ux.version();
        return 0;
    }
    if (argument == "--license" || argument == "-l") {
        ux.license();
        return 0;
    }

    std::vector<std::filesystem::path> files;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--verbose")
            continue;
        files.emplace_back(arg);
    }

    if (files.empty()) {
        ux.help();
        return 0;
    }

    bool all_succeeded = true;
    for (const auto &f : files) {
        if (!ux.run(f)) {
            all_succeeded = false;
        }
    }

    return all_succeeded ? 0 : EXIT_FAILURE;
}
