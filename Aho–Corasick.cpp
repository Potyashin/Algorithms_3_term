#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <iterator>



class Trie {
public:
    Trie();
    void add_string(const std::string& str);
    void make_suff_links();
    void make_compressed_links();
    std::vector<int> next_vertices(size_t index) const;
    size_t suffix_link(size_t index) const;
    size_t compressed_link(size_t index) const;
    std::vector<size_t> ending_patterns(size_t index) const;
    bool is_leaf(size_t index) const;
    bool is_there_the_edge(int node, unsigned char symbol) const;

private:
    size_t suff_link(size_t node);
    struct Node;
    std::vector<Node> trie;
    size_t count_patterns;

};

struct Trie::Node {
    Node() = default;
    std::vector<int> edges = std::vector<int>(26, -1);
    std::vector<size_t> patterns;
    bool is_leaf = false;
    unsigned char parent_ch = 0;
    size_t parent = 0;
    size_t link = 0;
    int compressed_link = -1;
};

Trie::Trie() : count_patterns(0) {
    Node root;
    trie.push_back(root);
}

void Trie::add_string(const std::string& str) {
    size_t index = 0;
    for (auto ch: str) {
        if (!is_there_the_edge(index, ch)) {
            Node new_node;
            trie.push_back(new_node);
            trie[index].edges[ch - 97] = trie.size() - 1;
            trie[trie.size() - 1].parent = index;
            trie[trie.size() - 1].parent_ch = ch;
            index = trie.size() - 1;

        }
        else {
            index = trie[index].edges[ch - 97];
        }
    }
    trie[index].is_leaf = true;
    trie[index].patterns.push_back(count_patterns++);
}

size_t Trie::suff_link(size_t node) {
    size_t parent = trie[node].parent;
    size_t current_link = trie[parent].link;
    unsigned char parent_symbol = trie[node].parent_ch;
    while (!is_there_the_edge(current_link, parent_symbol)) {
        if (current_link == 0)
            return 0;
        current_link = trie[current_link].link;
    }
    return trie[current_link].edges[parent_symbol - 97];
}

size_t Trie::compressed_link(size_t index) const {
    return trie[index].compressed_link;
}

void Trie::make_suff_links() {
    std::queue<size_t> queue;
    // обработали первый слой
    for (auto i: trie[0].edges) {
        if (i != -1)
            for (auto j: trie[i].edges)
                if (j != -1)
                    queue.push(j);
    }
    while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();
        for (auto i: trie[current].edges)
            if (i != -1)
                queue.push(i);
        trie[current].link = suff_link(current);
    }
}

void Trie::make_compressed_links() {
    std::queue<size_t> queue;
    // обработали первый слой
    for (auto i: trie[0].edges)
        if (i != -1)
            for (auto j: trie[i].edges)
                if (j != -1)
                    queue.push(j);
    while (!queue.empty()) {
        size_t current = queue.front();
        queue.pop();
        for (auto i: trie[current].edges)
            if (i != -1)
                queue.push(i);
        if (trie[trie[current].link].is_leaf)
            trie[current].compressed_link = trie[current].link;
        else if (trie[current].link == 0);
        else
            trie[current].compressed_link = trie[trie[current].link].compressed_link;
    }
}

std::vector<int> Trie::next_vertices(size_t index) const {
    return trie[index].edges;
}

size_t Trie::suffix_link(size_t index) const {
    return trie[index].link;
}

bool Trie::is_leaf(size_t index) const {
    return trie[index].is_leaf;
}

std::vector<size_t> Trie::ending_patterns(size_t index) const {
    return trie[index].patterns;
}

bool Trie::is_there_the_edge(int node, unsigned char symbol) const {
    if (trie[node].edges[symbol - 97] == -1)
        return false;
    else
        return true;
    //return trie[node].edges[symbol - 97];
}


std::vector<int> separate_patterns(const std::string& pattern, Trie& trie) {
    std::vector<int> ends_of_intervals;
    std::string current_str;
    for (size_t i = 0; i < pattern.length(); ++i)
        if (pattern[i] == '?') {
            ends_of_intervals.push_back(i-1);
            trie.add_string(current_str);
            current_str = std::string();
            for (;pattern[i] == '?' && i < pattern.length(); ++i);
            --i;
        }
        else
            current_str += pattern[i];

    trie.add_string(current_str);
    ends_of_intervals.push_back(pattern.length() - 1);

    return ends_of_intervals;
}

template <class Iter>
auto patterns_in_text(const Trie& trie, int to_miss_start, const std::vector<int>& ends, Iter begin, Iter end) {
    //result - количество подпаттернов, возможных из даного индекса
    std::vector<int> result;
    size_t current_node = 0;
    size_t count = 0;
    Iter current_iter = begin;
    std::advance(current_iter, to_miss_start);
    while (current_iter != end) {
        unsigned char symbol = *current_iter;
        if (symbol == '-')
           break;
        result.push_back(0);
        ++current_iter;
        while (true) {
            if (!trie.is_there_the_edge(current_node, symbol)) {
                if (current_node == 0) {
                    ++count;
                    break;
                }
                else {
                    current_node = trie.suffix_link(current_node);
                    continue;
                }
            }
            else
                current_node = trie.next_vertices(current_node)[symbol - 97];
            int current_compressed_link = trie.compressed_link(current_node);

            // Добавим все сжатые суффиксные
            while (current_compressed_link != -1) {
                for (auto i: trie.ending_patterns(current_compressed_link)) {
                    if (ends[i] <= count)
                        ++result[count - ends[i]];
                }
                current_compressed_link = trie.compressed_link(current_compressed_link);
            }
            // Добавим саму текущую, если нужно
            if (trie.is_leaf(current_node))
                for (auto i: trie.ending_patterns(current_node))
                    if (ends[i] <= count)
                        ++result[count - ends[i]];
            ++count;
            break;
        }
    }
    return result;
}

int main() {
    Trie trie;
    std::string pattern;
    std::cin >> pattern;

    int count_start_qmarks = 0;
    int count_end_qmarks = 0;
    while (pattern[count_start_qmarks] == '?')
        ++count_start_qmarks;
    while (pattern[pattern.size() - 1 - count_end_qmarks] == '?')
        ++count_end_qmarks;
    int new_length = pattern.size() - count_end_qmarks - count_start_qmarks;
    std::string new_pattern = pattern.substr(count_start_qmarks, new_length);
    if (new_pattern.empty()) {
        std::string str;
        std::cin >> str;
        for (int i = 0; i < str.length() - count_start_qmarks + 1; ++i)
            std::cout << i << " ";
        return 0;
    }

    std::vector<int> ends_of_patterns = separate_patterns(new_pattern, trie);
    trie.make_suff_links();
    trie.make_compressed_links();

    auto begin = std::istream_iterator<unsigned char>(std::cin);
    auto end = std::istream_iterator<unsigned char>();
    int number_of_patterns = ends_of_patterns.size();
    auto splitted_pattern_occurrences = patterns_in_text(trie, count_start_qmarks, ends_of_patterns, begin, end);
    std::vector<int> result;
    int text_size = splitted_pattern_occurrences.size();

    for (int i = 0; i < text_size - new_length - count_end_qmarks + 1; ++i)
        if (splitted_pattern_occurrences[i] == number_of_patterns)
            std::cout << i << " ";
    return 0;
}