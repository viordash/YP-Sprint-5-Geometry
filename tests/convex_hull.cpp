#include "convex_hull.hpp"
#include "geometry.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using namespace geometry;
using namespace geometry::convex_hull;

TEST(ConvexHullTest, GrahamScan_degenerate_case_if_less_than_3_points) {
    std::vector<Point2D> points = {{1, 1}, {5, 5}};

    auto result = GrahamScan(points);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), GeometryError::DegenrateCase);
}

TEST(ConvexHullTest, GrahamScan_degenerate_case_if_all_points_on_line) {
    std::vector<Point2D> points = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};

    auto result = GrahamScan(points);
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), GeometryError::DegenrateCase);
}

TEST(ConvexHullTest, GrahamScan_triangle) {
    std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 5}};
    std::vector<Point2D> expected_hull = {{0, 0}, {10, 0}, {5, 5}};

    auto result = GrahamScan(points);
    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(result->size(), 3);

    ASSERT_THAT(expected_hull, testing::Contains(result.value()[0]));
    ASSERT_THAT(expected_hull, testing::Contains(result.value()[1]));
    ASSERT_THAT(expected_hull, testing::Contains(result.value()[2]));
}

TEST(ConvexHullTest, GrahamScan_square) {
    std::vector<Point2D> points = {{0, 0}, {5, 0}, {5, 5}, {0, 5}, {2, 2}};
    std::vector<Point2D> expected_hull = {{0, 0}, {5, 0}, {5, 5}, {0, 5}};

    auto result = GrahamScan(points);
    ASSERT_TRUE(result.has_value());

    ASSERT_EQ(result->size(), 4);

    ASSERT_THAT(expected_hull, testing::Contains(result.value()[0]));
    ASSERT_THAT(expected_hull, testing::Contains(result.value()[1]));
    ASSERT_THAT(expected_hull, testing::Contains(result.value()[2]));
    ASSERT_THAT(expected_hull, testing::Contains(result.value()[3]));
}