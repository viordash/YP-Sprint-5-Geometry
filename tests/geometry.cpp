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

TEST(GeometryTest, Line_ctor) {
    Point2D start(1.0, 2.0);
    Point2D end(4.0, 6.0);
    Line line{start, end};

    ASSERT_DOUBLE_EQ(line.start.x, 1.0);
    ASSERT_DOUBLE_EQ(line.start.y, 2.0);
    ASSERT_DOUBLE_EQ(line.end.x, 4.0);
    ASSERT_DOUBLE_EQ(line.end.y, 6.0);
}

TEST(GeometryTest, Line_Length) {
    Line line_normal{{1.0, 2.0}, {4.0, 6.0}};
    ASSERT_DOUBLE_EQ(line_normal.Length(), 5.0);  // √((4-1)² + (6-2)²) = 5

    Line line_horz{{1.0, 2.0}, {5.0, 2.0}};
    ASSERT_DOUBLE_EQ(line_horz.Length(), 4.0);

    Line line_vert{{1.0, 2.0}, {1.0, 6.0}};
    ASSERT_DOUBLE_EQ(line_vert.Length(), 4.0);

    Line line_point{{3.0, 4.0}, {3.0, 4.0}};
    ASSERT_DOUBLE_EQ(line_point.Length(), 0.0);

    Line line_neg{{-1.0, -2.0}, {-4.0, -6.0}};
    ASSERT_DOUBLE_EQ(line_neg.Length(), 5.0);
}

TEST(GeometryTest, Line_Direction) {
    Line line_normal{{1.0, 2.0}, {4.0, 6.0}};
    Point2D dir_normal = line_normal.Direction();
    ASSERT_DOUBLE_EQ(dir_normal.x, 0.6);
    ASSERT_DOUBLE_EQ(dir_normal.y, 0.8);
    ASSERT_DOUBLE_EQ(dir_normal.Length(), 1.0);

    Line line_horz{{1.0, 2.0}, {5.0, 2.0}};
    Point2D dir_horz = line_horz.Direction();
    ASSERT_DOUBLE_EQ(dir_horz.x, 1.0);
    ASSERT_DOUBLE_EQ(dir_horz.y, 0.0);

    Line line_vert{{1.0, 2.0}, {1.0, 6.0}};
    Point2D dir_vert = line_vert.Direction();
    ASSERT_DOUBLE_EQ(dir_vert.x, 0.0);
    ASSERT_DOUBLE_EQ(dir_vert.y, 1.0);

    Line line_point{{3.0, 4.0}, {3.0, 4.0}};
    Point2D dir_point = line_point.Direction();
    ASSERT_DOUBLE_EQ(dir_point.x, 0.0);
    ASSERT_DOUBLE_EQ(dir_point.y, 0.0);
}

TEST(GeometryTest, Line_BoundBox) {
    Line line_normal{{1.0, 2.0}, {4.0, 6.0}};
    BoundingBox bbox_normal = line_normal.BoundBox();
    ASSERT_DOUBLE_EQ(bbox_normal.min_x, 1.0);
    ASSERT_DOUBLE_EQ(bbox_normal.min_y, 2.0);
    ASSERT_DOUBLE_EQ(bbox_normal.max_x, 4.0);
    ASSERT_DOUBLE_EQ(bbox_normal.max_y, 6.0);

    Line line_back_order{{4.0, 6.0}, {1.0, 2.0}};
    BoundingBox bbox_back_order = line_back_order.BoundBox();
    ASSERT_DOUBLE_EQ(bbox_back_order.min_x, 1.0);
    ASSERT_DOUBLE_EQ(bbox_back_order.min_y, 2.0);
    ASSERT_DOUBLE_EQ(bbox_back_order.max_x, 4.0);
    ASSERT_DOUBLE_EQ(bbox_back_order.max_y, 6.0);

    Line line_neg{{-1.0, -2.0}, {-4.0, -6.0}};
    BoundingBox bbox_neg = line_neg.BoundBox();
    ASSERT_DOUBLE_EQ(bbox_neg.min_x, -4.0);
    ASSERT_DOUBLE_EQ(bbox_neg.min_y, -6.0);
    ASSERT_DOUBLE_EQ(bbox_neg.max_x, -1.0);
    ASSERT_DOUBLE_EQ(bbox_neg.max_y, -2.0);
}

