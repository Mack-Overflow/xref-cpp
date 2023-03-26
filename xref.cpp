#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include <algorithm>

const int MAX_LINE_NUMBERS = 9;

// Function to sanitize a token by removing any non-letter, hyphen, or apostrophe characters
std::string sanitize_token(std::string token) {
    std::string sanitized = "";
    for (char c : token) {
        if (isalpha(c) || c == '-' || c == '\'') {
            sanitized += tolower(c);
        }
    }
    return sanitized;
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::vector<int>> xref;

    // Check if a filename was passed in as a command-line argument
    if (argc > 1) {
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Error opening file " << argv[1] << std::endl;
            return 1;
        }

        // Read the file line by line and process each line
        std::string line;
        int line_number = 0;
        while (std::getline(file, line)) {
            line_number++;
            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                if (token[0] == '-') {continue;}
                std::string sanitized_token = sanitize_token(token);
                if (!sanitized_token.empty()) {
                    xref[sanitized_token].push_back(line_number);
                }
            }
        }
    }
    // Otherwise, read from standard input
    else {
        std::string line;
        int line_number = 0;
        while (std::getline(std::cin, line)) {
            if (line == "") { // Enter an empty line to terminate the program
                break;
            }
            line_number++;
            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                std::string sanitized_token = sanitize_token(token);
                if (!sanitized_token.empty()) {
                    xref[sanitized_token].push_back(line_number);
                }
            }
        }
    }

    // Print the xref map
    int max_token_length = 0;
    for (auto it = xref.begin(); it != xref.end(); ++it) {
        max_token_length = std::max(max_token_length, (int)it->first.length());
    }

    for (auto it = xref.begin(); it != xref.end(); ++it) {
        std::string token = it->first;
        std::vector<int> line_numbers = it->second;
        std::sort(line_numbers.begin(), line_numbers.end());
        std::cout << std::setw(max_token_length) << std::left << token << " : ";

        int count = 0;
        int current_line = -1;
        for (int line : line_numbers) {
            if (line != current_line) {
                if (count > MAX_LINE_NUMBERS) {
                    std::cout << std::endl << std::string(max_token_length + 3, ' ');
                }
                std::cout << line << ":";
                count = 1;
                current_line = line;
            } else {
                count++;
            }
            std::cout << count << ", ";
        }
        std::cout << std::endl;
    }
    return 0;
}