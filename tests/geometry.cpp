#include "geometry.hpp"
#include <gtest/gtest.h>

using namespace geometry;

TEST(GeometryTest, Point2D_ctor) {
    Point2D p_default;
    ASSERT_DOUBLE_EQ(p_default.x, 0.0);
    ASSERT_DOUBLE_EQ(p_default.y, 0.0);

    Point2D p_parameterized(3.5, -2.1);
    ASSERT_DOUBLE_EQ(p_parameterized.x, 3.5);
    ASSERT_DOUBLE_EQ(p_parameterized.y, -2.1);
}

TEST(GeometryTest, Point2D_equality) {
    Point2D p1(1.0, 2.0);
    Point2D p2(1.0, 2.0);
    Point2D p3(1.0, 2.1);

    ASSERT_TRUE(p1 == p2);
    ASSERT_FALSE(p1 == p3);
}

TEST(GeometryTest, Point2D_less) {
    Point2D p1(1.0, 2.0);
    Point2D p2(1.0, 3.0);
    Point2D p3(2.0, 2.0);
    Point2D p4(-1.0, -2.0);

    ASSERT_TRUE(p1 < p2);
    ASSERT_TRUE(p1 < p3);
    ASSERT_TRUE(p4 < p1);
}

TEST(GeometryTest, Point2D_arithmetic) {
    Point2D p1(1.0, 2.0);
    Point2D p2(3.0, 4.0);

    Point2D sum = p1 + p2;
    ASSERT_DOUBLE_EQ(sum.x, 4.0);
    ASSERT_DOUBLE_EQ(sum.y, 6.0);

    Point2D sub = p2 - p1;
    ASSERT_DOUBLE_EQ(sub.x, 2.0);
    ASSERT_DOUBLE_EQ(sub.y, 2.0);

    Point2D mul = p1 * 2.0;
    ASSERT_DOUBLE_EQ(mul.x, 2.0);
    ASSERT_DOUBLE_EQ(mul.y, 4.0);

    Point2D div = p2 / 2.0;
    ASSERT_DOUBLE_EQ(div.x, 1.5);
    ASSERT_DOUBLE_EQ(div.y, 2.0);
}

TEST(GeometryTest, Point2D_Dot) {
    Point2D p1(1.0, 2.0);
    Point2D p2(3.0, 4.0);

    double dot = p1.Dot(p2);
    ASSERT_DOUBLE_EQ(dot, 11.0);  // 1*3 + 2*4
}

TEST(GeometryTest, Point2D_Cross) {
    Point2D p1(1.0, 2.0);
    Point2D p2(3.0, 4.0);

    double cross = p1.Cross(p2);
    ASSERT_DOUBLE_EQ(cross, -2.0);  // 1*4 - 2*3
}

