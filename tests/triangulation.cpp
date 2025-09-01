#include "triangulation.hpp"
#include "geometry.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using namespace geometry;
using namespace geometry::triangulation;

TEST(TriangulationTest, Delaunay_insufficient_points) {
    std::vector<Point2D> points = {{0, 0}, {1, 1}};
    auto result = DelaunayTriangulation(points);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), GeometryError::InsufficientPoints);
}

TEST(TriangulationTest, Delaunay_triangle) {
    std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 5}};
    auto result = DelaunayTriangulation(points);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 1);
    ASSERT_EQ(result.value()[0].a, points[0]);
    ASSERT_EQ(result.value()[0].b, points[1]);
    ASSERT_EQ(result.value()[0].c, points[2]);
}

TEST(TriangulationTest, Delaunay_square) {
    std::vector<Point2D> points = {{0, 0}, {5, 0}, {5, 5}, {0, 5}};
    auto result = DelaunayTriangulation(points);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->size(), 2);

    ASSERT_EQ(result.value()[0].a, points[0]);
    ASSERT_EQ(result.value()[0].b, points[1]);
    ASSERT_EQ(result.value()[0].c, points[3]);

    ASSERT_EQ(result.value()[1].a, points[1]);
    ASSERT_EQ(result.value()[1].b, points[2]);
    ASSERT_EQ(result.value()[1].c, points[3]);
}