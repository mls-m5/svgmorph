#include "ease.h" // 21
#include "tmppath.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
// block 5

constexpr auto helpStr = R"(
usage

./game [args]

options
--profile            Enable profiling
--subframes          Set number of frames to interpolate between
--out -o             Set output filename
--ease -e            Set ease function, linear, sine, elastic, bouncy // 24
)";
// end
// block 2

struct Settings {
    // block settingsinner 3
    std::vector<std::filesystem::path> files;
    int subframes = 10;
    std::filesystem::path outFilename = "";
    std::function<float(float)> ease = easeLinear; // 22
    // end settingsinner
    // block se 4

    Settings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);
            if (arg == "--help" || arg == "-h") {
                std::cout << helpStr << std::endl;
                std::exit(0);
            }
            else if (arg == "--subframes") {
                subframes = std::stoi(args.at(++i));
            }
            else if (arg == "--out" || arg == "-o") {
                outFilename = args.at(++i);
            }
            else if (arg == "--ease" || arg == "-e") {
                ease = ::ease(args.at(++i)); // 23
            }
            else {
                files.push_back(arg);
            }
        }
    }
    // end se
};
// end
// block read 7

std::string readFile(std::filesystem::path path) {
    auto file = std::ifstream{path};
    if (!file.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'\n";
        std::exit(1);
    }
    auto buffer = std::stringstream{};
    buffer << file.rdbuf();
    return buffer.str();
}
// end read
// block extract 9

// Regular expression for matching integers and floating-point numbers
const std::regex numberPattern(R"([-+]?[0-9]*\.?[0-9]+)");

std::vector<std::string> extractNumbers(const std::string &input) {
    auto numbersBegin =
        std::sregex_iterator{input.begin(), input.end(), numberPattern};
    auto numbersEnd = std::sregex_iterator{};

    auto numbers = std::vector<std::string>{};
    for (auto it = numbersBegin; it != numbersEnd; ++it) {
        numbers.push_back(it->str());
    }

    return numbers;
}
// end extract
// block 17

std::string replaceNumbers(const std::string &text,
                           const std::vector<std::string> &replacements) {

    auto result = std::string{};
    auto replacementsIterator = replacements.begin();
    auto searchStart = text.cbegin();
    auto match = std::smatch{};

    while (std::regex_search(searchStart, text.cend(), match, numberPattern)) {
        result += match.prefix().str(); // Add the text before the match
        if (replacementsIterator != replacements.end()) {
            result += *replacementsIterator++;
        }
        else {
            result += match.str();
        }
        searchStart = match.suffix().first;
    }

    result += std::string(searchStart, text.cend());
    return result;
}
// end
// block 12

std::string interpolateNumber(std::string a, std::string b, float amount) {
    if (a == b) {
        return a;
    }
    auto vA = std::stod(a);
    auto vB = std::stod(b);

    return std::to_string(std::lerp(vA, vB, amount));
}
// end
// block 13

std::vector<std::string> interpolate(const std::vector<std::string> a,
                                     const std::vector<std::string> b,
                                     float amount) {
    auto ret = std::vector<std::string>{};
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        ret.push_back(interpolateNumber(a.at(i), b.at(i), amount));
    }
    return ret;
}
// end
// block 15

static auto tmpPath = TmpPath{};
std::filesystem::path createTempOutFilename(std::filesystem::path base, int i) {
    return tmpPath / (base.stem().string() + "-" + std::to_string(i) + ".svg");
}
// end
// block 19

void createVideo(std::filesystem::path base,
                 std::vector<std::filesystem::path> files) {
    base.replace_extension(".mp4");

    std::cout << "encoding video " << base << std::endl;

    auto listPath = std::string{tmpPath / "sthaotsehu-video-list.txt"};

    {
        auto file = std::ofstream{listPath};
        for (auto &path : files) {
            file << "file '" << std::filesystem::absolute(path).string()
                 << "'\n";
        }
    }

    std::filesystem::remove(base);

    {
        auto command = "ffmpeg -f concat -safe 0 -r 24 -i " + listPath +
                       " -c:v libx264rgb -crf 18 -preset ultrafast -loglevel "
                       "error -stats " +
                       base.string();

        std::system(command.c_str());
    }

    {
        auto firstFrame = base;
        firstFrame.replace_extension("-first.png");

        auto command = "inkscape  " + files.front().string() + " -o " +
                       firstFrame.string();

        std::system(command.c_str());
    }
    {
        auto lastFrame = base;
        lastFrame.replace_extension("-last.png");

        auto command =
            "inkscape " + files.back().string() + " -o " + lastFrame.string();

        std::system(command.c_str());
    }
    std::filesystem::remove(listPath);
}
// end
// block interpolate 8

void interpolateSvgs(std::filesystem::path a,
                     std::filesystem::path b,
                     const Settings &settings) {

    auto contentA = readFile(a);
    auto contentB = readFile(b);
    // block 10

    auto numbersA = extractNumbers(contentA);
    auto numbersB = extractNumbers(contentB);
    // end
    // block loop 11
    auto files = std::vector<std::filesystem::path>{};

    for (int i = 0; i <= settings.subframes; ++i) {
        auto t = 1.f / settings.subframes * i; // 14
        // auto numbers = interpolate(numbersA, numbersB, t); // 14-25
        auto numbers = interpolate(numbersA, numbersB, settings.ease(t)); // 25
        auto path = createTempOutFilename(a, i);                          // 16
        std::cout << "write to " << path << "\n";                         // 16
        auto file = std::ofstream{path};                                  // 16
        file << replaceNumbers(contentA, numbers);                        // 18
        files.push_back(path);                                            // 18
    }
    // end loop
    // block 20

    createVideo(settings.outFilename.empty() ? a : settings.outFilename, files);

    for (auto &path : files) {
        std::filesystem::remove(path);
    }

    std::cout << "done...\n";
    // end
}
// end interpolate
// 1
int main(int argc, char *argv[]) { // 1
    // block mainA 6
    const auto settings = Settings{argc, argv};

    if (settings.files.size() < 2) {
        std::cerr << "need at least 2 files to interpolate between\n";
        return 1;
    }

    // interpolateSvgs( // 6-26
    //     settings.files.front(), settings.files.at(1), settings); // 6-26
    //  block more 26
    for (size_t i = 1; i < settings.files.size(); ++i) {
        auto a = settings.files.at(i - 1);
        auto b = settings.files.at(i);
        interpolateSvgs(a, b, settings);
    }
    // end more
    //  end mainA
    return 0; // 1
} // 1
  // 1