TEST(GeometryTest, Line_Height) {
    Line line_normal{{1.0, 2.0}, {4.0, 6.0}};
    ASSERT_DOUBLE_EQ(line_normal.Height(), 4.0);

    Line line_horz{{1.0, 2.0}, {5.0, 2.0}};
    ASSERT_DOUBLE_EQ(line_horz.Height(), 0.0);

    Line line_vert{{1.0, 2.0}, {1.0, 6.0}};
    ASSERT_DOUBLE_EQ(line_vert.Height(), 4.0);

    Line line_neg{{-1.0, -2.0}, {-4.0, -6.0}};
    ASSERT_DOUBLE_EQ(line_neg.Height(), 4.0);
}

TEST(GeometryTest, Line_Center) {
    Line line_normal{{1.0, 2.0}, {4.0, 6.0}};
    Point2D center_normal = line_normal.Center();
    ASSERT_DOUBLE_EQ(center_normal.x, 2.5);
    ASSERT_DOUBLE_EQ(center_normal.y, 4.0);

    Line line_horz{{1.0, 2.0}, {5.0, 2.0}};
    Point2D center_horz = line_horz.Center();
    ASSERT_DOUBLE_EQ(center_horz.x, 3.0);
    ASSERT_DOUBLE_EQ(center_horz.y, 2.0);

    Line line_vert{{1.0, 2.0}, {1.0, 6.0}};
    Point2D center_vert = line_vert.Center();
    ASSERT_DOUBLE_EQ(center_vert.x, 1.0);
    ASSERT_DOUBLE_EQ(center_vert.y, 4.0);

    Line line_neg{{-1.0, -2.0}, {-4.0, -6.0}};
    Point2D center_neg = line_neg.Center();
    ASSERT_DOUBLE_EQ(center_neg.x, -2.5);
    ASSERT_DOUBLE_EQ(center_neg.y, -4.0);
}

TEST(GeometryTest, Line_Vertices) {
    Line line{{1.0, 2.0}, {4.0, 6.0}};
    auto vertices = line.Vertices();

    ASSERT_EQ(vertices.size(), 2);
    ASSERT_DOUBLE_EQ(vertices[0].x, 1.0);
    ASSERT_DOUBLE_EQ(vertices[0].y, 2.0);
    ASSERT_DOUBLE_EQ(vertices[1].x, 4.0);
    ASSERT_DOUBLE_EQ(vertices[1].y, 6.0);
}

TEST(GeometryTest, Line_Lines) {
    Line line{{1.0, 2.0}, {4.0, 6.0}};
    auto lines = line.Lines();

    ASSERT_EQ(lines.x.size(), 2);
    ASSERT_EQ(lines.y.size(), 2);

    ASSERT_DOUBLE_EQ(lines.x[0], 1.0);
    ASSERT_DOUBLE_EQ(lines.y[0], 2.0);
    ASSERT_DOUBLE_EQ(lines.x[1], 4.0);
    ASSERT_DOUBLE_EQ(lines.y[1], 6.0);
}

TEST(GeometryTest, Line_with_number_limits) {
    Line line_huge{{0.0, 0.0}, {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()}};
    ASSERT_TRUE(std::isinf(line_huge.Length()) || line_huge.Length() == std::numeric_limits<double>::max());

    Line line_inf{{0.0, 0.0}, {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()}};
    ASSERT_TRUE(std::isinf(line_inf.Length()));
}

TEST(GeometryTest, Triangle_ctor) {
    Point2D a(0.0, 0.0);
    Point2D b(3.0, 0.0);
    Point2D c(0.0, 4.0);
    Triangle triangle{a, b, c};

    ASSERT_DOUBLE_EQ(triangle.a.x, 0.0);
    ASSERT_DOUBLE_EQ(triangle.a.y, 0.0);
    ASSERT_DOUBLE_EQ(triangle.b.x, 3.0);
    ASSERT_DOUBLE_EQ(triangle.b.y, 0.0);
    ASSERT_DOUBLE_EQ(triangle.c.x, 0.0);
    ASSERT_DOUBLE_EQ(triangle.c.y, 4.0);
}

