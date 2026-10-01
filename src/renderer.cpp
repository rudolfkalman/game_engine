#include "game_engine/renderer.hpp"

#include "game_engine/math.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace game_engine::renderer {

void DrawMesh(SDL_Renderer *renderer, const mesh::Mesh &mesh,
              const transform::TransformedMesh &transformed, int screen_width,
              int screen_height) {
  const std::array<SDL_Color, 6> face_colors{
      SDL_Color{255, 80, 80, 255},  SDL_Color{80, 255, 80, 255},
      SDL_Color{80, 80, 255, 255},  SDL_Color{255, 220, 80, 255},
      SDL_Color{255, 80, 220, 255}, SDL_Color{80, 220, 255, 255},
  };

  for (std::size_t tri_index = 0; tri_index < mesh.triangles.size();
       ++tri_index) {
    const mesh::Triangle &triangle = mesh.triangles[tri_index];

    const Eigen::Vector3d normal =
        transformed.view_normals[tri_index].head<3>();
    const Eigen::Vector3d to_camera =
        -transformed.view_vertices[triangle.p1].head<3>();

    if (normal.dot(to_camera) < 0.0) {
      continue;
    }

    const SDL_Color color = face_colors[(tri_index / 2) % face_colors.size()];
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const Eigen::Vector2d &p1 = transformed.screen_vertices[triangle.p1];
    const Eigen::Vector2d &p2 = transformed.screen_vertices[triangle.p2];
    const Eigen::Vector2d &p3 = transformed.screen_vertices[triangle.p3];

    const int min_x = std::clamp(
        static_cast<int>(std::floor(std::min({p1.x(), p2.x(), p3.x()}))), 0,
        screen_width - 1);
    const int min_y = std::clamp(
        static_cast<int>(std::floor(std::min({p1.y(), p2.y(), p3.y()}))), 0,
        screen_height - 1);
    const int max_x = std::clamp(
        static_cast<int>(std::ceil(std::max({p1.x(), p2.x(), p3.x()}))), 0,
        screen_width - 1);
    const int max_y = std::clamp(
        static_cast<int>(std::ceil(std::max({p1.y(), p2.y(), p3.y()}))), 0,
        screen_height - 1);

    const Eigen::Vector2d p1_p2 = p2 - p1;
    const Eigen::Vector2d p2_p3 = p3 - p2;
    const Eigen::Vector2d p3_p1 = p1 - p3;

    for (int y = min_y; y <= max_y; ++y) {
      for (int x = min_x; x <= max_x; ++x) {
        const Eigen::Vector2d pixel(static_cast<double>(x) + 0.5,
                                    static_cast<double>(y) + 0.5);

        const double cross1 = math::Cross2d(p1_p2, pixel - p1);
        const double cross2 = math::Cross2d(p2_p3, pixel - p2);
        const double cross3 = math::Cross2d(p3_p1, pixel - p3);

        const bool all_positive = cross1 >= 0.0 && cross2 >= 0.0 && cross3 >= 0.0;
        const bool all_negative = cross1 <= 0.0 && cross2 <= 0.0 && cross3 <= 0.0;

        if (all_positive || all_negative) {
          SDL_RenderPoint(renderer, static_cast<float>(x), static_cast<float>(y));
        }
      }
    }

    SDL_RenderLine(renderer, static_cast<float>(p1.x()), static_cast<float>(p1.y()),
                   static_cast<float>(p2.x()), static_cast<float>(p2.y()));
    SDL_RenderLine(renderer, static_cast<float>(p1.x()), static_cast<float>(p1.y()),
                   static_cast<float>(p3.x()), static_cast<float>(p3.y()));
    SDL_RenderLine(renderer, static_cast<float>(p2.x()), static_cast<float>(p2.y()),
                   static_cast<float>(p3.x()), static_cast<float>(p3.y()));
  }
}

} // namespace game_engine::renderer
