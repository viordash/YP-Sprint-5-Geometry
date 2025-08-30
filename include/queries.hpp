#pragma once
#include "geometry.hpp"
#include "intersections.hpp"
#include <algorithm>
#include <optional>
#include <ranges>
#include <variant>

namespace geometry::queries {

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};

template <typename T>
concept IsPolygonOrRegularPolygon = std::is_same_v<T, geometry::Polygon> || std::is_same_v<T, geometry::RegularPolygon>;

/*
 * Класс для поиска расстояния от точки до фигуры
 *
 * Требуется организовать возможность нахождения расстояния для всех возможных фигур типа-суммы Shape
 */
struct PointToShapeDistanceVisitor {
    Point2D point;

    explicit PointToShapeDistanceVisitor(const Point2D &p) : point(p) {}

    [[nodiscard]] double operator()(const Line &line) const {
        const Point2D ab = line.end - line.start;
        const Point2D ap = point - line.start;

        bool line_is_point = ab.Dot(ab) < 1e-9;
        if (line_is_point) {
            return ap.Length();
        }

        double projection = ap.Dot(ab) / ab.Dot(ab);
        projection = std::clamp(projection, 0.0, 1.0);

        const Point2D nearest_point = line.start + ab * projection;
        return point.DistanceTo(nearest_point);
    }

    [[nodiscard]] double operator()(const Circle &circle) const {
        const double dist_to_center = point.DistanceTo(circle.center_p);
        const double dist = dist_to_center - circle.radius;
        bool inside = dist < 0.0;
        if (inside) {
            return 0.0;
        }
        return dist;
    }

    [[nodiscard]] double operator()(const Rectangle &rect) const {
        const double nearest_x = std::clamp(point.x, rect.bottom_left.x, rect.bottom_left.x + rect.width);
        const double nearest_y = std::clamp(point.y, rect.bottom_left.y, rect.bottom_left.y + rect.height);

        return point.DistanceTo({nearest_x, nearest_y});
    }

    [[nodiscard]] double operator()(const Triangle &triangle) const {
        const double c1 = (point - triangle.a).Cross(triangle.b - triangle.a);
        const double c2 = (point - triangle.b).Cross(triangle.c - triangle.b);
        const double c3 = (point - triangle.c).Cross(triangle.a - triangle.c);

        bool inside = (c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0);
        if (inside) {
            return 0.0;
        }

        const double dist_ab = (*this)(Line{triangle.a, triangle.b});
        const double dist_bc = (*this)(Line{triangle.b, triangle.c});
        const double dist_ca = (*this)(Line{triangle.c, triangle.a});

        return std::min({dist_ab, dist_bc, dist_ca});
    }

    template <IsPolygonOrRegularPolygon PolygonType>
    [[nodiscard]] double operator()(const PolygonType &poly) const {
        const auto poly_lines = poly.Lines();
        const auto &x_coords = poly_lines.x;
        const auto &y_coords = poly_lines.y;

        auto dist_to_edges = std::views::iota(0u, x_coords.size() - 1)  //
                             | std::views::transform([&, this](size_t i) {
                                   Line edge{{x_coords[i], y_coords[i]}, {x_coords[i + 1], y_coords[i + 1]}};
                                   return (*this)(edge);  // Вычисляем расстояние от `point` до этого ребра
                               });

        return std::ranges::min(dist_to_edges);
    }
};
/*
 * Класс для поиска расстояния между двумя фигурами
 *
 * Требуется организовать возможность нахождения расстояния только для следующих комбинаций фигур:
 *    - Any    & Point
 *    - Line   & Line
 *    - Circle & Circle
 *
 * Важно: вы можете выбрать любой метод нахождения расстояния, даже если он даёт не точный результат
 *
 * Для всех остальных требуется вернуть пустое значение
 */
struct ShapeToShapeDistanceVisitor {
    [[nodiscard]] std::optional<double> operator()(const Circle &circle1, const Circle &circle2) const {
        PointToShapeDistanceVisitor p_visitor{circle1.center_p};
        return std::max(p_visitor(circle2) - circle1.radius, 0.0);
    }

    [[nodiscard]] std::optional<double> operator()(const Line &line1, const Line &line2) const {
        auto start1_line2 = PointToShapeDistanceVisitor{line1.start}(line2);
        auto end1_line2 = PointToShapeDistanceVisitor{line1.end}(line2);
        auto start2_line1 = PointToShapeDistanceVisitor{line2.start}(line1);
        auto end2_line1 = PointToShapeDistanceVisitor{line2.end}(line1);

        return std::min({start1_line2, end1_line2, start2_line1, end2_line1});
    }

    template <typename T, typename U>
    [[nodiscard]] std::optional<double> operator()(const T &, const U &) const {
        return std::nullopt;
    }
};

/*
 * Функции-помощники
 */
inline double DistanceToPoint(const Shape &shape, const Point2D &point) {
    return std::visit(PointToShapeDistanceVisitor{point}, shape);
}

inline BoundingBox GetBoundBox(const Shape &shape) {

    /* ваш код здесь */
    return {};
}

inline double GetHeight(const Shape &shape) {

    /* ваш код здесь */
    return 0.0;
}

inline bool BoundingBoxesOverlap(const Shape &shape1, const Shape &shape2) {

    /* ваш код здесь */
    return false;
}

inline std::optional<double> DistanceBetweenShapes(const Shape &shape1, const Shape &shape2) {
    if (intersections::GetIntersectPoint(shape1, shape2).has_value()) {
        return 0.0;
    }

    auto visitor = Multilambda{
        [](const Line &line1, const Line &line2) { return ShapeToShapeDistanceVisitor{}(line1, line2); },
        [](const Circle &circle1, const Circle &circle2) { return ShapeToShapeDistanceVisitor{}(circle1, circle2); },

        [](const auto &, const auto &) -> std::optional<double> { return std::nullopt; }};

    return std::visit(visitor, shape1, shape2);
}

}  // namespace geometry::queries