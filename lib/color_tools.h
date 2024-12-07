//
// Created by lin-salad on 12/6/24.
//

#ifndef COLOR_TOOLS_H
#define COLOR_TOOLS_H

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <fmt/format.h>

namespace salad {
    // Define a function to create the 3D array at compile time
    using rgb_cube_t = std::array<std::array<std::array<int, 6>, 6>, 6>;
    template<int Groups, int size>
    using rgb_group_t = std::array<std::array<int, size>, Groups>;

    constexpr auto create_rgb_cube() {
        std::array<std::array<std::array<int, 6>, 6>, 6> cube{};

        for (int r = 0; r < 6; ++r) {
            for (int g = 0; g < 6; ++g) {
                for (int b = 0; b < 6; ++b) {
                    cube[r][g][b] = 16 + r * 36 + g * 6 + b;
                }
            }
        }
        return cube;
    }

    // 8-bit colour stuff
    inline static constexpr rgb_cube_t rgb_cube = create_rgb_cube();

    // Const int(*)[8] is a pointer to an array of 8 integers
    inline static constexpr rgb_group_t<3, 8> rgb_greys = std::array<std::array<int, 8>, 3>{
            {{ 232, 233, 234, 235, 236, 237, 238, 239 },
               { 240, 241, 242, 243, 244, 245, 246, 247 },
               { 248, 249, 250, 251, 252, 253, 254, 255 }}
    };

    inline static constexpr rgb_group_t<2, 8> rgb_standard = std::array<std::array<int, 8>, 2>{
            {{ 0, 1, 2, 3, 4, 5, 6, 7 },
               { 8, 9, 10, 11, 12, 13, 14, 15 }}
    };

    enum COLOR {
        BLACK = rgb_cube[0][0][0],
        WHITE = rgb_cube[5][5][5],
        GREY = rgb_greys[1][0],
        RED = rgb_cube[5][0][0],
        GREEN = rgb_cube[0][5][0],
        BLUE = rgb_cube[0][0][5],
        YELLOW = rgb_cube[5][5][0],
        MAGENTA = rgb_cube[5][0][5],
        CYAN = rgb_cube[0][5][5],
        ORANGE = rgb_cube[5][2][0],
        PINK = rgb_cube[5][2][5],
        PURPLE = rgb_cube[2][0][5],
        TEAL = rgb_cube[0][2][5],
        LIME = rgb_cube[0][5][2]
    };
    
    inline std::string color_string(const std::string_view& str, const short fcolor = -1, const short bcolor = -1) {
        // Check if the string is already formatted like a color string
        // By checking if there's a sequence of adjacent [ESC, 38|48, ;, 5, ;
        std::string ret = std::string(str);
        if (str.find("\033[38;5;") != std::string::npos) {
            // Extract the string, find the first 'm' after the sequence
            std::size_t pos = str.find('m', str.find("\033[38;5;") + 8);
            std::size_t rst_pos = str.find("\033[0m", pos);

            if (pos == std::string::npos || rst_pos == std::string::npos) {
                return {str.begin(), str.end()};
            }
            
            ret = str.substr(pos + 1, rst_pos - (pos + 1));
        }
            
        switch ((fcolor != -1) + (bcolor != -1) << 1) {
        case 0b00: 
            return {ret.begin(), ret.end()};
        case 0b10:
            return fmt::format("\033[38;5;{:d}m{:s}\033[0m", fcolor, ret);
        case 0b01:
            return fmt::format("\033[48;5;{:d}m{:s}\033[0m", bcolor, ret);
        case 0b11:
            return fmt::format("\033[38;5;{:d};48;5;{:d}m{:s}\033[0m", fcolor, bcolor, ret);
        default:
            __builtin_unreachable();
        }
    }

    // Assuming the previous definitions are present, including rgb_cube, rgb_greys, rgb_standard, and COLOR enum

