#include <iostream>
#include <vector>
#include <cfloat>
#include <algorithm>
#include <cmath>
#include <complex>



struct Point {
    double x = 0;
    double y = 0;

    Point& operator+=(const Point& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    const Point operator+(const Point& other) const {
        Point result(*this);
        result += other;
        return result;
    }
    Point& operator-=(const Point& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    const Point operator-(const Point& other) const {
        Point result(*this);
        result -= other;
        return result;
    }
};

using vector = std::pair<Point, Point>;

using Polygon = std::vector<Point>;


int top_rigth_point_index(const Polygon& polygon) {
    double max_x = -DBL_MAX;
    double max_y = -DBL_MAX;
    int result = 0;

    for (int i = 0; i < polygon.size(); ++i) {
        if (polygon[i].y > max_y || (polygon[i].y == max_y && polygon[i].x > max_x)) {
            max_y = polygon[i].y;
            max_x = polygon[i].x;
            result = i;
        }
    }
    return result;
}

bool is_angle_greater(vector v1, vector v2) {
    Point vec1_pnt = v1.second - v1.first;
    Point vec2_pnt = v2.second - v2.first;

    bool result;

    std::complex<double> p1(vec1_pnt.x, vec1_pnt.y);
    std::complex<double> p2(vec2_pnt.x, vec2_pnt.y);
    double ang1 = std::arg(p1);
    double ang2 = std::arg(p2);
    ang1 < 0 ? ang1 += 2*M_PI : ang1;
    ang2 < 0 ? ang2 += 2*M_PI : ang2;

    result = ang1 > ang2;

    return result;
}


Polygon minkowski_sum(const Polygon& p1, const Polygon& p2) {

    int start1 = top_rigth_point_index(p1);
    int start2 = top_rigth_point_index(p2);

    int n = p1.size();
    int m = p2.size();
    Polygon result;

    int i = 0;
    int j = 0;
    while(i < p1.size() && j < p2.size()) {
        result.push_back(p1[(i + start1) % n] + p2[(j + start2) % m]);

        vector v1 = {p1[(i + start1) % n], p1[(i + 1 + start1) % n]};
        vector v2 = {p2[(j + start2) % m], p2[(j + 1 + start2) % m]};
        if (is_angle_greater(v1, v2)) {
            ++i;
        }
        else if (is_angle_greater(v2, v1)) {
               ++j;
        }
        else {
            ++i;
            ++j;
        }
    }
    while (i != p1.size()) {
        result.push_back(p1[(i + start1) % n] + p2[(j + start2) % m]);
        ++i;
    }
    while (j != p2.size()) {
        result.push_back(p1[(i + start1) % n] + p2[(j + start2) % m]);
        ++j;
    }


    return result;
}

double vector_product(Point p1, Point p2) {
    // Если больше или равен нулю, то true, иначе false
    return p1.x * p2.y - p1.y * p2.x;
}

bool does_belong_to_polygon(Point p, const Polygon& polygon) {
    Point p_vec; // Радиус-вектор из p в текущую точку многоугольника
    Point edge; // вектор, соответствующий ребру мн-ка в направлении по часовой стрелке обхода
    bool result = true;
    for (int i = 0; i < polygon.size(); ++i) {
        if (polygon[i].x == 0 && polygon[i].y == 0) {
            result = true;
            break;
        }

        p_vec = polygon[i];
        int next = i + 1 != polygon.size() ? i + 1 : 0;
        edge = polygon[next] - polygon[i];

        if (vector_product(p_vec, edge) > 0) {
            result = false;
        }
    }
    return result;
}

bool whether_polygons_intersect(const Polygon& polygon1, const Polygon& polygon2) {
    Polygon reversed_p2(polygon2);

    bool result = false;

    std::for_each(reversed_p2.begin(), reversed_p2.end(), [](Point& point){ 
        point.x *= -1; point.y *= -1;
    });

    Polygon mink_sum = minkowski_sum(polygon1, reversed_p2);

    if (does_belong_to_polygon(Point{0,0}, mink_sum)) {
        result = true;
    }

    return result;
}

int main() {

    int n;
    std::cin >> n;
    Polygon first_polygon(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> first_polygon[i].x >> first_polygon[i].y;
    }

    int m;
    std::cin >> m;
    Polygon second_polygon(m);
    for (int i = 0; i < m; ++i) {
        std::cin >> second_polygon[i].x >> second_polygon[i].y;
    }

    if (whether_polygons_intersect(first_polygon, second_polygon)) {
        std::cout << "YES";
    }
    else {
        std::cout << "NO";
    }

    return 0;
}