TEST(GeometryTest, Point2D_Length) {
    Point2D p(3.0, 4.0);
    ASSERT_DOUBLE_EQ(p.Length(), 5.0);

    Point2D p1(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    ASSERT_TRUE(std::isinf(p1.Length()));

    Point2D p2(std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
    ASSERT_DOUBLE_EQ(p2.Length(), 0.0);

    Point2D p3(std::numeric_limits<double>::infinity(), 0.0);
    ASSERT_TRUE(std::isinf(p3.Length()));

    Point2D zero(0.0, 0.0);
    ASSERT_DOUBLE_EQ(zero.Length(), 0.0);
}

TEST(GeometryTest, Point2D_DistanceTo) {
    Point2D p1(1.0, 2.0);
    Point2D p2(4.0, 6.0);

    ASSERT_DOUBLE_EQ(p1.DistanceTo(p2), 5.0);
}

TEST(GeometryTest, Point2D_Normalize) {
    Point2D p(3.0, 4.0);
    Point2D normalized = p.Normalize();

    ASSERT_DOUBLE_EQ(normalized.Length(), 1.0);
    ASSERT_DOUBLE_EQ(normalized.x, 0.6);
    ASSERT_DOUBLE_EQ(normalized.y, 0.8);

    Point2D p_zero(0.0, 0.0);
    Point2D zero_normalized = p_zero.Normalize();
    ASSERT_DOUBLE_EQ(zero_normalized.x, 0.0);
    ASSERT_DOUBLE_EQ(zero_normalized.y, 0.0);
}

TEST(GeometryTest, Lines2DDyn_ctor) {
    Lines2DDyn lines;

    ASSERT_TRUE(lines.x.empty());
    ASSERT_TRUE(lines.y.empty());
}

TEST(GeometryTest, Lines2DDyn_Reserve) {
    Lines2DDyn lines;
    lines.Reserve(10);

    ASSERT_GE(lines.x.capacity(), 10);
    ASSERT_GE(lines.y.capacity(), 10);
    ASSERT_TRUE(lines.x.empty());
    ASSERT_TRUE(lines.y.empty());
}

TEST(GeometryTest, Lines2DDyn_PushBack) {
    Lines2DDyn lines;
    Point2D point(1.0, 2.0);

    lines.PushBack(point);
    lines.PushBack(3.0, 4.0);

    ASSERT_EQ(lines.x.size(), 2);
    ASSERT_EQ(lines.y.size(), 2);
    ASSERT_DOUBLE_EQ(lines.x[0], 1.0);
    ASSERT_DOUBLE_EQ(lines.y[0], 2.0);
    ASSERT_DOUBLE_EQ(lines.x[1], 3.0);
    ASSERT_DOUBLE_EQ(lines.y[1], 4.0);
}

TEST(GeometryTest, Lines2DDyn_Front) {
    Lines2DDyn lines;

    lines.PushBack(1.0, 2.0);
    lines.PushBack(3.0, 4.0);

    ASSERT_DOUBLE_EQ(lines.Front().x, 1.0);
    ASSERT_DOUBLE_EQ(lines.Front().y, 2.0);
}

TEST(GeometryTest, BoundingBox_Overlaps) {
    BoundingBox bbox{1.0, 1.0, 5.0, 5.0};
    BoundingBox bbox_complete_intersection{3.0, 3.0, 7.0, 7.0};
    ASSERT_TRUE(bbox.Overlaps(bbox_complete_intersection));
    ASSERT_TRUE(bbox_complete_intersection.Overlaps(bbox));

    BoundingBox bbox_touching{5.0, 1.0, 7.0, 5.0};
    ASSERT_TRUE(bbox.Overlaps(bbox_touching));
    ASSERT_TRUE(bbox_touching.Overlaps(bbox));

    BoundingBox bbox_wo_intersection{6.0, 6.0, 8.0, 8.0};
    ASSERT_FALSE(bbox.Overlaps(bbox_wo_intersection));
    ASSERT_FALSE(bbox_wo_intersection.Overlaps(bbox));

    BoundingBox bbox_inside{2.0, 2.0, 4.0, 4.0};
    ASSERT_TRUE(bbox.Overlaps(bbox_inside));
    ASSERT_TRUE(bbox_inside.Overlaps(bbox));

    BoundingBox bbox_neg0{-3.0, -3.0, -1.0, -1.0};
    BoundingBox bbox_neg1{-2.0, -2.0, 0.0, 0.0};
    ASSERT_TRUE(bbox_neg0.Overlaps(bbox_neg1));
    ASSERT_TRUE(bbox_neg1.Overlaps(bbox_neg0));
}

TEST(GeometryTest, BoundingBox_Width) {
    BoundingBox bbox{1.0, 2.0, 5.0, 6.0};
    ASSERT_DOUBLE_EQ(bbox.Width(), 4.0);

    BoundingBox bbox_zero_width{3.0, 2.0, 3.0, 6.0};
    ASSERT_DOUBLE_EQ(bbox_zero_width.Width(), 0.0);

    BoundingBox bbox_neg{-5.0, 2.0, -1.0, 6.0};
    ASSERT_DOUBLE_EQ(bbox_neg.Width(), 4.0);
}

TEST(GeometryTest, BoundingBox_Height) {
    BoundingBox bbox1{1.0, 2.0, 5.0, 6.0};
    ASSERT_DOUBLE_EQ(bbox1.Height(), 4.0);

    BoundingBox bbox_zero_height{1.0, 3.0, 5.0, 3.0};
    ASSERT_DOUBLE_EQ(bbox_zero_height.Height(), 0.0);

    BoundingBox bbox_neg{1.0, -6.0, 5.0, -2.0};
    ASSERT_DOUBLE_EQ(bbox_neg.Height(), 4.0);
}

TEST(GeometryTest, BoundingBox_Center) {
    BoundingBox bbox{1.0, 2.0, 5.0, 6.0};
    Point2D center = bbox.Center();
    ASSERT_DOUBLE_EQ(center.x, 3.0);
    ASSERT_DOUBLE_EQ(center.y, 4.0);

    BoundingBox bbox_neg{-5.0, -6.0, -1.0, -2.0};
    center = bbox_neg.Center();
    ASSERT_DOUBLE_EQ(center.x, -3.0);
    ASSERT_DOUBLE_EQ(center.y, -4.0);

    BoundingBox bbox_zero{3.0, 3.0, 3.0, 3.0};
    center = bbox_zero.Center();
    ASSERT_DOUBLE_EQ(center.x, 3.0);
    ASSERT_DOUBLE_EQ(center.y, 3.0);
}

TEST(GeometryTest, BoundingBox_with_number_limits) {
    BoundingBox bbox_huge{0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    ASSERT_TRUE(std::isinf(bbox_huge.Width()) || bbox_huge.Width() == std::numeric_limits<double>::max());
    ASSERT_TRUE(std::isinf(bbox_huge.Height()) || bbox_huge.Height() == std::numeric_limits<double>::max());

    BoundingBox bbox_inf{0.0, 0.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
    ASSERT_TRUE(std::isinf(bbox_inf.Width()));
    ASSERT_TRUE(std::isinf(bbox_inf.Height()));
}