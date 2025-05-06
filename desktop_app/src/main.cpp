#include <SDL3/SDL.h>

#define SDL_ERROR(message) do { \
    SDL_LogError(1, "In %s at line %d: %s. SDL error: %s", __FILE__, __LINE__, message, SDL_GetError()); \
    exit; \
} while(0)

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_ERROR("Couldn't initialize SDL");
    }

    auto window_flags = SDL_WINDOW_VULKAN;
    auto window = SDL_CreateWindow("Sudokill", 1280, 720, window_flags);
    if (!window) {
        SDL_ERROR("Couldn't initialize window");
    }

    auto device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, true, nullptr);
    if (!device) {
        SDL_ERROR("Couldn't initialize gpu device");
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_ERROR("Couldn't claim window for gpu device");
    }

    while(true) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                goto exit;
            }
        }

        auto cmd_buffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmd_buffer) {
            SDL_ERROR("Couldn't acquire gpu command buffer");
        }

        SDL_GPUTexture *swapchain_tex;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, window, &swapchain_tex, nullptr, nullptr)) {
            SDL_ERROR("Couldn't acquire swapchain texture");
        }

        if (swapchain_tex) {
            SDL_GPUColorTargetInfo color_target_info = { 0 };
            color_target_info.texture = swapchain_tex;
            color_target_info.clear_color = { 1.0f, 0.0f, 0.0f, 1.0f };
            color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
            color_target_info.store_op = SDL_GPU_STOREOP_STORE;

            auto render_pass = SDL_BeginGPURenderPass(cmd_buffer, &color_target_info, 1, nullptr);
            SDL_EndGPURenderPass(render_pass);
        }

        SDL_SubmitGPUCommandBuffer(cmd_buffer);
    }

exit:
    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyWindow(window);
    SDL_DestroyGPUDevice(device);

    return 0;
}
