#pragma once
#include "geometry.hpp"
#include "queries.hpp"
#include <print>
#include <random>
#include <ranges>
#include <utility>
#include <vector>

namespace geometry::utils {

class ShapeGenerator {
public:
    ShapeGenerator(double min_coord = -100.0, double max_coord = 100.0, double min_size = 1.0, double max_size = 20.0)
        : gen(20), coord_dist(min_coord, max_coord), size_dist(min_size, max_size), sides_dist(3, 12), type_dist(0, 4) {
    }

    Shape GenerateRandomShape() {
        Point2D center{coord_dist(gen), coord_dist(gen)};
        double size = size_dist(gen);

        switch (type_dist(gen)) {
        case 0: {
            Point2D end{center.x + size, center.y + size};
            return Line{center, end};
        }
        case 1: {
            Point2D a{center.x, center.y};
            Point2D b{center.x + size, center.y};
            Point2D c{center.x + size / 2, center.y + size};
            return Triangle{a, b, c};
        }
        case 2: {
            return Rectangle{center, size, size * 0.8};
        }
        case 3: {
            int sides = sides_dist(gen);
            return RegularPolygon{center, size, sides};
        }
        case 4: {
            return Circle{center, size};
        }
        }
        return Circle{center, size};
    }

    std::vector<Shape> GenerateShapes(size_t count) {
        std::vector<Shape> shapes;
        shapes.reserve(count);

        for (auto _ : std::views::iota(0u, count)) {
            shapes.push_back(GenerateRandomShape());
        }

        return shapes;
    }

private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> coord_dist;
    std::uniform_real_distribution<double> size_dist;
    std::uniform_int_distribution<int> sides_dist;
    std::uniform_int_distribution<int> type_dist;
};

inline std::vector<std::pair<Shape, Shape>> FindAllCollisions(std::span<const Shape> shapes) {
    auto indices = std::views::iota(size_t{}, shapes.size());
    auto combination = std::views::cartesian_product(indices, indices);
    auto distinct_indices = combination | std::views::filter([](const auto &p) {
                                auto [i, j] = p;
                                return i < j;
                            });
    auto shapes_pair = distinct_indices | std::views::transform([&shapes](const auto &p) {
                           auto [i, j] = p;
                           return std::make_pair(shapes[i], shapes[j]);
                       });
    auto overlapped = shapes_pair | std::views::filter([](const auto &shape_pair) {
                          const auto &[shape1, shape2] = shape_pair;
                          return queries::BoundingBoxesOverlap(shape1, shape2);
                      });

    return overlapped | std::ranges::to<std::vector>();
}

inline std::optional<size_t> FindHighestShape(std::span<const Shape> shapes) {
    if (shapes.empty()) {
        return std::nullopt;
    }

    auto indices = std::views::iota(0u, shapes.size());
    auto max_it =
        std::ranges::max_element(indices, {}, [&shapes](size_t index) { return queries::GetHeight(shapes[index]); });

    return *max_it;
}

}  // namespace geometry::utils