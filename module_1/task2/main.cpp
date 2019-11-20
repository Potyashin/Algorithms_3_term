#include <iostream>
#include <vector>
#include <string>
#include <stack>



const int size_of_aplphabet = 26;
const int char_a = 97;

unsigned char to_char(int k) {
    return static_cast<unsigned char>(k + char_a);
}

std::string restored_string(const std::vector<int>& z_fun) {
    if (z_fun.empty())
        return std::string();
    size_t right_border = 0;  // Правая граница самого правого z-блока
    std::stack<size_t> left_borders;  // Все левый граници z-блоков, у которых правая - right_border
    std::string result = "a";
    std::vector<bool> alphabet(size_of_aplphabet, true);  // Доступный алфавит
    alphabet[0] = false;
    for (int i = 1; i < z_fun.size(); ++i) {
        if (z_fun[i] != 0) {
            int i_offset = z_fun[i] - 1;
            right_border = result.length() + z_fun[i] - 1;
            left_borders = std::stack<size_t>();
            for (int j = 0; j < z_fun[i]; ++j) {
                if (z_fun[i + j] + j > z_fun[i]) {
                    left_borders.push(i + j);
                    i_offset = j - 1;
                    break;
                }
                if (z_fun[i + j] + j == z_fun[i])
                    left_borders.push(i + j);
                result += result[j];
            }
            i += i_offset;
        }
        else {
            if (right_border == i - 1) {
                // Вычёркиваем симсолы, которые нельзя добавлять
                while (!left_borders.empty()) {
                    alphabet[result[right_border - left_borders.top() + 1] - char_a] = false;
                    left_borders.pop();
                }
                for (int k = 0; k < size_of_aplphabet; ++k)
                    if (alphabet[k] != false) {
                        result += to_char(k);
                        break;
                    }
                alphabet = std::vector<bool>(size_of_aplphabet, true);
                alphabet[0] = false;
            }
            else
                result += 'b';
        }
    }
    return result;
}


int main() {
    std::vector<int> input;
    int current;
    while (std::cin >> current) {
        if (current == '-')
            break;
        input.push_back(current);
    }
    std::cout << restored_string(input);
    return 0;
}
