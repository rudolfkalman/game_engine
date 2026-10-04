#ifndef GAME_ENGINE_RENDERER_HPP
#define GAME_ENGINE_RENDERER_HPP

#include "game_engine/mesh.hpp"
#include "game_engine/transform.hpp"

#include <SDL3/SDL.h>
#include <cstdint>

namespace game_engine::renderer {
uint32_t rgba(SDL_Color *color);

class Renderer {
public:
    Renderer(SDL_Renderer* sdl_renderer, int width, int height);

    void Clear(uint32_t color);
    void DrawMesh(
        const mesh::Mesh& mesh,
        const transform::TransformedMesh& transformed
    );
    void Present();

private:
    SDL_Renderer* sdl_renderer_;
    SDL_Texture* texture_;

    int width_;
    int height_;

    std::vector<uint32_t> frame_buffer_;
    std::vector<double> z_buffer_;
};

} // namespace game_engine::renderer

#endif
