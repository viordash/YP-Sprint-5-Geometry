#include "queries.hpp"
#include "geometry.hpp"
#include <gtest/gtest.h>

using namespace geometry;
using namespace geometry::queries;

TEST(QueriesTest, DistanceToPoint_Line) {
    Shape line_shape = Line{{0, 0}, {4, 4}};

    Point2D point_nearest_to_2_2{4, 0};
    ASSERT_NEAR(DistanceToPoint(line_shape, point_nearest_to_2_2), std::sqrt(8.0), 1e-9);

    Point2D point_nearest_to_start{-1, -1};
    ASSERT_NEAR(DistanceToPoint(line_shape, point_nearest_to_start), std::sqrt(2.0), 1e-9);

    Point2D point_nearest_to_end{5, 5};
    ASSERT_NEAR(DistanceToPoint(line_shape, point_nearest_to_end), std::sqrt(2.0), 1e-9);

    Point2D point_on_line{1, 1};
    ASSERT_NEAR(DistanceToPoint(line_shape, point_on_line), 0.0, 1e-9);
}

TEST(QueriesTest, DistanceToPoint_Circle) {
    Shape circle_shape = Circle{{0, 0}, 5.0};

    Point2D point_outside{10, 0};
    ASSERT_NEAR(DistanceToPoint(circle_shape, point_outside), 5.0, 1e-9);

    Point2D point_tangent{5, 0};
    ASSERT_NEAR(DistanceToPoint(circle_shape, point_tangent), 0.0, 1e-9);

    Point2D point_inside{1, 1};
    ASSERT_NEAR(DistanceToPoint(circle_shape, point_inside), 0.0, 1e-9);
}

TEST(QueriesTest, DistanceToPoint_Rectangle) {
    Shape rect_shape = Rectangle{{0, 0}, 10, 5};

    Point2D point_inside{2, 2};
    ASSERT_NEAR(DistanceToPoint(rect_shape, point_inside), 0.0, 1e-9);

    Point2D point_outside{12, 3};
    ASSERT_NEAR(DistanceToPoint(rect_shape, point_outside), 2.0, 1e-9);
}

TEST(QueriesTest, DistanceToPoint_Triangle) {
    Shape triangle_shape = Triangle{{0, 0}, {4, 0}, {2, 4}};

    Point2D point_inside{2, 1};
    ASSERT_NEAR(DistanceToPoint(triangle_shape, point_inside), 0.0, 1e-9);

    Point2D point_outside{2, -2};
    ASSERT_NEAR(DistanceToPoint(triangle_shape, point_outside), 2.0, 1e-9);
}

TEST(QueriesTest, DistanceToPoint_RegularPolygon) {
    Shape hexagon = RegularPolygon{{0, 0}, 2.0, 6};

    Point2D point_outside{3, 0};
    ASSERT_NEAR(DistanceToPoint(hexagon, point_outside), 1.0, 1e-9);
}

TEST(QueriesTest, DistanceToPoint_Polygon) {
    std::vector<Point2D> points = {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};
    Shape square = Polygon{points};

    Point2D point_outside{2, 0};
    ASSERT_NEAR(DistanceToPoint(square, point_outside), 1.0, 1e-9);
}