TEST(GeometryTest, Triangle_Area) {
    Triangle right_triangle{{0.0, 0.0}, {3.0, 0.0}, {0.0, 4.0}};
    ASSERT_DOUBLE_EQ(right_triangle.Area(), 6.0);

    Triangle common_triangle{{1.0, 1.0}, {4.0, 1.0}, {2.0, 5.0}};
    ASSERT_DOUBLE_EQ(common_triangle.Area(), 6.0);

    Triangle line_triangle{{0.0, 0.0}, {2.0, 2.0}, {4.0, 4.0}};
    ASSERT_DOUBLE_EQ(line_triangle.Area(), 0.0);

    Triangle triangle_neg{{-1.0, -1.0}, {-4.0, -1.0}, {-2.0, -5.0}};
    ASSERT_DOUBLE_EQ(triangle_neg.Area(), 6.0);

    Triangle triangle_back_order{{0.0, 4.0}, {3.0, 0.0}, {0.0, 0.0}};
    ASSERT_DOUBLE_EQ(triangle_back_order.Area(), 6.0);
}

TEST(GeometryTest, Triangle_Height) {
    Triangle right_triangle{{0.0, 0.0}, {3.0, 0.0}, {0.0, 4.0}};
    ASSERT_DOUBLE_EQ(right_triangle.Height(), 4.0);

    Triangle triangle_horz{{0.0, 2.0}, {3.0, 2.0}, {1.5, 2.0}};
    ASSERT_DOUBLE_EQ(triangle_horz.Height(), 0.0);

    Triangle triangle_vert{{2.0, 0.0}, {2.0, 3.0}, {2.0, 5.0}};
    ASSERT_DOUBLE_EQ(triangle_vert.Height(), 5.0);

    Triangle triangle_neg{{-1.0, -5.0}, {-3.0, -1.0}, {-2.0, -3.0}};
    ASSERT_DOUBLE_EQ(triangle_neg.Height(), 4.0);  // -1 - (-5) = 4
}

TEST(GeometryTest, Triangle_Center) {
    Triangle right_triangle{{0.0, 0.0}, {3.0, 0.0}, {0.0, 4.0}};
    Point2D right_triangle_center = right_triangle.Center();
    ASSERT_DOUBLE_EQ(right_triangle_center.x, 1.0);
    ASSERT_DOUBLE_EQ(right_triangle_center.y, 4.0 / 3.0);

    Triangle equilateral_triangle{{0.0, 0.0}, {4.0, 0.0}, {2.0, 3.464}};
    Point2D equilateral_triangle_center = equilateral_triangle.Center();
    ASSERT_NEAR(equilateral_triangle_center.x, 2.0, 1e-3);
    ASSERT_NEAR(equilateral_triangle_center.y, 1.155, 1e-3);

    Triangle triangle_neg{{-1.0, -2.0}, {-3.0, -4.0}, {-5.0, -6.0}};
    Point2D triangle_neg_center = triangle_neg.Center();
    ASSERT_DOUBLE_EQ(triangle_neg_center.x, -3.0);
    ASSERT_DOUBLE_EQ(triangle_neg_center.y, -4.0);
}

TEST(GeometryTest, Triangle_BoundBox) {
    Triangle right_triangle{{0.0, 0.0}, {3.0, 0.0}, {0.0, 4.0}};
    BoundingBox right_triangle_bbox = right_triangle.BoundBox();
    ASSERT_DOUBLE_EQ(right_triangle_bbox.min_x, 0.0);
    ASSERT_DOUBLE_EQ(right_triangle_bbox.min_y, 0.0);
    ASSERT_DOUBLE_EQ(right_triangle_bbox.max_x, 3.0);
    ASSERT_DOUBLE_EQ(right_triangle_bbox.max_y, 4.0);

    Triangle triangle_neg{{-1.0, -2.0}, {-3.0, 1.0}, {2.0, -5.0}};
    BoundingBox triangle_neg_bbox = triangle_neg.BoundBox();
    ASSERT_DOUBLE_EQ(triangle_neg_bbox.min_x, -3.0);
    ASSERT_DOUBLE_EQ(triangle_neg_bbox.min_y, -5.0);
    ASSERT_DOUBLE_EQ(triangle_neg_bbox.max_x, 2.0);
    ASSERT_DOUBLE_EQ(triangle_neg_bbox.max_y, 1.0);

    Triangle point_triangle{{1.0, 2.0}, {1.0, 2.0}, {1.0, 2.0}};
    BoundingBox point_triangle_bbox = point_triangle.BoundBox();
    ASSERT_DOUBLE_EQ(point_triangle_bbox.min_x, 1.0);
    ASSERT_DOUBLE_EQ(point_triangle_bbox.min_y, 2.0);
    ASSERT_DOUBLE_EQ(point_triangle_bbox.max_x, 1.0);
    ASSERT_DOUBLE_EQ(point_triangle_bbox.max_y, 2.0);
}

