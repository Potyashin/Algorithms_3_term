#include <iostream>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <set>
#include <iomanip>



struct Point {
    long double x = 0;
    long double y = 0;
    long double z = 0;
    int index = -1;

    Point* next;
    Point* prev;

    Point& operator+=(const Point& other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }
    Point& operator-=(const Point& other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }
    const Point operator+(const Point& other) const {
        Point result(*this);
        result += other;
        return result;
    }
    const Point operator-(const Point& other) const {
        Point result(*this);
        result -= other;
        return result;
    }

    bool act() {
        if (prev->next != this) {
            prev->next = this;
            next->prev = this;
            return true;
        }
        else {
            next->prev = prev;
            prev->next = next;
            return false;
        }
    }
};

using Action = Point;

long double vector_product_in_triangle(const Point* p1, const Point* p2, const Point* p3) {
    if (!p1 || !p2 || !p3) {
        return DBL_MAX;
    }

    return (p2->x - p1->x) * (p3->y - p2->y) - (p2->y - p1->y) * (p3->x - p2->x);
}

bool is_clockwise(const Point* p1, const Point* p2, const Point* p3) {
    return vector_product_in_triangle(p1, p2, p3) < 0;
}

long double time_point(const Point* p1, const Point* p2, const Point* p3) {
    if (!p1 || !p2 || !p3) {
        return DBL_MAX;
    }
    long double tmp = (p2->x - p1->x) * (p3->z - p2->z) - (p2->z - p1->z) * (p3->x - p2->x);
    return tmp/vector_product_in_triangle(p1, p2, p3);
}

struct Facet {
    int first;
    int second;
    int third;
};


std::vector<Action*> convex_hull_3d_rec(std::vector<Point>& points, int left, int right) {

    if (right <= left + 1) {
        return std::vector<Action*>();
    }

    int middle = (left + right)/2;

    std::vector<Action*> actions_left = convex_hull_3d_rec(points, left, middle);
    std::vector<Action*> actions_right = convex_hull_3d_rec(points, middle, right);

    Point* bridge_left = &points[middle - 1];
    Point* bridge_right = &points[middle];

    while (true) {
        if (is_clockwise(bridge_left->prev, bridge_left, bridge_right)) {
            bridge_left = bridge_left->prev;
        }
        else if (is_clockwise(bridge_left, bridge_right, bridge_right->next)) {
            bridge_right = bridge_right->next;
        }
        else {
            break;
        }
    }

    std::vector<Action*> result;

    int acted_left = 0;
    int acted_right = 0;
    long double time = -DBL_MAX;
    while(true) {
        Action* action_left = nullptr;
        Action* action_right = nullptr;

        std::vector<long double> time_points(6, DBL_MAX);

        if (acted_left < actions_left.size()) {
            action_left = actions_left[acted_left];
            time_points[0] = time_point(action_left->prev, action_left, action_left->next);
        }
        if (acted_right < actions_right.size()) {
            action_right = actions_right[acted_right];
            time_points[1] = time_point(action_right->prev, action_right, action_right->next);
        }
        time_points[2] = time_point(bridge_left->prev, bridge_left, bridge_right);
        time_points[3] = time_point(bridge_left, bridge_left->next, bridge_right);
        time_points[4] = time_point(bridge_left, bridge_right, bridge_right->next);
        time_points[5] = time_point(bridge_left, bridge_right->prev, bridge_right);


        int index_min = -1;
        long double time_min = DBL_MAX;
        for (int i = 0; i < 6; ++i) {
            if (time_points[i] > time && time_points[i] < time_min) {
                time_min = time_points[i];
                index_min = i;
            }
        }

        if (index_min == -1 || time_min == DBL_MAX) {
            break;
        }

        if (index_min == 0) {
            if (action_left->x < bridge_left->x) {
                result.push_back(action_left);
            }
            action_left->act();
            ++acted_left;
        }
        else if (index_min == 1) {
            if (action_right->x > bridge_right->x) {
                result.push_back(action_right);
            }
            action_right->act();
            ++acted_right;
        }
        else if (index_min == 2) {
            result.push_back(bridge_left);
            bridge_left = bridge_left->prev;
        }
        else if (index_min == 3) {
            result.push_back(bridge_left->next);
            bridge_left = bridge_left->next;
        }
        else if (index_min == 4) {
            result.push_back(bridge_right);
            bridge_right = bridge_right->next;
        }
        else if (index_min == 5) {
            result.push_back(bridge_right->prev);
            bridge_right = bridge_right->prev;
        }

        time = time_min;
    }

    bridge_left->next = bridge_right;
    bridge_right->prev = bridge_left;

    for (int i = static_cast<int>(result.size()) - 1; i >= 0; --i) {
        Point* cur = result[i];
        if (cur->x <= bridge_left->x || cur->x >= bridge_right->x) {
            cur->act();
            if (cur == bridge_left) {
                bridge_left = bridge_left->prev;
            }
            else if (cur == bridge_right) {
                bridge_right = bridge_right->next;
            }
        }
        else {
            bridge_left->next = cur;
            bridge_right->prev = cur;
            cur->prev = bridge_left;
            cur->next = bridge_right;
            if (cur->x <= points[middle - 1].x) {
                bridge_left = cur;
            }
            else {
                bridge_right = cur;
            }
        }
    }

    return result;
}


