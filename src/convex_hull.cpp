#include "convex_hull.hpp"
#include <algorithm>
#include <vector>

namespace geometry::convex_hull {

double CrossProduct(Point2D p1, Point2D middle, Point2D p2) {
    auto new_p1 = p1 - middle;
    auto new_p2 = p2 - middle;
    return new_p1.Cross(new_p2);
}

GeometryResult<std::vector<Point2D>> GrahamScan(std::span<const Point2D> points) {
    const double epsilon = 1e-9;

    if (points.size() < 3) {
        return std::unexpected{GeometryError::DegenrateCase};
    }

    std::vector<Point2D> sorted_points(points.begin(), points.end());

    auto p0_it = std::ranges::min_element(
        sorted_points, [](const Point2D &a, const Point2D &b) { return (a.y < b.y) || (a.y == b.y && a.x < b.x); });
    std::iter_swap(sorted_points.begin(), p0_it);
    const Point2D p0 = sorted_points[0];

    auto tail_view = sorted_points | std::views::drop(1);
    std::ranges::sort(tail_view, [&](const Point2D &a, const Point2D &b) {
        const double cross = CrossProduct(p0, a, b);

        bool farthest_one = std::abs(cross) < epsilon;
        if (farthest_one) {
            return p0.DistanceTo(a) > p0.DistanceTo(b);
        }

        return cross > 0;
    });

    auto last = std::unique(sorted_points.begin() + 1, sorted_points.end(), [&](const Point2D &a, const Point2D &b) {
        return std::abs(CrossProduct(p0, a, b)) < epsilon;
    });
    sorted_points.erase(last, sorted_points.end());

    if (sorted_points.size() < 3) {
        return std::unexpected{GeometryError::DegenrateCase};
    }

    StackForGrahamScan stack;
    stack.Push(sorted_points[0]);
    stack.Push(sorted_points[1]);

    for (const auto &point : sorted_points | std::views::drop(2)) {
        while (stack.Size() > 1 && CrossProduct(stack.NextToTop(), stack.Top(), point) <= 0) {
            stack.Pop();
        }
        stack.Push(point);
    }

    return std::move(stack).Extract();
}

}  // namespace geometry::convex_hull