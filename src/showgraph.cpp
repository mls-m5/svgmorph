#include "ease.h"
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {

    auto y = std::vector<double>{.1, .4, .3, .4, 3};

    for (float x = 0.f; x <= 1; x += .1) {
        std::cout << std::setw(static_cast<int>(easeBouncy(x) * 20)) << ".\n";
    }

    return 0;
}
