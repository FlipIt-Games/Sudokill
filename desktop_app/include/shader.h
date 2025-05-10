#pragma once

#include <SDL3/SDL.h>

SDL_GPUShader *LoadShader(SDL_GPUDevice *device, SDL_GPUShaderStage stage, const char* path);
