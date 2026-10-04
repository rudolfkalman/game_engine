#include "game_engine/renderer.hpp"

#include "game_engine/math.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

namespace game_engine::renderer {
uint32_t rgba(SDL_Color *color) {
  uint32_t color_32 = 0 | (static_cast<uint32_t>(color->r) << 24 |
                           static_cast<uint32_t>(color->g) << 16 |
                           static_cast<uint32_t>(color->b) << 8 |
                           static_cast<uint32_t>(color->a));
  return color_32;
}

Renderer::Renderer(SDL_Renderer *sdl_renderer, int width, int height) {
  this->sdl_renderer_ = sdl_renderer;
  this->width_ = width;
  this->height_ = height;
  this->frame_buffer_ = std::vector<uint32_t>(width * height);
  this->texture_ = SDL_CreateTexture(sdl_renderer_, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET, width, height);
}

void Renderer::Clear(uint32_t color) {
  std::fill(this->frame_buffer_.begin(), this->frame_buffer_.end(), color);
}

void Renderer::DrawMesh(const mesh::Mesh &mesh,
                        const transform::TransformedMesh &transformed) {
  std::vector<SDL_Color> face_colors{
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

    SDL_Color color = face_colors[(tri_index / 2) % face_colors.size()];
    uint32_t packed_color = rgba(&color);
    // SDL_SetRenderDrawColor(this->sdl_renderer_, color.r, color.g, color.b,
    // color.a);

    const Eigen::Vector2d &p1 = transformed.screen_vertices[triangle.p1];
    const Eigen::Vector2d &p2 = transformed.screen_vertices[triangle.p2];
    const Eigen::Vector2d &p3 = transformed.screen_vertices[triangle.p3];

    const int min_x = std::clamp(
        static_cast<int>(std::floor(std::min({p1.x(), p2.x(), p3.x()}))), 0,
        this->width_ - 1);
    const int min_y = std::clamp(
        static_cast<int>(std::floor(std::min({p1.y(), p2.y(), p3.y()}))), 0,
        this->height_ - 1);
    const int max_x = std::clamp(
        static_cast<int>(std::ceil(std::max({p1.x(), p2.x(), p3.x()}))), 0,
        this->width_ - 1);
    const int max_y = std::clamp(
        static_cast<int>(std::ceil(std::max({p1.y(), p2.y(), p3.y()}))), 0,
        this->height_ - 1);

    const Eigen::Vector2d p1_p2 = p2 - p1;
    const Eigen::Vector2d p2_p3 = p3 - p2;
    const Eigen::Vector2d p3_p1 = p1 - p3;

    const Eigen::Vector2d pixel(min_x + 0.5, min_y + 0.5);

    double row_cross1 = math::Cross2d(p1_p2, pixel - p1);
    double row_cross2 = math::Cross2d(p2_p3, pixel - p2);
    double row_cross3 = math::Cross2d(p3_p1, pixel - p3);

    for (int y = min_y; y <= max_y; ++y) {
      double cross1 = row_cross1;
      double cross2 = row_cross2;
      double cross3 = row_cross3;

      for (int x = min_x; x <= max_x; ++x) {
        const bool all_positive =
            cross1 >= 0.0 && cross2 >= 0.0 && cross3 >= 0.0;
        const bool all_negative =
            cross1 <= 0.0 && cross2 <= 0.0 && cross3 <= 0.0;

        if (all_positive || all_negative) {
          this->frame_buffer_[y * this->width_ + x] = packed_color;
        }

        cross1 -= p1_p2.y();
        cross2 -= p2_p3.y();
        cross3 -= p3_p1.y();
      }
      row_cross1 += p1_p2.x();
      row_cross2 += p2_p3.x();
      row_cross3 += p3_p1.x();
    }

    /*SDL_RenderLine(this->sdl_renderer_, static_cast<float>(p1.x()),
                   static_cast<float>(p1.y()), static_cast<float>(p2.x()),
                   static_cast<float>(p2.y()));
    SDL_RenderLine(this->sdl_renderer_, static_cast<float>(p1.x()),
                   static_cast<float>(p1.y()), static_cast<float>(p3.x()),
                   static_cast<float>(p3.y()));
    SDL_RenderLine(this->sdl_renderer_, static_cast<float>(p2.x()),
                   static_cast<float>(p2.y()), static_cast<float>(p3.x()),
                   static_cast<float>(p3.y()));*/
  }
}

void Renderer::Present() {
  SDL_UpdateTexture(this->texture_, nullptr, this->frame_buffer_.data(),
                    this->width_ * sizeof(uint32_t));
  SDL_RenderTexture(this->sdl_renderer_, this->texture_, nullptr, nullptr);
  SDL_RenderPresent(this->sdl_renderer_);
}

} // namespace game_engine::renderer
