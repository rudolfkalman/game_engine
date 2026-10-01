#include "game_engine/math.hpp"
#include "game_engine/mesh.hpp"
#include "game_engine/renderer.hpp"
#include "game_engine/transform.hpp"
#include "game_engine/window.hpp"

#include <Eigen/Dense>
#include <SDL3/SDL.h>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

// window size
#define WIDTH 1280
#define HEIGHT 720

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

    game_engine::mesh::Mesh cube = game_engine::mesh::MakeCube();

    double roll = 0.0;
    double pitch = 0.0;
    double yaw = 0.0;

    // cam
    Eigen::Vector4d Cam(0, 0, 500, 1);
    Eigen::Vector3d Cam_Pose(0, 0, 0); // rad

    // projection
    Eigen::Matrix4d Projection = game_engine::math::MakeProjection(
        game_engine::math::Deg2rad(104), 16.0 / 9.0, 200.0, 1000.0);

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

      const bool *keys = SDL_GetKeyboardState(nullptr);

      if (keys[SDL_SCANCODE_W]) {
        Cam += Eigen::Vector4d(0, 0, -100, 0);
      }

      if (keys[SDL_SCANCODE_S]) {
        Cam += Eigen::Vector4d(0, 0, 100, 0);
      }

      if (keys[SDL_SCANCODE_A]) {
        Cam += Eigen::Vector4d(-100, 0, 0, 0);
      }

      if (keys[SDL_SCANCODE_D]) {
        Cam += Eigen::Vector4d(100, 0, 0, 0);
      }

      // --------------------------------------------------------
      // 2-2. Clear frame
      // --------------------------------------------------------
      SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
      SDL_RenderClear(renderer);

      // --------------------------------------------------------
      // 2-3. Transform / draw
      // --------------------------------------------------------

      // view
      Eigen::Matrix4d View =
          game_engine::math::MakeRotation(-Cam_Pose[0], -Cam_Pose[1],
                                          -Cam_Pose[2])
              .transpose() *
          game_engine::math::MakeTranslation(-Cam[0], -Cam[1], -Cam[2]);

      const Eigen::Matrix4d Model1 =
          game_engine::math::MakeTranslation(0, 0, 0) *
          game_engine::math::MakeRotation(roll, pitch, yaw) *
          game_engine::math::MakeScale(100, 100, 100);

      const game_engine::transform::TransformedMesh transformed1 =
          game_engine::transform::Apply(cube, Model1, View, Projection, WIDTH,
                                        HEIGHT);

      game_engine::renderer::DrawMesh(renderer, cube, transformed1, WIDTH,
                                      HEIGHT);

      const Eigen::Matrix4d Model2 =
          game_engine::math::MakeTranslation(100, 100, 100) *
          game_engine::math::MakeRotation(roll, pitch, yaw) *
          game_engine::math::MakeScale(100, 100, 100);

      const game_engine::transform::TransformedMesh transformed2 =
          game_engine::transform::Apply(cube, Model2, View, Projection, WIDTH,
                                        HEIGHT);

      game_engine::renderer::DrawMesh(renderer, cube, transformed2, WIDTH,
                                      HEIGHT);

      // --------------------------------------------------------
      // 2-4. Present
      // --------------------------------------------------------
      SDL_RenderPresent(renderer);

      // Temporary frame limiter (~60 FPS).
      roll += 0.1;
      pitch += 0.1;
      yaw += 0.1;
      SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }

  return 0;
}
