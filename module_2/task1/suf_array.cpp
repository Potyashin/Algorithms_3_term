#include <iostream>
#include <vector>
#include <string>



const int alphabet_size = 256;
const int char_a = 0;



void sort_letters(
        const std::string& str,
        std::vector<size_t>& sorted_suffixes,
        std::vector<size_t>& classes) {
    std::vector<size_t> count(alphabet_size, 0);
    for (auto ch: str) {
        ++count[ch - char_a];
    }
    for (size_t i = 1; i < alphabet_size; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = str.length() - 1; i >= 0; --i) {
        sorted_suffixes[--count[str[i] - char_a]] = i;
    }
    size_t current_class = 0;
    classes[sorted_suffixes[0]] = 0;
    for (size_t i = 1; i < str.length(); ++i) {
        if (str[sorted_suffixes[i]] != str[sorted_suffixes[i - 1]]) {
            ++current_class;
        }
        classes[sorted_suffixes[i]] = current_class;
    }
}

std::vector<size_t> sort_by_second_parts (
        const std::string& str,
        int current_length,
        const std::vector<size_t>& sorted_suffixes) {
    std::vector<size_t> result(str.length());
    for (size_t i = 0; i < str.length(); ++i) {
        if (sorted_suffixes[i] < current_length) {
            // Случай "зацикливания"
            result[i] = str.length() + sorted_suffixes[i] - current_length;
        }
        else {
            result[i] = sorted_suffixes[i] - current_length;
        }
    }
    return result;
}

void count_sort(
        const std::string& str,
        const std::vector<size_t>& sorted_by_second_parts,
        int current_length,
        std::vector<size_t>& sorted_suffixes,
        std::vector<size_t>& classes) {
    std::vector<size_t> new_classes(str.length());
    std::vector<size_t> count(alphabet_size, 0);

    for (size_t i = 0; i < str.length(); ++i) {
        ++count[classes[sorted_by_second_parts[i]]];
    }
    for (size_t i = 1; i < alphabet_size; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = str.length() - 1; i >= 0; --i) {
        size_t index = sorted_by_second_parts[i];
        sorted_suffixes[--count[classes[index]]] = index;
    }
    new_classes[sorted_suffixes[0]] = 0;
    size_t current_class = 0;
    for (size_t i = 1; i < str.length(); ++i) {
        size_t first1 = sorted_suffixes[i];
        size_t first2 = sorted_suffixes[i - 1];
        size_t second1 = (first1 + current_length) % str.length();
        size_t second2 = (first2 + current_length) % str.length();

        if ((classes[first1] != classes[first2]) || (classes[second1] != classes[second2])) {
            ++current_class;
        }
        new_classes[sorted_suffixes[i]] = current_class;
    }
    classes = new_classes;
}

std::vector<size_t> suffix_array(const std::string& str) {
    std::vector<size_t> sorted_suffixes(str.length());  // Номера суффиксов, отсортированных лексикографически
    std::vector<size_t> classes(str.length()); // Для хранения классов эквивалентности

    // Отсортируем циклические подстроки длины один
    sort_letters(str, sorted_suffixes, classes);

    // Теперь индуктивно строим для подстрок длины 2^k
    for (size_t current_length = 1; current_length < str.length(); current_length*=2) {
        // Сформируем сортированную по второй половине суффикса длины 2*current_length последовательность
        auto sorted_by_second_parts = sort_by_second_parts(str, current_length, sorted_suffixes);

        // И теперь сортируем подсчётом по левой половине
        count_sort(str, sorted_by_second_parts, current_length, sorted_suffixes, classes);
        // В classes и sorted_suffixes теперь лежат новые значения
    }


    return sorted_suffixes;
}

std::vector<int> lcp_on_suff_array(const std::string& str, const std::vector<size_t>& suf) {
    std::vector<int> lcp(str.length());
    std::vector<size_t> positions(str.length()); // обратный к suf

    for (size_t i = 0; i < positions.size(); ++i) {
        positions[suf[i]] = i;
    }

    size_t k = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (k > 0) {
            --k;
        }
        if (positions[i] == str.length() - 1) {
            lcp[str.length() - 1] = -1;
            k = 0;
            continue;
        }
        else {
            int j = suf[positions[i] + 1];
            while (std::max(i + k, j + k) < str.length() && str[i + k] == str[j + k]) {
                ++k;
            }
            lcp[positions[i]] = k;
        }
    }
    return lcp;
}

size_t count_substring(const std::string& str) {
    std::vector<size_t> suf = suffix_array(str);
    auto lcp = lcp_on_suff_array(str, suf);
    size_t result = 0;
    for (size_t i = 0; i < str.length() - 1; ++i) {
        result += str.length() - suf[i] - lcp[i];
    }
    result -= suf[str.length() - 1];
    return result;
}

int main() {

    std::string input;
    std::cin >> input;
    if (input.length() == 0) {
        std::cout << 0;
        return 0;
    }
    input += '#'; // Предыдущий символ до 'a'
    std::cout << count_substring(input);

    return 0;
}