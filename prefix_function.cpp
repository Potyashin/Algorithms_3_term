#include <iostream>
#include <string>
#include <vector>


int find_current_position(
        const std::string& pattern,
        const std::vector<int>& prefix_array,
        int prev_value, char symbol) {
    if (prev_value == 0)
        return symbol == pattern[0] ? 1 : 0;

    else
        return symbol == pattern[prev_value] ?
               prev_value + 1 :
               find_current_position(pattern, prefix_array, prefix_array[prev_value - 1],symbol);
}


std::vector<int> find_pattern_positions(const std::string& pattern) {
    std::vector<int> prefix_array(pattern.size());
    prefix_array[0] = 0;
    std::vector<int> result;

    // Хотим заполнить сначала значения для паттерна
    for (int i = 1; i < pattern.length(); ++i) {
        prefix_array[i] = find_current_position(pattern, prefix_array, prefix_array[i - 1], pattern[i]);
    }

    int prev_value = 0;
    int index = 0;
    while (true) {
        char current_symbol;
        std::cin >> current_symbol;
        if (std::cin.eof())
            break;
        prev_value = find_current_position(pattern, prefix_array, prev_value, current_symbol);
        if (prev_value == pattern.size() - 1)
            result.push_back(index - pattern.size() + 2);
        ++index;
    }
    return result;
}

int main() {
    std::string pattern;
    std::cin >> pattern;
    pattern += "#";  // Разделитель между паттерном и текстом
    std::vector positions = find_pattern_positions(pattern);
    for (const auto& i: positions) {
        std::cout << i << " ";
    }
    return 0;
}