TEST(GeometryTest, Triangle_Vertices) {
    Point2D a(0.0, 0.0);
    Point2D b(3.0, 0.0);
    Point2D c(0.0, 4.0);
    Triangle triangle{a, b, c};

    auto vertices = triangle.Vertices();

    ASSERT_EQ(vertices.size(), 3);
    ASSERT_DOUBLE_EQ(vertices[0].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[0].y, 0.0);
    ASSERT_DOUBLE_EQ(vertices[1].x, 3.0);
    ASSERT_DOUBLE_EQ(vertices[1].y, 0.0);
    ASSERT_DOUBLE_EQ(vertices[2].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[2].y, 4.0);
}

TEST(GeometryTest, Triangle_Lines) {
    Point2D a(0.0, 0.0);
    Point2D b(3.0, 0.0);
    Point2D c(0.0, 4.0);
    Triangle triangle{a, b, c};

    auto lines = triangle.Lines();

    ASSERT_EQ(lines.x.size(), 4);
    ASSERT_EQ(lines.y.size(), 4);

    ASSERT_DOUBLE_EQ(lines.x[0], 0.0);
    ASSERT_DOUBLE_EQ(lines.y[0], 0.0);
    ASSERT_DOUBLE_EQ(lines.x[1], 3.0);
    ASSERT_DOUBLE_EQ(lines.y[1], 0.0);
    ASSERT_DOUBLE_EQ(lines.x[2], 0.0);
    ASSERT_DOUBLE_EQ(lines.y[2], 4.0);

    ASSERT_DOUBLE_EQ(lines.x[3], 0.0);
    ASSERT_DOUBLE_EQ(lines.y[3], 0.0);
}

TEST(GeometryTest, Triangle_with_number_limits) {
    Triangle triangle_huge{
        {0.0, 0.0}, {std::numeric_limits<double>::max(), 0.0}, {0.0, std::numeric_limits<double>::max()}};
    ASSERT_TRUE(std::isinf(triangle_huge.Area()));

    Triangle triangle_inf{
        {0.0, 0.0}, {std::numeric_limits<double>::infinity(), 0.0}, {0.0, std::numeric_limits<double>::infinity()}};
    ASSERT_TRUE(std::isnan(triangle_inf.Area()));
}

TEST(GeometryTest, RegularPolygon_ctor) {
    RegularPolygon poly({0.0, 0.0}, 5.0, 6);

    ASSERT_DOUBLE_EQ(poly.Center().x, 0.0);
    ASSERT_DOUBLE_EQ(poly.Center().y, 0.0);
    ASSERT_DOUBLE_EQ(poly.radius, 5.0);
    ASSERT_EQ(poly.sides, 6);
}

