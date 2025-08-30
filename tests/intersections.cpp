#include "intersections.hpp"
#include "geometry.hpp"
#include <gtest/gtest.h>

using namespace geometry;
using namespace geometry::intersections;

TEST(IntersectionTest, LineLine_intersecting) {
    Line line1{Point2D{0, 0}, Point2D{2, 2}};
    Line line2{Point2D{0, 2}, Point2D{2, 0}};
    auto result = GetIntersectPoint(line1, line2);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->x, 1.0, 1e-9);
    ASSERT_NEAR(result->y, 1.0, 1e-9);
}

TEST(IntersectionTest, LineLine_parallel) {
    Line line1{Point2D{0, 0}, Point2D{2, 2}};
    Line line2{Point2D{0, 1}, Point2D{2, 3}};
    auto result = GetIntersectPoint(line1, line2);
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionTest, LineLine_non_intersecting) {
    Line line1{Point2D{0, 0}, Point2D{2, 2}};
    Line line2{Point2D{3, 0}, Point2D{5, 2}};
    auto result = GetIntersectPoint(line1, line2);
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionTest, LineCircle_intersecting) {
    Line line{Point2D{0, 0}, Point2D{4, 0}};
    Circle circle{Point2D{2, 0}, 1.0};
    auto result = GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->x, 1.0, 1e-9);
    ASSERT_NEAR(result->y, 0.0, 1e-9);
}

TEST(IntersectionTest, LineCircle_tangent) {
    Line line{Point2D{0, 1}, Point2D{4, 1}};
    Circle circle{Point2D{2, 0}, 1.0};
    auto result = GetIntersectPoint(line, circle);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->x, 2.0, 1e-9);
    ASSERT_NEAR(result->y, 1.0, 1e-9);
}

TEST(IntersectionTest, LineCircle_no_intersection) {
    Line line{Point2D{0, 2}, Point2D{4, 2}};
    Circle circle{Point2D{2, 0}, 1.0};
    auto result = GetIntersectPoint(line, circle);
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionTest, CircleLine_reverse_order) {
    Line line{Point2D{0, 0}, Point2D{4, 0}};
    Circle circle{Point2D{2, 0}, 1.0};
    auto result = GetIntersectPoint(circle, line);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->x, 1.0, 1e-9);
    ASSERT_NEAR(result->y, 0.0, 1e-9);
}

TEST(IntersectionTest, CircleCircle_intersecting) {
    Circle circle1{Point2D{0, 0}, 2.0};
    Circle circle2{Point2D{3, 0}, 2.0};
    auto result = GetIntersectPoint(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->DistanceTo(circle1.center_p), circle1.radius, 1e-9);
    ASSERT_NEAR(result->DistanceTo(circle2.center_p), circle2.radius, 1e-9);
}

TEST(IntersectionTest, CircleCircle_tangent) {
    Circle circle1{Point2D{0, 0}, 1.0};
    Circle circle2{Point2D{2, 0}, 1.0};
    auto result = GetIntersectPoint(circle1, circle2);
    ASSERT_TRUE(result.has_value());
    ASSERT_NEAR(result->x, 1.0, 1e-9);
    ASSERT_NEAR(result->y, 0.0, 1e-9);
}

TEST(IntersectionTest, CircleCircle_no_intersection) {
    Circle circle1{Point2D{0, 0}, 1.0};
    Circle circle2{Point2D{3, 0}, 1.0};
    auto result = GetIntersectPoint(circle1, circle2);
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionTest, UnsupportedTypes) {
    Triangle triangle{Point2D{0, 0}, Point2D{3, 0}, Point2D{1.5, 3}};
    Rectangle rect{{1.0, 1.0}, 2.0, 2.0};
    RegularPolygon regular_polygon({2.0, 2.0}, 1.5, 4);
    std::vector<Point2D> points = {{0.5, 0.5}, {2.5, 0.5}, {2.5, 2.5}, {0.5, 2.5}};
    Polygon polygon(points);

    std::vector<Shape> unsupported_shapes = {triangle, rect, regular_polygon, polygon};
    auto pairs = std::views::cartesian_product(unsupported_shapes, unsupported_shapes);
    std::ranges::for_each(pairs, [](const auto &pair) {
        const auto &[shape1, shape2] = pair;
        ASSERT_FALSE(GetIntersectPoint(shape1, shape2).has_value());
    });
}