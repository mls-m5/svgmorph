#pragma once

#include <cstdlib>
#include <filesystem>

struct TmpPath : public std::filesystem::path {
    using path::path;

    TmpPath() {
        auto templateStr = std::string{"/tmp/svgmorph-XXXXX"};
        srand(time(0));
        for (auto &c : templateStr) {
            if (c == 'X') {
                c = '0' + rand() % 10;
            }
        }

        *this = templateStr;

        std::filesystem::create_directory(*this);
    }
};
