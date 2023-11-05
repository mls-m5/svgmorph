#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

constexpr auto helpStr = R"(
usage

./game [args]

options
--profile            Enable profiling
--subframes          Set number of frames to interpolate between
--out -o             Set output filename
)";

// Regular expression for matching integers and floating-point numbers
const std::regex numberPattern(R"([-+]?[0-9]*\.?[0-9]+)");

struct Settings {
    std::vector<std::filesystem::path> files;
    bool shouldEnableProfiling = false;
    int subframes = 10;
    std::filesystem::path outFilename = "out.svg";

    Settings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);
            if (arg == "--profile") {
                shouldEnableProfiling = true;
            }
            else if (arg == "--help" || arg == "-h") {
                std::cout << helpStr << std::endl;
                std::exit(0);
            }
            else if (arg == "--subframes") {
                subframes = std::stoi(args.at(++i));
            }
            else if (arg == "--out" || arg == "-o") {
                outFilename = args.at(++i);
            }
            else {
                files.push_back(arg);
            }
        }
    }
};

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

std::string interpolateNumber(std::string a, std::string b, float amount) {
    if (a == b) {
        return a;
    }
    auto vA = std::stod(a);
    auto vB = std::stod(b);

    return std::to_string(std::lerp(vA, vB, amount));
}

std::vector<std::string> interpolate(const std::vector<std::string> a,
                                     const std::vector<std::string> b,
                                     float amount) {
    auto ret = std::vector<std::string>{};
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        ret.push_back(interpolateNumber(a.at(i), b.at(i), amount));
    }
    return ret;
}

std::filesystem::path createOutFilename(std::filesystem::path base, int i) {
    return base.parent_path() / (base.stem().string() + std::to_string(i) +
                                 base.extension().string());
}

int main(int argc, char *argv[]) {
    const auto settings = Settings{argc, argv};
    std::cout << "files :\n";
    for (auto &file : settings.files) {
        std::cout << file << "\n";
    }

    if (settings.files.size() < 2) {
        std::cerr << "need at least 2 files to interpolate between\n";
        std::exit(1);
    }

    auto contentA = readFile(settings.files.front());
    auto contentB = readFile(settings.files.at(1));

    auto numbersA = extractNumbers(contentA);
    auto numbersB = extractNumbers(contentB);

    for (int i = 0; i <= settings.subframes; ++i) {
        auto t = 1.f / settings.subframes * i;
        auto numbers = interpolate(numbersA, numbersB, t);
        auto path = createOutFilename(settings.outFilename, i);
        std::cout << "write to " << path << "\n";
        auto file = std::ofstream{path};
        file << replaceNumbers(contentA, numbers);
    }

    std::cout << "done...\n";

    return 0;
}
