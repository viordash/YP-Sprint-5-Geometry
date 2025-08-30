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

TEST(QueriesTest, DistanceBetweenShapes_Lines) {
    Shape parallel_line1 = Line{{0, 0}, {5, 0}};
    Shape parallel_line2 = Line{{0, 2}, {5, 2}};
    auto dist1 = DistanceBetweenShapes(parallel_line1, parallel_line2);
    ASSERT_TRUE(dist1.has_value());
    ASSERT_NEAR(*dist1, 2.0, 1e-9);

    Shape cross_line1 = Line{{-1, -1}, {1, 1}};
    Shape cross_line2 = Line{{-1, 1}, {1, -1}};
    auto dist2 = DistanceBetweenShapes(cross_line1, cross_line2);
    ASSERT_TRUE(dist2.has_value());
    ASSERT_NEAR(*dist2, 0.0, 1e-9);

    Shape line1 = Line{{0, 0}, {2, 0}};
    Shape line2 = Line{{3, 1}, {3, 3}};
    auto dist3 = DistanceBetweenShapes(line1, line2);
    ASSERT_TRUE(dist3.has_value());
    ASSERT_NEAR(*dist3, std::sqrt(2.0), 1e-9);
}

TEST(QueriesTest, DistanceBetweenShapes_Circles) {
    Shape far_circle1 = Circle{{0, 0}, 2.0};
    Shape far_circle2 = Circle{{10, 0}, 3.0};
    auto dist1 = DistanceBetweenShapes(far_circle1, far_circle2);
    ASSERT_TRUE(dist1.has_value());
    ASSERT_NEAR(*dist1, 5.0, 1e-9);

    Shape tangent_circle1 = Circle{{0, 0}, 2.0};
    Shape tangent_circle2 = Circle{{5, 0}, 3.0};
    auto dist2 = DistanceBetweenShapes(tangent_circle1, tangent_circle2);
    ASSERT_TRUE(dist2.has_value());
    ASSERT_NEAR(*dist2, 0.0, 1e-9);

    Shape cross_circle1 = Circle{{0, 0}, 3.0};
    Shape cross_circle2 = Circle{{4, 0}, 3.0};
    auto dist3 = DistanceBetweenShapes(cross_circle1, cross_circle2);
    ASSERT_TRUE(dist3.has_value());
    ASSERT_NEAR(*dist3, 0.0, 1e-9);

    Shape external_circle = Circle{{0, 0}, 5.0};
    Shape internal_circle = Circle{{1, 0}, 1.0};
    auto dist4 = DistanceBetweenShapes(external_circle, internal_circle);
    ASSERT_TRUE(dist4.has_value());
    ASSERT_NEAR(*dist4, 0.0, 1e-9);
}

TEST(QueriesTest, DistanceBetweenShapes_Unsupported) {
    Shape line = Line{{0, 0}, {1, 1}};
    Shape circle = Circle{{10, 10}, 5};
    Shape rect = Rectangle{{0, 0}, 2, 2};
    Shape triangle = Triangle{{0, 0}, {1, 0}, {0, 1}};

    auto dist1 = DistanceBetweenShapes(line, circle);
    ASSERT_FALSE(dist1.has_value());

    auto dist2 = DistanceBetweenShapes(rect, triangle);
    ASSERT_FALSE(dist2.has_value());

    auto dist3 = DistanceBetweenShapes(circle, rect);
    ASSERT_FALSE(dist3.has_value());
}