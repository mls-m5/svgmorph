#include "ease.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Coord {
    float x = 0;
    float y = 0;

    bool operator<(const Coord &other) {
        return y < other.y;
    }
};

std::vector<Coord> createCoords(EaseType type) {
    auto f = ease(type);
    int len = 50;
    auto ret = std::vector<Coord>{};
    for (float x = 0.f; x <= 1.f; x += 1.f / len) {
        ret.push_back({
            .x = x,
            .y = f(x),
        });
    }

    return ret;
}

void showGraph(EaseType type) {
    int width = 50;
    for (auto coord : createCoords(type)) {
        std::cout << std::setfill(' ')
                  << std::setw(static_cast<int>(coord.y * width + 1.5f))
                  << "x\n";
    }
}

std::string replace(std::string content,
                    std::string_view substr,
                    std::string_view replacement) {
    for (std::string::size_type f = 0;
         f = content.find(substr, f), f != std::string::npos;) {
        content.replace(f, substr.size(), replacement);
        f += replacement.size();
    }

    return content;
}

std::string readFile(std::filesystem::path path) {
    auto file = std::ifstream{path};
    auto ss = std::stringstream{};
    ss << file.rdbuf();
    return ss.str();
}

std::string createString(const std::vector<Coord> &coordinates) {
    auto ss = std::ostringstream{};
    for (auto coord : coordinates) {
        ss << " " << coord.x << "," << coord.y;
    }
    return ss.str();
}

std::filesystem::path createGraph(EaseType type, int n) {
    auto svg = readFile("data/graphtemplate.svg");

    auto coords = createCoords(type);
    auto coordStr = createString(coords);

    auto min = 0.f;
    auto max = 0.f;

    for (auto c : coords) {
        min = std::min(c.y, min);
        max = std::max(c.y, max);
    }

    auto height = max - min;

    const auto values = std::vector<std::pair<std::string, std::string>>{
        {"width", std::to_string(10)},
        {"height", std::to_string(static_cast<int>(10 * height + 1))},
        {"min", std::to_string(min)},
        {"max", std::to_string(max)},
        {"line", std::move(coordStr)},
    };

    for (auto &v : values) {
        svg = replace(svg, "{" + v.first + "}", v.second);
    }

    auto path = "graph-out" + std::to_string(n) + ".svg";

    std::ofstream{path} << svg;
    //    std::cout << svg;

    return path;
}

int main(int argc, char *argv[]) {

    auto paths = std::vector<std::filesystem::path>{};

    for (auto type : {
             EaseType::Linear,
             EaseType::Sine,
             EaseType::Exp,
             EaseType::Elastic,
             EaseType::Bouncy,
         }) {
        std::cout << "type " << static_cast<int>(type) << "\n";

        showGraph(type);
        auto path = createGraph(type, static_cast<int>(type));
        paths.push_back(path);
    }

    auto futures = std::vector<std::future<void>>{};

    for (size_t i = 1; i < paths.size(); ++i) {
        auto a = paths.at(i - 1);
        auto b = paths.at(i);
        auto ss = std::ostringstream{};
        ss << "./svgmorph --subframes 30";
        ss << " --ease " << typeToStr(static_cast<EaseType>(i));
        ss << " " << a << " " << b;
        auto command = ss.str();
        std::cout << command << std::endl;

        futures.push_back(
            std::async([command] { std::system(command.c_str()); }));
    }
    return 0;
}
