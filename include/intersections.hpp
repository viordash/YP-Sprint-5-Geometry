#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>

namespace geometry::intersections {

/*
 * Класс для поиска пересечений между двумя фигурами
 *
 * Требуется организовать возможность нахождения пересечений только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Line & Circle (и Circle & Line)
 *    - Circle & Circle
 *
 * Для всех остальных требуется вернуть std::nullopt
 */
class IntersectionVisitor {
    const double epsilon = 1e-9;

public:
    [[nodiscard]] std::optional<Point2D> operator()(const Line &line1, const Line &line2) const {
        const Point2D p1 = line1.start;
        const Point2D d1 = line1.end - line1.start;
        const Point2D p2 = line2.start;
        const Point2D d2 = line2.end - line2.start;

        const double cross = d1.Cross(d2);

        bool parallel = std::abs(cross) < epsilon;
        if (parallel) {
            return std::nullopt;
        }

        const double t = (p2 - p1).Cross(d2) / cross;
        const double u = (p2 - p1).Cross(d1) / cross;

        if (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
            return p1 + d1 * t;
        }

        return std::nullopt;
    }

    [[nodiscard]] std::optional<Point2D> operator()(const Circle &c1, const Circle &c2) const {
        const double r1 = c1.radius;
        const double r2 = c2.radius;
        const Point2D diff = c1.center_p - c2.center_p;
        const double dist_sq = diff.Dot(diff);
        const double dist = std::sqrt(dist_sq);

        if (dist > r1 + r2 + epsilon || dist < std::abs(r1 - r2) - epsilon) {
            return std::nullopt;
        }

        const double a = (r1 * r1 - r2 * r2 + dist_sq) / (2.0 * dist);
        const double h_sq = r1 * r1 - a * a;
        if (h_sq < -epsilon) {
            return std::nullopt;
        }
        const double h = h_sq > 0 ? std::sqrt(h_sq) : 0.0;

        const Point2D p2 = c1.center_p + (c2.center_p - c1.center_p) * (a / dist);
        return Point2D{p2.x + h * (c2.center_p.y - c1.center_p.y) / dist,
                       p2.y - h * (c2.center_p.x - c1.center_p.x) / dist};
    }

    [[nodiscard]] std::optional<Point2D> operator()(const Line &line, const Circle &circle) const {
        const Point2D d = line.end - line.start;
        const Point2D f = line.start - circle.center_p;

        const double a = d.Dot(d);
        if (a < epsilon) {
            return (f.Dot(f) - circle.radius * circle.radius < epsilon) ? std::optional<Point2D>(line.start)
                                                                        : std::nullopt;
        }

        const double b = 2.0 * f.Dot(d);
        const double c = f.Dot(f) - circle.radius * circle.radius;
        double discriminant = b * b - 4 * a * c;

        if (discriminant < -epsilon) {
            return std::nullopt;
        }
        discriminant = std::max(0.0, discriminant);
        const double sqrt_d = std::sqrt(discriminant);
        const double t1 = (-b - sqrt_d) / (2 * a);
        const double t2 = (-b + sqrt_d) / (2 * a);

        if (t1 >= 0.0 && t1 <= 1.0) {
            return line.start + d * t1;
        }
        if (t2 >= 0.0 && t2 <= 1.0) {
            return line.start + d * t2;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<Point2D> operator()(const Circle &circle, const Line &line) const {
        return (*this)(line, circle);
    }

    template <typename T, typename U>
    [[nodiscard]] std::optional<Point2D> operator()(const T &, const U &) const {
        return std::nullopt;
    }
};

inline std::optional<Point2D> GetIntersectPoint(const Shape &shape1, const Shape &shape2) {
    return std::visit(IntersectionVisitor{}, shape1, shape2);
}

}  // namespace geometry::intersections