TEST(GeometryTest, RegularPolygon_Vertices) {
    RegularPolygon square({0.0, 0.0}, 1.0, 4);

    auto vertices = square.Vertices();
    ASSERT_EQ(vertices.size(), 4);

    ASSERT_NEAR(vertices[0].x, 1.0, 1e-10);
    ASSERT_NEAR(vertices[0].y, 0.0, 1e-10);

    ASSERT_NEAR(vertices[1].x, 0.0, 1e-10);
    ASSERT_NEAR(vertices[1].y, 1.0, 1e-10);

    ASSERT_NEAR(vertices[2].x, -1.0, 1e-10);
    ASSERT_NEAR(vertices[2].y, 0.0, 1e-10);

    ASSERT_NEAR(vertices[3].x, 0.0, 1e-10);
    ASSERT_NEAR(vertices[3].y, -1.0, 1e-10);

    RegularPolygon triangle({0.0, 0.0}, 1.0, 3);
    auto triangle_vertices = triangle.Vertices();
    ASSERT_EQ(triangle_vertices.size(), 3);
    auto distance = triangle_vertices[0].DistanceTo(triangle.Center());
    ASSERT_NEAR(distance, 1.0, 1e-10);

    RegularPolygon pentagon({0.0, 0.0}, 1.0, 5);
    auto pentagon_vertices = pentagon.Vertices();
    ASSERT_EQ(pentagon_vertices.size(), 5);
    distance = pentagon_vertices[0].DistanceTo(pentagon.Center());
    ASSERT_NEAR(distance, 1.0, 1e-10);

    RegularPolygon octagon({0.0, 0.0}, 1.0, 8);
    auto octagon_vertices = octagon.Vertices();
    ASSERT_EQ(octagon_vertices.size(), 8);
    distance = octagon_vertices[0].DistanceTo(octagon.Center());
    ASSERT_NEAR(distance, 1.0, 1e-10);

    RegularPolygon circle({0.0, 0.0}, 1.0, 100);
    auto circle_vertices = circle.Vertices();
    ASSERT_EQ(circle_vertices.size(), 100);
    distance = circle_vertices[0].DistanceTo(circle.Center());
    ASSERT_NEAR(distance, 1.0, 1e-10);
}

TEST(GeometryTest, RegularPolygon_Lines) {
    RegularPolygon square({0.0, 0.0}, 1.0, 4);

    Lines2DDyn lines = square.Lines();
    ASSERT_EQ(lines.x.size(), 5);
    ASSERT_EQ(lines.y.size(), 5);

    ASSERT_NEAR(lines.x[0], 1.0, 1e-10);
    ASSERT_NEAR(lines.y[0], 0.0, 1e-10);

    ASSERT_NEAR(lines.x[1], 0.0, 1e-10);
    ASSERT_NEAR(lines.y[1], 1.0, 1e-10);

    ASSERT_NEAR(lines.x[2], -1.0, 1e-10);
    ASSERT_NEAR(lines.y[2], 0.0, 1e-10);

    ASSERT_NEAR(lines.x[3], 0.0, 1e-10);
    ASSERT_NEAR(lines.y[3], -1.0, 1e-10);

    ASSERT_NEAR(lines.x[4], 1.0, 1e-10);
    ASSERT_NEAR(lines.y[4], 0.0, 1e-10);
}

TEST(GeometryTest, RegularPolygon_BoundBox) {
    RegularPolygon poly({2.0, 3.0}, 5.0, 6);

    BoundingBox bbox = poly.BoundBox();
    ASSERT_DOUBLE_EQ(bbox.min_x, -3.0);
    ASSERT_DOUBLE_EQ(bbox.min_y, -2.0);
    ASSERT_DOUBLE_EQ(bbox.max_x, 7.0);
    ASSERT_DOUBLE_EQ(bbox.max_y, 8.0);
}

TEST(GeometryTest, RegularPolygon_Height) {
    RegularPolygon poly({0.0, 0.0}, 5.0, 6);
    ASSERT_DOUBLE_EQ(poly.Height(), 10.0);

    RegularPolygon square({0.0, 0.0}, 3.0, 4);
    ASSERT_DOUBLE_EQ(square.Height(), 6.0);
}

TEST(GeometryTest, RegularPolygon_zero_radius) {
    RegularPolygon poly({0.0, 0.0}, 0.0, 4);
    auto vertices = poly.Vertices();
    ASSERT_EQ(vertices.size(), 4);

    ASSERT_DOUBLE_EQ(vertices[0].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[0].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[1].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[1].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[2].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[2].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[3].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[3].y, 0.0);
}

TEST(GeometryTest, RegularPolygon_zero_sides) {
    RegularPolygon poly({0.0, 0.0}, 1.0, 0);

    ASSERT_EQ(poly.Vertices().size(), 0);

    ASSERT_EQ(poly.Lines().x.size(), 0);
    ASSERT_EQ(poly.Lines().y.size(), 0);
}