void change_coordinates(long double& coord1, long double& coord2, long double angle) {
    long double new_coord1 = coord1 * cos(angle) +coord2 * sin(angle);
    long double new_coord2 = coord1 * (-1) * sin(angle) + coord2 * cos(angle);
    coord1 = new_coord1;
    coord2 = new_coord2;
}


void make_shift(std::vector<Point>& points) {
    long double angle = 0.000000000001;
    for (auto& p: points) {
        change_coordinates(p.x, p.z, angle);
        change_coordinates(p.z, p.y, angle);
        change_coordinates(p.x, p.y, angle);
    }
}

std::vector<Facet> convex_hull_3d(std::vector<Point>& points) {

    make_shift(points);

    std::vector<Facet> convex_hull;
    std::sort(points.begin(), points.end(), [](const Point& p1, const Point& p2){ return p1.x < p2.x; });

    std::vector<Action*> actions = convex_hull_3d_rec(points, 0, points.size());

    for(auto i: actions) {
        Facet current {i->prev->index, i->index, i->next->index};
        if (!i->act()) {
            std::swap(current.first, current.second);
        }
        convex_hull.push_back(current);
    }


    return convex_hull;
}

void sort_in_facets(std::vector<Facet>& facets) {
    for (auto& i: facets) {
        if (i.second == std::min({i.first, i.second, i.third})) {
            int tmp = i.first;
            i.first = i.second;
            i.second = std::min({tmp, i.third});
            i.third = std::max({tmp, i.third});
        }
        else if (i.third == std::min({i.first, i.second, i.third})) {
            int tmp1 = i.first;
            int tmp2 = i.second;
            i.first = i.third;
            i.second = std::min({tmp1, tmp2});
            i.third = std::max({tmp1, tmp2});
        }
        else {
            if (i.second > i.third) {
                std::swap(i.second, i.third);
            }
        }
    }
}

long double avg_num_of_facets_in_voronoi_diagram(std::vector<Point> points) {
    long double result = 0;

    long double min_x = 2e7;
    long double min_y = 3e8;
    long double min_z = min_x * min_y;


    points.push_back({-min_x, min_y, min_z, -1});
    points.push_back({min_x, -min_y, min_z, -2});

    std::vector<Facet> facets = convex_hull_3d(points);

    if (facets.empty())
        return result;

    sort_in_facets(facets);

    std::set<int> not_int_sites;

    int num_of_edges = 0;

    for(auto i: facets) {
        if (i.first < 0) {
            not_int_sites.insert(i.first);
            not_int_sites.insert(i.second);
            not_int_sites.insert(i.third);
        }
    }
    for(auto i: facets) {
        bool b1 = not_int_sites.count(i.first) == 0;
        bool b2 = not_int_sites.count(i.second) == 0;
        bool b3 = not_int_sites.count(i.third) == 0;
        if (b1) {
            num_of_edges += 1;
        }
        if (b2) {
            num_of_edges += 1;
        }
        if (b3) {
            num_of_edges += 1;
        }
    }

    int num_of_sites = points.size() - not_int_sites.size();
    result = num_of_sites == 0 ? 0 : 1.*num_of_edges/num_of_sites;

    return result;
}

int main() {


    long double current_x;
    long double current_y;
    std::vector<Point> points;
    int i = 0;
    while (std::cin >> current_x) {
        if (current_x == 123456789)
            break;
        std::cin >> current_y;
        points.push_back({current_x, current_y, current_x * current_x + current_y * current_y, i});
        ++i;
    }

    printf("%.16Lf", avg_num_of_facets_in_voronoi_diagram(points));
    //std::cout << std::setprecision(6)<< std::fixed << avg_num_of_facets_in_voronoi_diagram(points) << '\n';
    return 0;
}
