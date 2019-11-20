#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <algorithm>





const int64_t alphabet_size = 256;
const int64_t char_a = 0;



int64_t sort_letters(
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
    for (int64_t i = str.length() - 1; i >= 0; --i) {
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
    return current_class;
}

std::vector<size_t> sort_by_second_parts (
        const std::string& str,
        int64_t current_length,
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
        int64_t current_length,
        std::vector<size_t>& sorted_suffixes,
        std::vector<size_t>& classes,
        int64_t& maxclass) {
    std::vector<size_t> new_classes(str.length());
    std::vector<size_t> count(maxclass + 1, 0);

    for (size_t i = 0; i < str.length(); ++i) {
        ++count[classes[sorted_by_second_parts[i]]];
    }
    for (size_t i = 1; i < maxclass + 1; ++i) {
        count[i] += count[i - 1];
    }
    for (int64_t i = str.length() - 1; i >= 0; --i) {
        if (i == 12) {
            int a = 2;
        }
        size_t index = sorted_by_second_parts[i];
        sorted_suffixes[--count[classes[index]]] = index;
    }
    new_classes[sorted_suffixes[0]] = 0;
    maxclass = 0;
    for (size_t i = 1; i < str.length(); ++i) {
        size_t first1 = sorted_suffixes[i];
        size_t first2 = sorted_suffixes[i - 1];
        size_t second1 = (first1 + current_length) % str.length();
        size_t second2 = (first2 + current_length) % str.length();

        if ((classes[first1] != classes[first2]) || (classes[second1] != classes[second2])) {
            ++maxclass;
        }
        new_classes[sorted_suffixes[i]] = maxclass;
    }
    classes = new_classes;
}

std::vector<size_t> suffix_array(const std::string& str) {
    std::vector<size_t> sorted_suffixes(str.length());  // Номера суффиксов, отсортированных лексикографически
    std::vector<size_t> classes(alphabet_size + str.length(), 0); // Для хранения классов эквивалентности

    // Отсортируем циклические подстроки длины один
    int64_t maxclass = sort_letters(str, sorted_suffixes, classes);

    // Теперь индуктивно строим для подстрок длины 2^k
    for (size_t current_length = 1; current_length < str.length(); current_length*=2) {
        // Сформируем сортированную по второй половине суффикса длины 2*current_length последовательность
        auto sorted_by_second_parts = sort_by_second_parts(str, current_length, sorted_suffixes);

        // И теперь сортируем подсчётом по левой половине
        count_sort(str, sorted_by_second_parts, current_length, sorted_suffixes, classes, maxclass);
        // В classes и sorted_suffixes теперь лежат новые значения
    }


    return sorted_suffixes;
}

std::vector<int64_t> lcp_on_suff_array(const std::string& str, const std::vector<size_t>& suf) {
    std::vector<int64_t> lcp(str.length());
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
            int64_t j = suf[positions[i] + 1];
            while (std::max(i + k, j + k) < str.length() && str[i + k] == str[j + k]) {
                ++k;
            }
            lcp[positions[i]] = k;
        }
    }
    return lcp;
}

bool are_in_different_strs (
        int64_t index1,
        int64_t index2,
        const std::string& first) {
    return ((index1 > first.length() && index2 < first.length()) ||
            (index2 > first.length() && index1 < first.length()));
}

std::string k_stat(const std::string& first, const std::string& second, int64_t k) {
    auto concatenated = first + '#' + second + '$';
    auto suf = suffix_array(concatenated);
    auto lcp = lcp_on_suff_array(concatenated, suf);
    std::string result;
    int64_t zero = 0;
    int64_t min = 0;
    for (int64_t i = 2; i < suf.size() - 1; ++i) {
        if (are_in_different_strs(suf[i], suf[i + 1], first) && lcp[i] != 0) {
            int64_t current_subtract = std::max(lcp[i] - min, zero);
            if (k <= current_subtract) {
                result = concatenated.substr(suf[i], k + min);
                k = 0;
                break;
            }
            else {
                k -= current_subtract;
                min = lcp[i];
            }
        }
        min = std::min(min, lcp[i]);
    }
    if (k > 0) {
        result = "-1";
    }
    return result;
}

int main() {

    std::string first;
    std::string second;
    int64_t k;
    std::cin >> first;
    std::cin >> second;
    std::cin >> k;
    if (k == 0) {
        std::cout << -1;
        return 0;
    }
    if (first.length() == 0 || second.length() == 0) {
        std::cout << -1;
        return 0;
    }
    std::cout << k_stat(first, second, k);

    return 0;
}