#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <expected>
#include <format>
#include <numbers>
#include <optional>
#include <print>
#include <ranges>
#include <variant>
#include <vector>

namespace geometry {

/*
 * Добавьте к методам класса Point2D и Lines2DDyn все необходимые аттрибуты и спецификаторы
 * Важно: Возвращаемый тип и принимаемые аргументы менять не нужно
 */
struct Point2D {
    double x, y;

    constexpr Point2D() noexcept : x(0), y(0) {}
    constexpr Point2D(double x, double y) noexcept : x(x), y(y) {}

    // Comparison
    [[nodiscard]] constexpr bool operator<(const Point2D &other) const noexcept {
        return x < other.x || (x == other.x && y < other.y);
    }
    [[nodiscard]] constexpr bool operator==(const Point2D &other) const noexcept {
        return x == other.x && y == other.y;
    }

    // Binary math operators
    [[nodiscard]] constexpr Point2D operator+(const Point2D &other) const noexcept {
        return {x + other.x, y + other.y};
    }
    [[nodiscard]] constexpr Point2D operator-(const Point2D &other) const noexcept {
        return {x - other.x, y - other.y};
    }
    [[nodiscard]] constexpr Point2D operator*(double value) const noexcept { return {x * value, y * value}; }
    [[nodiscard]] constexpr Point2D operator/(double value) const noexcept { return {x / value, y / value}; }

    // Binary geometry operations
    [[nodiscard]] constexpr double Dot(const Point2D &other) const noexcept { return x * other.x + y * other.y; }
    [[nodiscard]] constexpr double Cross(const Point2D &other) const noexcept { return x * other.y - y * other.x; }
    [[nodiscard]] double Length() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] double DistanceTo(const Point2D &other) const noexcept { return (*this - other).Length(); }

    [[nodiscard]] Point2D Normalize() const noexcept {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template <size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    void Reserve(size_t n) noexcept {
        x.reserve(n);
        y.reserve(n);
    }
    void PushBack(Point2D p) noexcept {
        x.push_back(p.x);
        y.push_back(p.y);
    }
    void PushBack(double px, double py) noexcept {
        x.push_back(px);
        y.push_back(py);
    }
    [[nodiscard]] Point2D Front() const noexcept { return {x.front(), y.front()}; }
};

struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    [[nodiscard]] constexpr bool Overlaps(const BoundingBox &other) const noexcept {
        return max_x >= other.min_x && min_x <= other.max_x && max_y >= other.min_y && min_y <= other.max_y;
    }

    [[nodiscard]] constexpr double Width() const noexcept { return max_x - min_x; }
    [[nodiscard]] constexpr double Height() const noexcept { return max_y - min_y; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return {(min_x + max_x) / 2, (min_y + max_y) / 2}; }
};

struct Line {
    Point2D start, end;

    [[nodiscard]] double Length() const noexcept { return start.DistanceTo(end); }
    [[nodiscard]] Point2D Direction() const noexcept { return (end - start).Normalize(); }
    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y)};
    }
    [[nodiscard]] double Height() const noexcept { return BoundBox().Height(); }
    [[nodiscard]] Point2D Center() const noexcept { return (start + end) / 2.0; }
    [[nodiscard]] std::array<Point2D, 2> Vertices() const noexcept { return {start, end}; }
    [[nodiscard]] Lines2D<2> Lines() const noexcept { return {{start.x, end.x}, {start.y, end.y}}; }
};

struct Triangle {
    Point2D a, b, c;

    //
    // Обратите внимание! В методе Lines(), в отличие от Vertices(), координаты точек замыкаются на начало:
    // a.x, b.x, c.x а затем идёт снова первая вершина a.x
    //
    // Это необходимо для правильного рисования фигур через gnuplot, который формирует линии используя пары точек.
    // В случае с  Triangle будут составлены такие пары точек:
    //      - { a, b }
    //      - { b, c }
    //      - { c, a }
    //

