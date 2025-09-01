#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <span>
#include <string>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

std::string to_string(const Shape &shape) {
    return std::visit([](const auto &s) { return std::format("{}", s); }, shape);
}

void PrintAllIntersections(const Shape &shape, std::span<const Shape> others) {
    std::println("\n=== Intersections ===");

    /*
     * Используйте ranges чтобы оставить только фигуры,
     * поддерживающие возможность находить пересечения между собой
     *
     * Затем примените монадический интерфейс для обработки результатов:
     *     - Пересечение найдено в точке A между фигурами B и C
     *     - Фигуры B и C не пересекаются
     */

    auto is_supported_type = [](const Shape &s) {
        return std::holds_alternative<Line>(s) || std::holds_alternative<Circle>(s);
    };

    if (!is_supported_type(shape)) {
        std::println("Intersection is not supported for {}", to_string(shape));
        return;
    }
    std::println("Shape: {} ", to_string(shape));

    auto supported_shapes = others                                                                             //
                            | views::filter([&](const Shape &other_shape) { return &shape != &other_shape; })  //
                            | views::filter(is_supported_type);

    rng::for_each(supported_shapes, [&](const Shape &other_shape) {
        auto intersection_point = intersections::GetIntersectPoint(shape, other_shape);
        if (intersection_point.has_value()) {
            std::println("  - Intersection with {}: YES at point {}", to_string(other_shape),
                         intersection_point.value());
        } else {
            std::println("  - Intersection with {}: NO", to_string(other_shape));
        }
    });
}

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");
    std::println("Testing point: {} ", p);

    /*
     * Используйте ranges чтобы выбрать любые 5 фигур из списка.
     * Затем найдите расстояния от заданной точки до всех выбранных фигур.
     * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
     */

    rng::for_each(shapes | views::take(5), [&p](const auto &shape) {
        std::println("  - Distance to {}: {:.2f}", to_string(shape), queries::DistanceToPoint(shape, p));
    });
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Найти все пересечения между фигурами используя метод Bounding Box
     *     - Найти самую высокую фигуру (чья высота наибольшая)
     *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают данную функциональность
     */

    auto collisions = utils::FindAllCollisions(shapes);
    for (const auto &[shape1, shape2] : collisions) {
        std::println("  - Collision between {} and {}", to_string(shape1), to_string(shape2));
    }

    auto highest_shape = utils::FindHighestShape(shapes);
    if (highest_shape.has_value()) {
        size_t index = highest_shape.value();
        std::println("Highest shape is at index {}: {} with height {:.2f}", index, to_string(shapes[index]),
                     queries::GetHeight(shapes[index]));
    }

    auto indices = views::iota(0u, shapes.size());
    auto first_valid_pair = views::cartesian_product(indices, indices)  //
                            | views::filter([](const auto &p) {
                                  auto [i, j] = p;
                                  return i < j;
                              })  //
                            | views::filter([&](const auto &p) {
                                  auto [i, j] = p;
                                  return queries::DistanceBetweenShapes(shapes[i], shapes[j]).has_value();
                              })  //
                            | views::take(1);

    if (!first_valid_pair.empty()) {
        auto [i, j] = first_valid_pair.front();
        auto dist = queries::DistanceBetweenShapes(shapes[i], shapes[j]);
        std::println("Distance between {} and {}: {:.2f}", to_string(shapes[i]), to_string(shapes[j]), *dist);
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */

    const double height_threshold = 50.0;
    auto high_shapes = shapes |
                       views::filter([=](const Shape &s) { return queries::GetHeight(s) > height_threshold; }) |
                       views::take(3);
    if (!high_shapes.empty()) {
        std::println("Three shapes with height > {:.2f}:", height_threshold);
        for (const auto &shape : high_shapes) {
            std::println("  - {}", to_string(shape));
        }
    }

    auto [min_it, max_it] = rng::minmax_element(shapes, {}, [](const Shape &s) { return queries::GetHeight(s); });

    std::println("Shape with minimum height: {}", to_string(*min_it));
    std::println("Shape with maximum height: {}", to_string(*max_it));
}

int main() {
    utils::ShapeGenerator generator(-50.0, 50.0, 5.0, 28.0);
    std::vector<Shape> shapes = generator.GenerateShapes(15);

    std::println("Generated {} random shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту

    for (const auto &[index, shape] : views::enumerate(shapes)) {
        std::println("  [{}] Height: {:.2f}", index, queries::GetHeight(shape));
    }

    //
    // Вызываем разработанные функции
    //
    PrintAllIntersections(shapes[0], shapes);

    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    PerformShapeAnalysis(shapes);

    PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points;
    rng::for_each(shapes, [&](const auto &shape) {
        std::visit(
            [&](const auto &s) {
                const auto &vertices = s.Vertices();
                points.insert(points.end(), vertices.begin(), vertices.end());
            },
            shape);
    });

    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    auto hull_result = convex_hull::GrahamScan(points);
    if (hull_result.has_value()) {
        shapes.emplace_back(Polygon(std::move(hull_result.value())));
        visualization::Draw(shapes);
    } else {
        std::println("Could not build convex hull: {}", static_cast<int>(hull_result.error()));
    }
    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
        auto triangulation_result = triangulation::DelaunayTriangulation(points);
        if (triangulation_result.has_value()) {
            visualization::Draw(triangulation_result.value());
        } else {
            std::println("Could not perform triangulation: {}", static_cast<int>(triangulation_result.error()));
        }
    }
    return 0;
}