#pragma once

#include <cstdlib>
#include <filesystem>
#include <random>

struct TmpPath : public std::filesystem::path {
    using path::path;

    TmpPath() {
        auto templateStr = std::string{"/tmp/svgmorph-XXXXX"};
        srand(time(0));
        auto gen = std::mt19937{std::random_device{}()};
        for (auto &c : templateStr) {
            if (c == 'X') {
                c = '0' + std::uniform_int_distribution<int>(0, 9)(gen);
            }
        }

        *this = templateStr;

        std::filesystem::create_directory(*this);
    }
};