    [[nodiscard]] double Area() const noexcept {
        return 0.5 * std::abs(a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
    }
    [[nodiscard]] double Height() const noexcept { return BoundBox().Height(); }
    [[nodiscard]] Point2D Center() const noexcept { return {(a.x + b.x + c.x) / 3.0, (a.y + b.y + c.y) / 3.0}; }
    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}), std::max({a.x, b.x, c.x}),
                std::max({a.y, b.y, c.y})};
    }
    [[nodiscard]] std::array<Point2D, 3> Vertices() const noexcept { return {a, b, c}; }
    [[nodiscard]] Lines2D<4> Lines() const noexcept { return {{a.x, b.x, c.x, a.x}, {a.y, b.y, c.y, a.y}}; }
};

struct Rectangle {
    Point2D bottom_left;
    double width, height;

    [[nodiscard]] double Area() const noexcept { return width * height; }
    [[nodiscard]] double Height() const noexcept { return height; }
    [[nodiscard]] Point2D Center() const noexcept {
        return {bottom_left.x + width / 2.0, bottom_left.y + height / 2.0};
    }
    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {bottom_left.x, bottom_left.y, bottom_left.x + width, bottom_left.y + height};
    }
    [[nodiscard]] std::array<Point2D, 4> Vertices() const noexcept {
        return {bottom_left,
                {bottom_left.x + width, bottom_left.y},
                {bottom_left.x + width, bottom_left.y + height},
                {bottom_left.x, bottom_left.y + height}};
    }
    [[nodiscard]] Lines2D<5> Lines() const noexcept {
        return {{bottom_left.x, bottom_left.x + width, bottom_left.x + width, bottom_left.x, bottom_left.x},
                {bottom_left.y, bottom_left.y, bottom_left.y + height, bottom_left.y + height, bottom_left.y}};
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius, int sides) noexcept
        : center_p(center), radius(radius), sides(sides) {}

    [[nodiscard]] Point2D Center() const noexcept { return center_p; }
    [[nodiscard]] double Height() const noexcept { return BoundBox().Height(); }
    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] std::vector<Point2D> Vertices() const noexcept {
        return std::views::iota(0, sides)                                       //
               | std::views::transform([this](int i) { return GetVertex(i); })  //
               | std::ranges::to<std::vector>();
    }
    [[nodiscard]] Lines2DDyn Lines() const {
        if (sides <= 0) {
            return {};
        }
        Lines2DDyn lines;
        lines.Reserve(sides + 1);
        std::ranges::for_each(std::views::iota(0, sides + 1) |
                                  std::views::transform([this](int i) { return GetVertex(i); }),
                              [&lines](const Point2D &p) { lines.PushBack(p); });
        return lines;
    }

protected:
    [[nodiscard]] Point2D GetVertex(int number) const {
        const double angle = 2 * std::numbers::pi * (number % sides) / sides;
        return {center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle)};
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius) noexcept : center_p(center), radius(radius) {}

    [[nodiscard]] BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] double Height() const noexcept { return 2 * radius; }
    [[nodiscard]] Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] std::vector<Point2D> Vertices(size_t N = 30) const {
        return std::views::iota(size_t{}, N)                                          //
               | std::views::transform([this, N](int i) { return GetVertex(i, N); })  //
               | std::ranges::to<std::vector>();
    }
    [[nodiscard]] Lines2DDyn Lines(size_t N = 100) const {
        if (N == 0) {
            return {};
        }
        Lines2DDyn lines;
        lines.Reserve(N + 1);
        std::ranges::for_each(std::views::iota(size_t{}, N + 1) |
                                  std::views::transform([this, N](int i) { return GetVertex(i, N); }),
                              [&lines](const Point2D &p) { lines.PushBack(p); });
        return lines;
    }

protected:
    [[nodiscard]] Point2D GetVertex(int number, size_t sides) const {
        const double angle = 2 * std::numbers::pi * (number % sides) / sides;
        return {center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle)};
    }
};

