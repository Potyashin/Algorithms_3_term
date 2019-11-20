#pragma optimize("g", off)

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <climits>
#include <algorithm>
#include <unordered_map>




const int alphabet_size = 28;
const int char_offset = 95;


class SuffixTree {
private:
    struct Node {
        Node();
        Node(size_t _left, size_t _right, int _parent = -1, int _link = -1);
        // На ребре хранится подстрока [left, right)
        size_t left = 0;
        size_t right = 0;
        int parent = -1;
        int link = -1;
        std::array<int, alphabet_size> edges;
    };

    struct ActivePtr {
        size_t vertex = 0;
        unsigned char edge = 0;
        size_t length = 0;
    };

public:
    SuffixTree();
    SuffixTree(const std::string& str);
    void add_symbol(unsigned char sym);
    Node get_vertex(size_t v) const { return tree[v]; }
    size_t get_length() const { return text.length(); }
private:
    std::string text;

    ActivePtr active_ptr;
    size_t remainder = 0;
    int waiting_suf_link = 0; // Вершина, из которой нужно будет кинуть ссылку
    std::vector<Node> tree;

    void make_suf_link(size_t node_for_link); // Кидает ссылку из запомненной в node_for_link
    void go_down(bool& flag); // Спускается вниз, пока active_length не будет больше длины текущего ребра
    // И если спустились, то flag = true

    size_t active_edge_length();  // Длина ребра из активной вершины по активному ребру
    size_t edge_length(size_t node, unsigned char edge);
};

SuffixTree::Node::Node(size_t _left, size_t _right, int _parent, int _link) :
        left(_left),
        right(_right),
        parent(_parent),
        link(_link) {
    for (size_t i = 0; i < alphabet_size; ++i) {
        edges[i] = -1;
    }
}

SuffixTree::Node::Node() {
    for (size_t i = 0; i < alphabet_size; ++i) {
        edges[i] = -1;
    }
}

SuffixTree::SuffixTree() {
    Node root;
    root.link = 0;
    tree.push_back(root);
}

SuffixTree::SuffixTree(const std::string& str) : SuffixTree() {
    for (auto i: str) {
        add_symbol(i);
    }
    add_symbol('$');
}

void SuffixTree::make_suf_link(size_t node_for_link) {
    if (waiting_suf_link > 0) {
        tree[waiting_suf_link].link = node_for_link;
    }
    waiting_suf_link = node_for_link;
}

size_t SuffixTree::active_edge_length() {
    auto edge = tree[active_ptr.vertex].edges[active_ptr.edge - char_offset];
    if (edge == -1)
        return INT_MAX;
    return tree[edge].right - tree[edge].left;
}

size_t SuffixTree::edge_length(size_t node, unsigned char edge) {
    auto q = tree[node].edges[edge - char_offset];
    return tree[q].right - tree[q].left;
}

void SuffixTree::go_down(bool& flag) {
    while (active_edge_length() <= active_ptr.length) {
        active_ptr.length -= active_edge_length();
        int length = active_edge_length();
        size_t old_active_node = active_ptr.vertex;
        active_ptr.vertex = tree[old_active_node].edges[active_ptr.edge - char_offset];
        if (active_ptr.length == 0) {
            active_ptr.edge = char_offset;
        }
        else {
            active_ptr.edge = text[text.size() - 1 - active_ptr.length];
        }

        flag = true;
    }
}

