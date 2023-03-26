#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <set>
#include <cmath>
#include <iomanip>

const int MAX_LINES_PER_TOKEN = 9;

struct LineInfo {
    int line_num;
    std::set<int> lines;
    int count;
};

// Comparator function object to implement a strict weak order
struct CompareIgnoreCase {
    bool operator()(const std::string& a, const std::string& b) const {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), 
                                            [](char c1, char c2){ return std::tolower(c1) < std::tolower(c2); });
    }
};

int getMaxWordLength(const std::unordered_map<std::string, std::set<LineInfo>>& wordMap) {
    int maxLength = 0;
    for (auto& [word, lineInfoSet] : wordMap) {
        int length = word.length();
        if (length > maxLength) {
            maxLength = length;
        }
    }
    return maxLength;
}

void printLineNumbers(const std::string& word, int* lineNumbers, int numLineNumbers, int maxWordLength) {
    std::cout << std::setw(maxWordLength) << std::left << word << " : ";
    std::sort(lineNumbers, lineNumbers + numLineNumbers);
    int prevLineNumber = -1;
    int count = 0;
    for (int i = 0; i < numLineNumbers; i++) {
        int lineNumber = lineNumbers[i];
        if (lineNumber != prevLineNumber) {
            if (count > 0) {
                std::cout << ", ";
            }
            std::cout << lineNumber << ":" << count + 1;
            prevLineNumber = lineNumber;
            count = 0;
        } else {
            count++;
        }
    }
    std::cout << std::endl;
}

// Utility function to strip non-word characters from the beginning and end of a string
std::string stripNonWord(const std::string& str) {
    std::string result = str;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](char c){ return std::isalpha(c); }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](char c){ return std::isalpha(c); }).base(), result.end());
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: unable to open input file \"" << argv[1] << "\"" << std::endl;
        return 1;
    }

    std::regex wordRegex(R"([a-zA-Z][a-zA-Z'-]*)"); // regular expression to match word-like tokens
    std::unordered_map<std::string, std::set<LineInfo, CompareIgnoreCase>> wordMap; // map of words to line info sets

    std::string line;
    int lineNum = 0;
    while (std::getline(inputFile, line)) {
        // std::cout << "lineNum: " << lineNum << std::endl;
        ++lineNum;
        std::regex_iterator<std::string::iterator> wordIt(line.begin(), line.end(), wordRegex);
        std::regex_iterator<std::string::iterator> wordEnd;
        while (wordIt != wordEnd) {
            // std::cout << "wordIt: " << wordIt->str() << std::endl;
            std::string word = stripNonWord(wordIt->str());
            auto& lineInfoSetRef = wordMap[word];
            // std::cout << "here" << std::endl;
            if (lineInfoSetRef.empty()) {
                ++wordIt;
                continue;
            }
            LineInfo lastLineInfo = *lineInfoSetRef.rbegin();
            lastLineInfo.count++;
            if (wordMap[word].size() > MAX_LINES_PER_TOKEN) {
                // std::cout << "Erasing word: " << word << std::endl;
                wordMap[word].erase(wordMap[word].begin());
            }
            ++wordIt;
        }
    }

    // Print the cross-reference listing
    int maxWordLength = 0;
    for (auto& [word, lineInfoSet] : wordMap) {
        maxWordLength = std::max(maxWordLength, static_cast<int>(word.length()));
    }
    std::string formatString = "%" + std::to_string(maxWordLength) + "s: ";
    for (int i = 1; i <= MAX_LINES_PER_TOKEN; ++i) {
        formatString += "%" + std::to_string(2 + static_cast<int>(std::log10(i + 1))) + "s ";
    }
    std::cout << std::endl;

    int lineNumbers[MAX_LINES_PER_TOKEN];
    int numLineNumbers = 0;

    for (auto& [word, lineInfoSet] : wordMap) {
        numLineNumbers = 0;
        if (word.empty()) {
            // std::cout << "line info set empty" << std::endl;
            // std::cout << "Word: " << word << std::endl;
        }
        // std::cout << "Word: " << word << std::endl;
        for (auto& lineInfo : lineInfoSet) {
            if (numLineNumbers >= MAX_LINES_PER_TOKEN) {
                // print the line numbers collected so far and reset the array and count
                printLineNumbers(word, lineNumbers, numLineNumbers, maxWordLength);
                numLineNumbers = 0;
            }
            lineNumbers[numLineNumbers++] = lineInfo.line_num;
        }
        // print any remaining line numbers and reset the array and count
        if (numLineNumbers > 0) {
            printLineNumbers(word, lineNumbers, numLineNumbers, maxWordLength);
        }
    }
}