    // Function to map color specifications to 8-bit color codes
    inline short parse_color(const std::string& color_value, const rgb_cube_t& rgb_cube) {
        if (color_value.empty()) return -1;

        // Map of color names to their corresponding COLOR enum values
        static const std::unordered_map<std::string, short> color_map = {
            {"black", BLACK},
            {"white", WHITE},
            {"grey", GREY},
            {"red", RED},
            {"green", GREEN},
            {"blue", BLUE},
            {"yellow", YELLOW},
            {"magenta", MAGENTA},
            {"cyan", CYAN},
            {"orange", ORANGE},
            {"pink", PINK},
            {"purple", PURPLE},
            {"teal", TEAL},
            {"lime", LIME}
        };

        // Convert color_value to lowercase for case-insensitive matching
        std::string color_lower = color_value;
        std::transform(color_lower.begin(), color_lower.end(), color_lower.begin(), ::tolower);

        // Check if the color is a named color
        auto it = color_map.find(color_lower);
        if (it != color_map.end()) {
            return it->second;
        }

        // Check if the color_value is a pure numeric 8-bit code
        if (std::all_of(color_value.begin(), color_value.end(), ::isdigit)) {
            int code = std::stoi(color_value);
            if (code >= 0 && code <= 255) return static_cast<short>(code);
        }

        // Check if the color_value is an RGB triplet in the format "r,g,b"
        size_t first_comma = color_value.find(',');
        if (first_comma != std::string::npos) {
            size_t second_comma = color_value.find(',', first_comma + 1);
            if (second_comma != std::string::npos) {
                try {
                    short r = std::stoi(color_value.substr(0, first_comma));
                    int g = std::stoi(color_value.substr(first_comma + 1, second_comma - first_comma - 1));
                    int b = std::stoi(color_value.substr(second_comma + 1));
                    if (r >= 0 && r <= 5 && g >= 0 && g <= 5 && b >= 0 && b <= 5) {
                        return static_cast<short>(rgb_cube[r][g][b]);
                    }
                } catch (...) {
                    // Invalid RGB values; fall through to return -1
                }
            }
        }

        // If all parsing attempts fail, return -1 indicating no color
        return -1;
    }

    inline void trim(std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            start++;
        }

        auto end = s.end();
        do {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));
        s = std::string(start, end + 1);
    }

    // Takes a string and removes shortens all whitespaces to a single of that type
    inline void trim_spacing(std::string& s) {
        std::string result;
        char last_whitespace = '\0';

        for (char c : s) {
            if (c == last_whitespace) {
                continue;
            }
            last_whitespace = std::isspace(c) ? c : '\0';
            
            result += c;
        }

        s = result;
    }
    
    // Complete format_color function
    inline std::string format_color(const std::string_view str) {
        std::string ret;
        ret.reserve(str.size()); // Reserve enough space to optimize performance

        size_t pos = 0;

        while (pos < str.size()) {
            size_t start = str.find('{', pos);
            if (start == std::string::npos) {
                // No more color patterns; append the rest of the string
                ret += std::string(str.substr(pos));
                break;
            }

            // Append the text before the '{'
            ret += std::string(str.substr(pos, start - pos));

            size_t end = str.find('}', start);
            if (end == std::string::npos) {
                // No closing '}', append the rest of the string and exit
                ret += std::string(str.substr(start));
                break;
            }

            // Extract the content within '{' and '}'
            std::string inside = std::string(str.substr(start + 1, end - start - 1));

            // Find the last ':' to split text and color specification
            size_t colon = inside.rfind(':');
            if (colon == std::string::npos) {
                // No ':' found; treat as normal text
                ret += "{" + inside + "}";
                pos = end + 1;
                continue;
            }

            // Split into text and color_spec
            std::string text = inside.substr(0, colon);
            std::string color_spec = inside.substr(colon + 1);
            
            trim(text);
            trim(color_spec);

            // Split color_spec into type and value
            size_t space = color_spec.find(' ');
            if (space == std::string::npos) {
                // Invalid format; treat as normal text
                ret += "{" + inside + "}";
                pos = end + 1;
                continue;
            }

            std::string color_type = color_spec.substr(0, space);
            std::string color_value = color_spec.substr(space + 1);

            short fcolor = -1;
            short bcolor = -1;

            // Determine the color type and parse accordingly
            if (color_type == "f") {
                // Foreground color
                fcolor = parse_color(color_value, rgb_cube);
            }
            else if (color_type == "b") {
                // Background color
                bcolor = parse_color(color_value, rgb_cube);
            }
            else if (color_type == "fb") {
                // Both foreground and background colors
                // For simplicity, using the same color for both
                fcolor = parse_color(color_value, rgb_cube);
                bcolor = parse_color(color_value, rgb_cube);
            }
            else {
                // Unknown color type; treat as normal text
                ret += "{" + inside + "}";
                pos = end + 1;
                continue;
            }

            // Apply color formatting using color_string
            std::string colored_text = color_string(text, fcolor, bcolor);

            // Append the colored text to the result
            ret += colored_text;

            // Move the position past the '}'
            pos = end + 1;
        }

        return ret;
    }
 }

#endif //COLOR_TOOLS_H