class Polygon {
public:
    explicit Polygon(std::vector<Point2D> points) : points_(std::move(points)) {
        if (points_.empty()) {
            bounding_box_ = BoundingBox{0, 0, 0, 0};
            return;
        }
        double min_x = points_[0].x;
        double max_x = points_[0].x;
        double min_y = points_[0].y;
        double max_y = points_[0].y;
        for (const auto &p : points_ | std::views::drop(1)) {
            min_x = std::min(min_x, p.x);
            max_x = std::max(max_x, p.x);
            min_y = std::min(min_y, p.y);
            max_y = std::max(max_y, p.y);
        }
        bounding_box_ = BoundingBox{min_x, min_y, max_x, max_y};
    }

    [[nodiscard]] Point2D Center() const noexcept { return bounding_box_.Center(); }
    [[nodiscard]] double Height() const noexcept { return bounding_box_.Height(); }
    [[nodiscard]] BoundingBox BoundBox() const noexcept { return bounding_box_; }

    [[nodiscard]] const std::vector<Point2D> &Vertices(size_t N = 30) const noexcept {
        (void)N;
        return points_;
    }
    [[nodiscard]] Lines2DDyn Lines(size_t N = 100) const {
        (void)N;
        if (points_.empty()) {
            return {};
        }
        Lines2DDyn lines;
        lines.Reserve(points_.size() + 1);
        std::ranges::for_each(std::views::iota(size_t{}, points_.size() + 1) |
                                  std::views::transform([this](int i) { return points_[i % points_.size()]; }),
                              [&lines](const Point2D &p) { lines.PushBack(p); });
        return lines;
    }

private:
    std::vector<Point2D> points_;
    BoundingBox bounding_box_;
};

using Shape = std::variant<Line, Triangle, Rectangle, RegularPolygon, Circle, Polygon>;

enum class GeometryError { Unsupported, NoIntersection, InvalidInput, DegenrateCase, InsufficientPoints };

template <typename T>
using GeometryResult = std::expected<T, GeometryError>;

/*
 * В коде везде используется ReplaceMe. Ваша задача - удалить ReplaceMe и везде вместо него
 использовать наиболее подходящий тип для решения задачи
 */
struct ReplaceMe {
    ReplaceMe(std::vector<Shape>) {}
};

}  // namespace geometry

template <>
struct std::formatter<geometry::Point2D> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Point2D &p, FormatContext &ctx) const {
        return format_to(ctx.out(), "({:.2f}, {:.2f})", p.x, p.y);
    }
};
template <>
struct std::formatter<std::vector<geometry::Point2D>> {
    bool use_new_line = false;

    constexpr auto parse(std::format_parse_context &ctx) const {
        auto it = ctx.begin();

        /* ваш код здесь */

        return it;
    }

    template <typename FormatContext>
    auto format(const std::vector<geometry::Point2D> &v, FormatContext &ctx) const {

        /* ваш код здесь */
        return ctx.out();
    }
};

template <>
struct std::formatter<geometry::Line> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Line &l, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Line({}, {})", l.start, l.end);
    }
};

template <>
struct std::formatter<geometry::Circle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Circle &c, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Circle(center={}, r={:.2f})", c.center_p, c.radius);
    }
};

template <>
struct std::formatter<geometry::Rectangle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Rectangle &r, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Rectangle(bottom_left={}, w={:.2f}, h={:.2f})", r.bottom_left, r.width,
                              r.height);
    }
};

template <>
struct std::formatter<geometry::RegularPolygon> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::RegularPolygon &p, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "RegularPolygon(center={}, r={:.2f}, sides={})", p.center_p, p.radius,
                              p.sides);
    }
};
template <>
struct std::formatter<geometry::Triangle> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Triangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Triangle({}, {}, {})", t.a, t.b, t.c);
    }
};
template <>
struct std::formatter<geometry::Polygon> {
    constexpr auto parse(std::format_parse_context &ctx) const { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Polygon &poly, FormatContext &ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "Polygon[{} points]: [", poly.Vertices().size());

        for (const auto &p : poly.Vertices()) {
            out = std::format_to(out, "{} ", p);
        }

        return std::format_to(out, "]");
    }
};
