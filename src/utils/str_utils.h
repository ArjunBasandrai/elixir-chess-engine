#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace elixir::str_utils {
    inline std::vector<std::string> split(const std::string& str, char delim) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream token_stream(str);
        while (std::getline(token_stream, token, delim)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    inline std::string remove_whitespaces(std::string_view input) {
        std::string output;
        for (char c : input) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                output.push_back(c);
            }
        }
        return output;
    }

}