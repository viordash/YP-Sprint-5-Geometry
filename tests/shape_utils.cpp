#include "shape_utils.hpp"
#include "geometry.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace geometry;
using namespace geometry::utils;

TEST(ShapeUtilsTest, FindAllCollisions_no_shapes) {
    std::vector<Shape> shapes;
    auto collisions = FindAllCollisions(shapes);
    ASSERT_TRUE(collisions.empty());
}

TEST(ShapeUtilsTest, FindAllCollisions_single_shape) {
    std::vector<Shape> shapes = {Rectangle{{0, 0}, 5, 5}};
    auto collisions = FindAllCollisions(shapes);
    ASSERT_TRUE(collisions.empty());
}

TEST(ShapeUtilsTest, FindAllCollisions_no_collisions) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 2, 2},  //
        Circle{{10, 10}, 1},      //
        Line{{-5, -5}, {-4, -4}}  //
    };
    auto collisions = FindAllCollisions(shapes);
    ASSERT_TRUE(collisions.empty());
}

TEST(ShapeUtilsTest, FindAllCollisions_simple) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 5, 5},  //
        Circle{{4, 4}, 2}         //
    };
    auto collisions = FindAllCollisions(shapes);
    ASSERT_EQ(collisions.size(), 1);
}

TEST(ShapeUtilsTest, FindAllCollisions_multiple) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 5, 5},     //
        Circle{{4, 4}, 2},           //
        Line{{5, 0}, {10, 5}},       //
        Rectangle{{100, 100}, 1, 1}  //
    };

    auto collisions = FindAllCollisions(shapes);
    ASSERT_EQ(collisions.size(), 3);
}

TEST(ShapeUtilsTest, FindAllCollisions_tangent) {
    std::vector<Shape> shapes = {
        Rectangle{{0, 0}, 5, 5},  //
        Rectangle{{5, 0}, 5, 5}   //
    };
    auto collisions = FindAllCollisions(shapes);
    ASSERT_EQ(collisions.size(), 1);
}