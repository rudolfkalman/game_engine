#pragma once

#include <SDL3/SDL.h>

#include <string_view>

namespace game_engine {

class Window {
public:
    // Creates an SDL-backed resizable window.
    Window(std::string_view title, int width, int height);
    ~Window();

    // Window owns the SDL_Window, so copying/moving is disabled for now.
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Access to SDL's native window handle for low-level SDL operations.
    [[nodiscard]] SDL_Window* native_handle() const noexcept;

private:
    SDL_Window* window_{nullptr};
};

} // namespace game_engine
