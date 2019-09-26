#include <iostream>
#include <vector>
#include <string>
#include <stack>



std::string restored_string(const std::vector<int>& z_fun) {
    if (z_fun.empty())
        return std::string();
    size_t right_border = 0;
    std::stack<size_t> left_borders;
    std::string result = "a";
    std::vector<int> alphabet(26, 0);
    alphabet[0] = 1;
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
                while (!left_borders.empty()) {
                    alphabet[result[right_border - left_borders.top() + 1] - 97] = 1;
                    left_borders.pop();
                }
                for (int k = 0; k < 26; ++k)
                    if (alphabet[k] != 1) {
                        result += static_cast<unsigned char>(k + 97);
                        break;
                    }
                alphabet = std::vector<int>(26, 0);
                alphabet[0] = 1;
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
    while (std::cin >> current)
        input.push_back(current);
    std::cout << restored_string(input);
    return 0;
}