TEST(GeometryTest, Circle_ctor) {
    Circle circle({2.0, 3.0}, 5.0);

    ASSERT_DOUBLE_EQ(circle.Center().x, 2.0);
    ASSERT_DOUBLE_EQ(circle.Center().y, 3.0);
    ASSERT_DOUBLE_EQ(circle.radius, 5.0);
}

TEST(GeometryTest, Circle_BoundBox) {
    Circle circle({2.0, 3.0}, 5.0);

    BoundingBox bbox = circle.BoundBox();
    ASSERT_DOUBLE_EQ(bbox.min_x, -3.0);
    ASSERT_DOUBLE_EQ(bbox.min_y, -2.0);
    ASSERT_DOUBLE_EQ(bbox.max_x, 7.0);
    ASSERT_DOUBLE_EQ(bbox.max_y, 8.0);
}

TEST(GeometryTest, Circle_Height) {
    Circle circle({0.0, 0.0}, 5.0);
    ASSERT_DOUBLE_EQ(circle.Height(), 10.0);
}

TEST(GeometryTest, Circle_Center) {
    Circle circle({2.0, 3.0}, 5.0);
    Point2D center = circle.Center();

    ASSERT_DOUBLE_EQ(center.x, 2.0);
    ASSERT_DOUBLE_EQ(center.y, 3.0);
}

TEST(GeometryTest, Circle_Vertices) {
    Circle circle({0.0, 0.0}, 1.0);

    auto vertices_default = circle.Vertices();
    ASSERT_EQ(vertices_default.size(), 30);

    auto vertices_8 = circle.Vertices(8);
    ASSERT_EQ(vertices_8.size(), 8);

    for (const auto &vertex : vertices_8) {
        ASSERT_NEAR(vertex.DistanceTo(circle.Center()), 1.0, 1e-10);
    }

    auto vertices_zero = circle.Vertices(0);
    ASSERT_TRUE(vertices_zero.empty());
}

TEST(GeometryTest, Circle_Lines) {
    Circle circle({0.0, 0.0}, 1.0);

    Lines2DDyn lines_default = circle.Lines();
    ASSERT_EQ(lines_default.x.size(), 101);
    ASSERT_EQ(lines_default.y.size(), 101);

    Lines2DDyn lines_4 = circle.Lines(4);
    ASSERT_EQ(lines_4.x.size(), 5);
    ASSERT_EQ(lines_4.y.size(), 5);

    ASSERT_NEAR(lines_4.x[0], 1.0, 1e-10);
    ASSERT_NEAR(lines_4.y[0], 0.0, 1e-10);

    ASSERT_NEAR(lines_4.x[1], 0.0, 1e-10);
    ASSERT_NEAR(lines_4.y[1], 1.0, 1e-10);

    ASSERT_NEAR(lines_4.x[2], -1.0, 1e-10);
    ASSERT_NEAR(lines_4.y[2], 0.0, 1e-10);

    ASSERT_NEAR(lines_4.x[3], 0.0, 1e-10);
    ASSERT_NEAR(lines_4.y[3], -1.0, 1e-10);

    ASSERT_NEAR(lines_4.x[4], 1.0, 1e-10);
    ASSERT_NEAR(lines_4.y[4], 0.0, 1e-10);

    Lines2DDyn lines_zero = circle.Lines(0);
    ASSERT_TRUE(lines_zero.x.empty());
    ASSERT_TRUE(lines_zero.y.empty());
}

TEST(GeometryTest, Circle_zero_radius) {
    Circle circle({0.0, 0.0}, 0.0);
    auto vertices = circle.Vertices(4);

    ASSERT_DOUBLE_EQ(vertices[0].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[0].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[1].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[1].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[2].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[2].y, 0.0);

    ASSERT_DOUBLE_EQ(vertices[3].x, 0.0);
    ASSERT_DOUBLE_EQ(vertices[3].y, 0.0);

    Lines2DDyn lines = circle.Lines(4);
    ASSERT_EQ(lines.x.size(), 5);
    ASSERT_EQ(lines.y.size(), 5);
}
