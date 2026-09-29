#include "game_engine/window.hpp"

#include <Eigen/Dense>
#include <SDL3/SDL.h>

#include <array>
#include <exception>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>
#define _USE_MATH_DEFINES
#include <cmath>

// window size
#define WIDTH 1280
#define HEIGHT 720

// triangle
struct Triangle {
  int p1;
  int p2;
  int p3;
};

// convinience functions
Eigen::Matrix4d MakeScale(double x, double y, double z) {
  Eigen::Matrix4d S;
  S << x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1;
  return S;
}

Eigen::Matrix4d MakeRotation(double roll, double pitch, double yaw) {
  Eigen::Matrix4d Yaw;
  Yaw << std::cos(yaw), -std::sin(yaw), 0, 0, std::sin(yaw), std::cos(yaw), 0,
      0, 0, 0, 1, 0, 0, 0, 0, 1;

  Eigen::Matrix4d Pitch;
  Pitch << std::cos(pitch), 0, std::sin(pitch), 0, 0, 1, 0, 0, -std::sin(pitch),
      0, std::cos(pitch), 0, 0, 0, 0, 1;

  Eigen::Matrix4d Roll;
  Roll << 1, 0, 0, 0, 0, std::cos(roll), -std::sin(roll), 0, 0, std::sin(roll),
      std::cos(roll), 0, 0, 0, 0, 1;
  return Roll * Pitch * Yaw;
}

Eigen::Matrix4d MakeTranslation(double x, double y, double z) {
  Eigen::Matrix4d T;
  T << 1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1;
  return T;
}

Eigen::Matrix4d MakeProjection(double fovY, double aspect, double near,
                               double far) {
  double w, h;
  h = 2 * near * std::tan(fovY / 2);
  w = h * aspect;
  Eigen::Matrix4d P;
  P << 2 * near / w, 0, 0, 0, 0, 2 * near / h, 0, 0, 0, 0,
      -(far + near) / (far - near), -(2 * far * near) / (far - near), 0, 0, -1,
      0;
  return P;
}

double deg2rad(double deg) { return deg * M_PI / 180.0; }

