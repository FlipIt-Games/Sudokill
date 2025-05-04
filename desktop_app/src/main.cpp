#include <SDL3/SDL.h>

#define SDL_ERROR(message) do { \
    SDL_LogError(1, "In %s at line %d: %s. SDL error: %s", __FILE__, __LINE__, message, SDL_GetError()); \
    exit; \
} while(0)

void handle_sdl_error(const char* message);

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_ERROR("Couldn't initialize SDL");
    }

    auto window_flags = SDL_WINDOW_VULKAN | SDL_WINDOWPOS_CENTERED;
    auto window = SDL_CreateWindow("Sudokill", 1280, 720, window_flags);
    if (!window) {
        SDL_ERROR("Couldn't initialize window");
    }

    auto device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, true, nullptr);
    if (!device) {
        SDL_ERROR("Couldn't initialize gpu device");
    }

    while(true) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                goto exit;
            }
        }


    }

exit:
    return 0;
}
