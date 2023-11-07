#include "ease.h"
#include <iomanip>
#include <iostream>

void showGraph(EaseType type) {
    auto f = ease(type);

    int len = 50;
    int width = 50;

    std::cout << "0" << std::setfill('-')
              << std::setw(static_cast<int>(width - 1)) << "+\n";
    for (float x = 0.f; x <= 1.f; x += 1.f / len) {
        std::cout << std::setfill(' ')
                  << std::setw(static_cast<int>(f(x) * width + .5f)) << "x\n";
    }
}

int main(int argc, char *argv[]) {
    for (auto type : {
             EaseType::Linear,
             EaseType::Sine,
             EaseType::Elastic,
             EaseType::Bouncy,
         }) {
        std::cout << "type " << static_cast<int>(type) << "\n";

        showGraph(type);
    }

    return 0;
}
