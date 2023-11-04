#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <vector>

constexpr auto helpStr = R"(
usage

./game [args]

options
--profile            Enable profiling
--capture [file]     Save screen to video file
)";

// Regular expression for matching integers and floating-point numbers
const std::regex numberPattern(R"([-+]?[0-9]*\.?[0-9]+)");

struct Settings {
    std::vector<std::filesystem::path> files;
    bool shouldEnableProfiling = false;

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

// std::string replaceNumbersWithString(const std::string& text, const
// std::vector<std::string>& replacements) {
//     std::regex numberRegex("(\\d+)"); // Match one or more digits

//    auto replacementsIterator = replacements.begin();

//           // Marking the lambda as mutable to modify the captures
//    auto numberReplacer = [&replacementsIterator, &replacements](const
//    std::smatch& match) mutable -> std::string {
//        if (replacementsIterator != replacements.end()) {
//            // Get the replacement string
//            std::string replacement = *replacementsIterator;
//            // Move to the next replacement
//            ++replacementsIterator;
//            return replacement;
//        } else {
//            // If we run out of replacements, just return the match
//            return match[0];
//        }
//    };

//           // This will call numberReplacer for each match and use its return
//           value as the replacement
//    return std::regex_replace(text, numberRegex, numberReplacer);
//}
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

int main(int argc, char *argv[]) {
    const auto settings = Settings{argc, argv};
    std::cout << "files :\n";
    for (auto &file : settings.files) {
        std::cout << file << "\n";
    }

    auto content = readFile(settings.files.front());
    std::cout << content << "\n";

    auto numbers = extractNumbers(content);

    for (auto &n : numbers) {
        std::cout << "number: " << n << "\n";
        n = std::to_string(42);
    }

    std::cout << replaceNumbersWithString(content, numbers);

    return 0;
}
