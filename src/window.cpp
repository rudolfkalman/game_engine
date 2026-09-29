#include "game_engine/window.hpp"

#include <stdexcept>
#include <string>

namespace game_engine {

Window::Window(std::string_view title, int width, int height) {
    // Initialize the SDL video subsystem.
    // This must succeed before creating any SDL windows.
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string{"SDL_Init failed: "} + SDL_GetError());
    }

    // SDL expects a null-terminated C string, so convert string_view first.
    const std::string window_title{title};

    // Create the native OS window through SDL.
    // SDL_WINDOW_RESIZABLE allows the user to resize it.
    window_ = SDL_CreateWindow(window_title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        const std::string error = SDL_GetError();

        // SDL was already initialized above, so clean it up before
        // propagating the constructor failure.
        SDL_Quit();
        throw std::runtime_error("SDL_CreateWindow failed: " + error);
    }
}

Window::~Window() {
    // Destroy the native window first, then shut SDL down.
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

SDL_Window* Window::native_handle() const noexcept {
    // Expose the underlying SDL handle only when direct SDL access is needed.
    return window_;
}

} // namespace game_engine