void SuffixTree::add_symbol(unsigned char sym) {
    ++remainder;
    text += sym;
    while (remainder > 0) {
        // На каждом шаге хотим добавить sym в суффикс длины remainder
        if (active_ptr.length == 0) {
            active_ptr.edge = sym;
        }
        // Если из active_ptr нет нужного ребра, то мы создаём лист
        if (tree[active_ptr.vertex].edges[active_ptr.edge - char_offset] == -1) {
            Node leaf(text.size() - 1, INT_MAX, active_ptr.vertex, -1);
            tree.push_back(leaf);
            tree[active_ptr.vertex].edges[active_ptr.edge - char_offset] = tree.size() - 1;
            // Не забываем кидать суффикную ссылку
            make_suf_link(active_ptr.vertex);
        }
        else {

            size_t next_vertex = tree[active_ptr.vertex].edges[active_ptr.edge - char_offset];

            bool did_go_down = false;
            go_down(did_go_down);
            if (did_go_down) {
                continue;
            }
            // Если символ уже на ребре, мы всего лишь изменяем active_length
            if (text[tree[next_vertex].left + active_ptr.length] == sym) {
                ++active_ptr.length;
                if (waiting_suf_link > 0) {
                    make_suf_link(active_ptr.vertex);
                }
                break;
            }

            // Иначе мы находимся где-то на ребре и сплитим его
            size_t new_node_index = tree.size();
            size_t new_leaf_index = new_node_index + 1;

            auto edge = tree[next_vertex];
            Node new_node(edge.left, edge.left + active_ptr.length, active_ptr.vertex, -1);
            tree.push_back(new_node);

            tree[active_ptr.vertex].edges[active_ptr.edge - char_offset] = new_node_index;
            tree[next_vertex].left += active_ptr.length;

            Node new_leaf(text.size() - 1, INT_MAX, new_node_index, -1);
            tree.push_back(new_leaf);

            tree[new_node_index].parent = active_ptr.vertex;
            tree[new_node_index].edges[sym - char_offset] = new_leaf_index;
            tree[new_node_index].edges[text[new_node.right] - char_offset] = next_vertex;

            tree[new_leaf_index].parent = new_node_index;
            tree[next_vertex].parent = new_node_index;

            make_suf_link(new_node_index);
        }

        --remainder;

        // Если active_ptr в root'е, то мы его готовим у следующему суффиксу
        if (active_ptr.vertex == 0 && active_ptr.length > 0) {
            --active_ptr.length;
            active_ptr.edge = text[text.size() - remainder];
        }
            // Иначе мы переходим по суффиксной ссылке
        else {
            int link = tree[active_ptr.vertex].link;
            active_ptr.vertex = link > 0 ? link : 0;
        }

    }
    waiting_suf_link = 0;
}

struct VertexDescription {
    size_t p = 0; // родитель
    size_t w = 0; // первая или вторая строка
    size_t lf = 0; // левая граница строки
    size_t rg = 0; // правя граница
};

void describe_tree (  // Выписывает вершины дерева
        const SuffixTree& tree,
        size_t vertex,
        std::vector<VertexDescription>& result,
        size_t first_length,
        size_t second_length,
        std::unordered_map<size_t, size_t>& indexes
) {
    for (size_t i = 0; i < alphabet_size; ++i) {
        if (tree.get_vertex(vertex).edges[i] != -1) {
            auto next_vertex = tree.get_vertex(tree.get_vertex(vertex).edges[i]);
            VertexDescription current;
            current.p = indexes[next_vertex.parent];
            current.lf = next_vertex.left;
            if (next_vertex.left < first_length) {
                current.w = 0;
            }
            else {
                current.w = 1;
            }

            if (current.w == 1) {
                current.rg = std::min(next_vertex.right, tree.get_length()) - first_length;
                current.lf -= first_length;
            }
            else {
                current.rg = std::min(next_vertex.right, first_length);
            }
            result.push_back(current);
            indexes[tree.get_vertex(vertex).edges[i]] = result.size();
            describe_tree(tree, tree.get_vertex(vertex).edges[i], result, first_length, second_length, indexes);
        }
    }
}

std::vector<VertexDescription> tree_from_two_string(const std::string& first, const std::string& second) {
    SuffixTree tree;
    for (auto i: first) {
        tree.add_symbol(i);
    }
    for (auto i: second) {
        tree.add_symbol(i);
    }
    std::vector<VertexDescription> result;
    std::unordered_map<size_t, size_t> indexes;
    indexes[0] = 0;
    describe_tree(tree, 0, result, first.length(), second.length(), indexes);
    return result;
}




int main() {

    std::string first;
    std::string second;

    std::cin >> first;

    SuffixTree tree(first);

    first.back() = 96;
    std::cin >> second;
    second.back() = 95;

    auto result = tree_from_two_string(first, second);
    std::cout << result.size() + 1 << std::endl;
    for (auto i: result) {
        std::cout << i.p << " "
                  << i.w << " "
                  << i.lf << " "
                  << i.rg << "\n";
    }

    return 0;
}