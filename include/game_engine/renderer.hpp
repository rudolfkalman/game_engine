#ifndef GAME_ENGINE_RENDERER_HPP
#define GAME_ENGINE_RENDERER_HPP

#include "game_engine/mesh.hpp"
#include "game_engine/transform.hpp"

#include <SDL3/SDL.h>
#include <cstdint>

namespace game_engine::renderer {
//std::vector<uint32_t> frame_buff(1280 * 720, SDL_Color{255, 255, 255, 255});

void DrawMesh(SDL_Renderer *renderer, const mesh::Mesh &mesh,
              const transform::TransformedMesh &transformed, int screen_width,
              int screen_height);

} // namespace game_engine::renderer

#endif
