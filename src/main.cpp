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
--out                Set output filename
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
            else {
                files.push_back(arg);
            }
        }
    }
};

std::string readFile(std::filesystem::path path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> extractNumbers(const std::string &input) {

    // Using std::sregex_iterator to apply the pattern
    std::sregex_iterator numbersBegin =
        std::sregex_iterator(input.begin(), input.end(), numberPattern);
    std::sregex_iterator numbersEnd = std::sregex_iterator();

    // Iterate over all matches and store them in the vector
    std::vector<std::string> numbers;
    for (std::sregex_iterator it = numbersBegin; it != numbersEnd; ++it) {
        std::smatch match = *it;
        numbers.push_back(match.str());
    }

    return numbers;
}

std::string replaceNumbersWithString(
    const std::string &text, const std::vector<std::string> &replacements) {
    //    std::regex numberRegex("(\\d+)"); // Match one or more digits

    std::string result;
    auto replacementsIterator = replacements.begin();

    std::string::const_iterator searchStart = text.cbegin();
    std::smatch match;
    while (std::regex_search(searchStart, text.cend(), match, numberPattern)) {
        result += match.prefix().str(); // Add the text before the match

        // Replace the match with the next replacement string, or with the match
        // itself if there are no more replacements
        if (replacementsIterator != replacements.end()) {
            result += *replacementsIterator++;
        }
        else {
            result += match.str();
        }

        searchStart =
            match.suffix()
                .first; // Continue searching from the end of the match
    }

    result +=
        std::string(searchStart,
                    text.cend()); // Add the remaining text after the last match
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

    for (int i = 0; i < settings.subframes; ++i) {
        auto t = 1.f / settings.subframes * i;
        auto numbers = interpolate(numbersA, numbersB, t);
        auto file = std::ofstream{createOutFilename(settings.outFilename, i)};
        file << replaceNumbersWithString(contentA, numbers);
    }

    //    std::cout << replaceNumbersWithString(contentA, numbersA);

    return 0;
}
