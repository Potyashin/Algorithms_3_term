#include <iostream>
#include <functional>
#include <cmath>

const double epsilon = 0.00000001;

struct Point {
    double x = 0;
    double y = 0;
    double z = 0;

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
};

struct Segment {
    Point first;
    Point second;
};

double point_point_distance (const Point& p1, const Point& p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
}

double cut(double left, double right) {
    return left + (right - left)/3;
}

Point cut_segment(const Segment& seg) {
    double x = cut(seg.first.x, seg.second.x);
    double y = cut(seg.first.y, seg.second.y);
    double z = cut(seg.first.z, seg.second.z);
    return Point{x,y,z};
}


template <class T, class CutFuncLeft, class CutFuncRight, class Func, class Cmp>
auto ternary_search(const T& left_border, const T& right_border, CutFuncLeft cut_left, CutFuncRight cut_right, Func func, Cmp cmp) {
    // cut_left has to take [left, right] and return middle left border, cur_right - right ^)
    // func has to be unimodal; it's the func, the max\min value of which you search; take one arg of type T
    // cmp has to compare types func returns;
    // e.g. if you search maximum cmp has to be like less
    T left = left_border;
    T right = right_border;
    T mid_left = cut_left(left, right);
    T mid_right = cut_right(left, right);


    while (true) {
        auto mid_left_value = func(mid_left);
        auto mid_right_value = func(mid_right);

        if (!cmp(mid_left_value, mid_right_value)) {
            left = mid_left;
        }
        else {
            right = mid_right;
        }

        mid_left = cut_left(left, right);
        mid_right = cut_right(left, right);

        mid_right_value = func(mid_right);
        mid_left_value = func(mid_left);

        if (point_point_distance(mid_left, mid_right) < epsilon) {
            return mid_left_value;
        }
    }
}

int main() {
    Segment seg1;
    Segment seg2;
    std::cin >> seg1.first.x >> seg1.first.y >> seg1.first.z;
    std::cin >> seg1.second.x >> seg1.second.y >> seg1.second.z;
    std::cin >> seg2.first.x >> seg2.first.y >> seg2.first.z;
    std::cin >> seg2.second.x >> seg2.second.y >> seg2.second.z;

    auto cut_left = [](const Point& l, const Point& r){
        return cut_segment(Segment{l, r});
    };
    auto cut_right = [cut_left](const Point& l, const Point& r){
        return l + r - cut_left(l, r);
    };
    auto segment_point_distance = [seg2, cut_left, cut_right](const Point& p) {
        auto pp_dist = [p](const Point& p1) { return point_point_distance(p, p1); };
        return ternary_search(seg2.first, seg2.second, cut_left, cut_right, pp_dist,std::less<>());
    };

    double result = ternary_search(seg1.first, seg1.second, cut_left, cut_right, segment_point_distance, std::less<>());
    printf("%.16lf", result);

    return 0;
}