int main() {
  try {
    // ------------------------------------------------------------
    // 1. Window / SDL initialization
    // ------------------------------------------------------------
    game_engine::Window window{"game_engine", WIDTH, HEIGHT};

    // SDL's 2D rendering context.
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window.native_handle(), nullptr);
    if (renderer == nullptr) {
      throw std::runtime_error(std::string{"SDL_CreateRenderer failed: "} +
                               SDL_GetError());
    }

    // cube
    std::array<Eigen::Vector4d, 8> vertices{
        Eigen::Vector4d{-1, -1, -1, 1}, Eigen::Vector4d{1, -1, -1, 1},
        Eigen::Vector4d{1, 1, -1, 1},   Eigen::Vector4d{-1, 1, -1, 1},
        Eigen::Vector4d{-1, -1, 1, 1},  Eigen::Vector4d{1, -1, 1, 1},
        Eigen::Vector4d{1, 1, 1, 1},    Eigen::Vector4d{-1, 1, 1, 1},
    };

    std::array<Triangle, 12> triangles{
        // z = -1
        Triangle{0, 3, 2},
        Triangle{0, 2, 1},

        // z = +1
        Triangle{4, 5, 6},
        Triangle{4, 6, 7},

        // x = -1
        Triangle{0, 4, 7},
        Triangle{0, 7, 3},

        // x = +1
        Triangle{1, 2, 6},
        Triangle{1, 6, 5},

        // y = -1
        Triangle{0, 1, 5},
        Triangle{0, 5, 4},

        // y = +1
        Triangle{3, 7, 6},
        Triangle{3, 6, 2},
    };

    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;

    // cam
    Eigen::Vector4d Cam(0, 0, 500, 1);
    Eigen::Vector3d Cam_Pose(0, 0, 0); // rad

    // view
    Eigen::Matrix4d View =
        MakeRotation(-Cam_Pose[0], -Cam_Pose[1], -Cam_Pose[2]).transpose() *
        MakeTranslation(-Cam[0], -Cam[1], -Cam[2]);

    // projection
    Eigen::Matrix4d Projection =
        MakeProjection(deg2rad(104), 16.0 / 9.0, 200.0, 200.0);

    // ------------------------------------------------------------
    // 2. Main loop
    // ------------------------------------------------------------
    bool running = true;
    while (running) {
      // --------------------------------------------------------
      // 2-1. Event processing
      // --------------------------------------------------------
      SDL_Event event{};
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
          running = false;
        }
      }

      // --------------------------------------------------------
      // 2-2. Clear frame
      // --------------------------------------------------------
      SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
      SDL_RenderClear(renderer);

      // --------------------------------------------------------
      // 2-3. Draw points
      // --------------------------------------------------------
      // Our points are centered around (0, 0), but SDL's render
      // coordinates start at the top-left corner of the window.
      // Translate the local origin to the center of the screen.
      constexpr double screen_center_x = WIDTH / 2.0;
      constexpr double screen_center_y = HEIGHT / 2.0;

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

      std::vector<Eigen::Vector2d> transformed_vertices;

      // model mat: transition + rotation + scale
      Eigen::Matrix4d Model = MakeTranslation(0, 0, 0) *
                              MakeRotation(roll, pitch, yaw) *
                              MakeScale(100, 100, 100);

      for (Eigen::Vector4d point : vertices) {
        // local -> clip
        point = Projection * View * Model * point;

        // clip -> NDC
        point[0] /= point[3];
        point[1] /= point[3];
        point[2] /= point[3];

        // NDC -> screen
        const float screen_x =
            static_cast<float>((point[0] + 1.0) * 0.5 * WIDTH);

        const float screen_y =
            static_cast<float>((1.0 - point[1]) * 0.5 * HEIGHT);

        for (int dy = -2; dy <= 2; ++dy) {
          for (int dx = -2; dx <= 2; ++dx) {
            SDL_RenderPoint(renderer, screen_x + dx, screen_y + dy);
          }
        }

        transformed_vertices.push_back(Eigen::Vector2d(screen_x, screen_y));
      }

      // wire frame
      for (auto t : triangles) {
        int min_x = std::min({transformed_vertices[t.p1][0], transformed_vertices[t.p2][0], transformed_vertices[t.p3][0]});
        int min_y = std::min({transformed_vertices[t.p1][1], transformed_vertices[t.p2][1], transformed_vertices[t.p3][1]});
        int max_x = std::max({transformed_vertices[t.p1][0], transformed_vertices[t.p2][0], transformed_vertices[t.p3][0]});
        int max_y = std::max({transformed_vertices[t.p1][1], transformed_vertices[t.p2][1], transformed_vertices[t.p3][1]});

        // p1 -> p2, p1 -> p3, p2 -> p3

        for(int i = min_y; i <= max_y; i++){
          for(int j = min_x; j <= max_x; j++){
            if(){
              
            }
          }
        }

        SDL_RenderLine(renderer, transformed_vertices[t.p1][0],
                       transformed_vertices[t.p1][1],
                       transformed_vertices[t.p2][0],
                       transformed_vertices[t.p2][1]);
        SDL_RenderLine(renderer, transformed_vertices[t.p1][0],
                       transformed_vertices[t.p1][1],
                       transformed_vertices[t.p3][0],
                       transformed_vertices[t.p3][1]);
        SDL_RenderLine(renderer, transformed_vertices[t.p2][0],
                       transformed_vertices[t.p2][1],
                       transformed_vertices[t.p3][0],
                       transformed_vertices[t.p3][1]);
      }

      // --------------------------------------------------------
      // 2-4. Present
      // --------------------------------------------------------
      SDL_RenderPresent(renderer);

      // Temporary frame limiter (~60 FPS).
      roll += 0.01;
      pitch += 0.01;
      yaw += 0.01;
      SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
