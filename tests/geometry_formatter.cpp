#include "geometry.hpp"
#include <gtest/gtest.h>

using namespace geometry;

TEST(FormatterTest, VectorPoint2DFormatter_default) {
    std::vector<Point2D> points_empty;
    std::string formatted_empty = std::format("{}", points_empty);
    ASSERT_EQ(formatted_empty, "[]");

    std::vector<Point2D> points_single = {{1.1, 2.2}};
    std::string formatted_single = std::format("{}", points_single);
    ASSERT_EQ(formatted_single, "[(1.10, 2.20)]");

    std::vector<Point2D> points_multiple = {{1.1, 2.2}, {3.3, 4.4}, {5.5, 6.6}};
    std::string formatted_multiple = std::format("{}", points_multiple);
    ASSERT_EQ(formatted_multiple, "[(1.10, 2.20), (3.30, 4.40), (5.50, 6.60)]");
}

TEST(FormatterTest, VectorPoint2DFormatter_new_line) {
    std::vector<Point2D> points_empty;
    std::string formatted_empty = std::format("{:new_line}", points_empty);
    ASSERT_EQ(formatted_empty, "[\n]");

    std::vector<Point2D> points_single = {{1.1, 2.2}};
    std::string formatted_single = std::format("{:new_line}", points_single);
    ASSERT_EQ(formatted_single, "[\n\t(1.10, 2.20)\n]");

    std::vector<Point2D> points_multiple = {{1.1, 2.2}, {3.3, 4.4}};
    std::string formatted_multiple = std::format("{:new_line}", points_multiple);
    ASSERT_EQ(formatted_multiple, "[\n\t(1.10, 2.20),\n\t(3.30, 4.40)\n]");
}

TEST(FormatterTest, VectorPoint2DFormatter_invalid_specifier) {
    std::vector<Point2D> points = {{1.1, 2.2}, {3.3, 4.4}};

    std::string formatted_invalid = std::format("{:some_other_spec}", points);
    ASSERT_EQ(formatted_invalid, "[(1.10, 2.20), (3.30, 4.40)]");
}