#include <iostream>
#include <string>
#include <vector>
#include <iterator>



int find_pref_func_value(
        const std::string& pattern,
        const std::vector<int>& prefix_array,
        int prev_value, char symbol) {
    if (prev_value == 0)
        return symbol == pattern[0] ? 1 : 0;

    else
        return symbol == pattern[prev_value] ?
               prev_value + 1 :
               find_pref_func_value(pattern, prefix_array, prefix_array[prev_value - 1],symbol);
}


template <class Iter>
void get_char(unsigned char& symbol, Iter iter) {
    symbol = *iter;
}


template <class Iter>
std::vector<int> find_pattern_positions(const std::string& pattern, Iter begin, Iter end) {
    std::vector<int> prefix_func_values(pattern.size());
    prefix_func_values[0] = 0;
    std::vector<int> result;

    // Хотим заполнить сначала значения для паттерна
    for (int i = 1; i < pattern.length(); ++i) {
        prefix_func_values[i] = find_pref_func_value(pattern, prefix_func_values, prefix_func_values[i - 1], pattern[i]);
    }
    auto current_iter = begin;
    int prev_value = 0;
    int index = 0;
    while (current_iter != end) {
        unsigned char current_symbol;
        get_char(current_symbol, current_iter);
        ++current_iter;
        prev_value = find_pref_func_value(pattern, prefix_func_values, prev_value, current_symbol);
        if (prev_value == pattern.size() - 1)
            result.push_back(index - pattern.size() + 2);
        ++index;
    }
    return result;
}

int main() {
    std::string pattern;
    std::cin >> pattern;
    auto begin = std::istream_iterator<unsigned char>(std::cin);
    auto end = std::istream_iterator<unsigned char>();
    pattern += "#";  // Разделитель между паттерном и текстом
    std::vector positions = find_pattern_positions(pattern, begin, end);
    for (auto i: positions) {
        std::cout << i << " ";
    }
    return 0;
}