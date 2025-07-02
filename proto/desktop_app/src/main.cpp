#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#define SDL_ERROR(message) do { \
    SDL_LogError(1, "In %s at line %d: %s. SDL error: %s", __FILE__, __LINE__, message, SDL_GetError()); \
    exit(1); \
} while(0)

struct Shader {
    SDL_GPUShader *data;
    SDL_ShaderCross_GraphicsShaderMetadata metadata;
};

Shader LoadShader(SDL_GPUDevice *device, const char *name);

struct PositionColorVertex {
    float x, y, z;
    uint8_t r, g, b, a;
};

struct TextVertex {
    float x, y, z;
    SDL_FColor color;
    float u, v;
};

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_ERROR("Couldn't initialize SDL");
    }

    if (!SDL_ShaderCross_Init()) {
        SDL_ERROR("Couldn't initialize shadercross");
    }

    if (!TTF_Init()) {
        SDL_ERROR("COuldn't initialize ttf");
    }

    auto window = SDL_CreateWindow("Sudokill", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_ERROR("Couldn't initialize window");
    }

    auto device = SDL_CreateGPUDevice(SDL_ShaderCross_GetHLSLShaderFormats(), true, nullptr);
    if (!device) {
        SDL_ERROR("Couldn't initialize gpu device");
    }

    auto text_engine = TTF_CreateGPUTextEngine(device);
    if (!text_engine) {
        SDL_ERROR("Couldn't initialize text engine");
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window)) {
        SDL_ERROR("Couldn't claim window for gpu device");
    } 

    // COLORED RECT PIPELINE
    auto v_shader = LoadShader(device, "position_color.vert.hlsl");
    auto f_shader = LoadShader(device, "solid_color.frag.hlsl");

    SDL_GPUColorTargetDescription color_targets_descriptions[] = {
        { .format = SDL_GetGPUSwapchainTextureFormat(device, window)}   
    };

    SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {
		.vertex_shader = v_shader.data,
		.fragment_shader = f_shader.data,
		.vertex_input_state = (SDL_GPUVertexInputState){
			.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
				.slot = 0,
				.pitch = sizeof(PositionColorVertex),
				.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
				.instance_step_rate = 0
			}},
			.num_vertex_buffers = 1,
			.vertex_attributes = (SDL_GPUVertexAttribute[]){{
				.location = 0,
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.offset = 0
			}, {
				.location = 1,
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
				.offset = sizeof(float) * 3
			}},
			.num_vertex_attributes = 2,
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.target_info = {
			.color_target_descriptions = color_targets_descriptions,
			.num_color_targets = 1,
		},
	};

    auto pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);
    if (!pipeline) {
        SDL_ERROR("Couldn't create graphics pipeline");
    }

    SDL_ReleaseGPUShader(device, v_shader.data);
    SDL_ReleaseGPUShader(device, f_shader.data);

    SDL_GPUBufferCreateInfo vertex_buffer_create_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionColorVertex) * 6 * 81,
    };
    auto vertex_buffer = SDL_CreateGPUBuffer(device, &vertex_buffer_create_info);

    SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(PositionColorVertex) * 6 * 81
    };
    auto transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_create_info);
    auto transfer_data = (PositionColorVertex*)SDL_MapGPUTransferBuffer(device, transfer_buffer, false);

    // TEXT RENDERING
    auto v_text_shader = LoadShader(device, "text.vert.hlsl");
    auto f_text_shader = LoadShader(device, "text.frag.hlsl");

    SDL_GPUGraphicsPipelineCreateInfo text_pipeline_create_infos = {
        .vertex_shader = v_text_shader.data,
        .fragment_shader = f_text_shader.data,
        .vertex_input_state = (SDL_GPUVertexInputState){
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
                .slot = 0,
                .pitch = sizeof(TextVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = (SDL_GPUVertexAttribute[]){{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = 0
            }, {
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .offset = sizeof(float) * 3
            }, {
                .location = 2,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .offset = sizeof(float) * 7
            }},
            .num_vertex_attributes = 3,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = (SDL_GPUGraphicsPipelineTargetInfo){
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]) {{
                .format = SDL_GetGPUSwapchainTextureFormat(device, window),
                .blend_state = (SDL_GPUColorTargetBlendState){
                    .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .color_blend_op = SDL_GPU_BLENDOP_ADD,
                    .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                    .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_DST_ALPHA,
                    .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                    .color_write_mask = 0xF,
                    .enable_blend = true,
                }
            }},
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_INVALID,
            .has_depth_stencil_target = false,
        }
    };

    auto text_pipeline = SDL_CreateGPUGraphicsPipeline(device, &text_pipeline_create_infos);
    if (!text_pipeline) {
        SDL_ERROR("Couldn't create text pipeline");
    }

    SDL_ReleaseGPUShader(device, v_text_shader.data);
    SDL_ReleaseGPUShader(device, f_text_shader.data);

    char font_path[256];
    SDL_snprintf(font_path, 256, "%sfonts/Consolas7NF-Regular.ttf", RES_FOLDER_PATH);
    auto font = TTF_OpenFont(font_path, 12);

    auto text = TTF_CreateText(text_engine, font, "Hello, world!", 13);
    

    // WRITE VERTICES
    int widht;
    int height;
    SDL_GetWindowSize(window, &widht, &height);
    float width_ratio = (float)height / (float)widht;

    float board_size = 0.8f;
    float cell_size = 0.1f;
    float small_border_size = 0.01f;
    float big_border_size = 0.02f;

    int row = 1;
    int col = 1;

    float min_x = -board_size * width_ratio;
    float min_y = -board_size;
    float max_x = min_x + (cell_size * 2 * board_size * width_ratio);
    float max_y = min_y + (cell_size * 2 * board_size);

    for (int i = 0; i < 81 * 6; i += 6) {
        transfer_data[i] =      PositionColorVertex { .x = min_x, .y = min_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };
        transfer_data[i + 1] =  PositionColorVertex { .x = min_x, .y = max_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };
        transfer_data[i + 2] =  PositionColorVertex { .x = max_x, .y = min_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };
        transfer_data[i + 3] =  PositionColorVertex { .x = max_x, .y = min_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };
        transfer_data[i + 4] =  PositionColorVertex { .x = max_x, .y = max_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };
        transfer_data[i + 5] =  PositionColorVertex { .x = min_x, .y = max_y, .z = 0, .r = 50, .g = 50, .b = 50, .a = 255 };

        if (++col == 10) {
            col = 1;
            row++;
            min_x = -board_size * width_ratio;
            max_x = min_x + (cell_size * 2 * board_size * width_ratio);

            min_y += board_size * cell_size * 2;
            min_y += (row -1) % 3 == 0 
                ? big_border_size * 2 * board_size
                : small_border_size * 2 * board_size;
            
            max_y = min_y + (cell_size * 2 * board_size);
        } else {
            min_x += cell_size * 2 * board_size * width_ratio;
            min_x += (col -1) % 3 == 0 
                ? big_border_size * 2 * board_size * width_ratio
                : small_border_size * 2 * board_size * width_ratio;
            
            max_x = min_x + cell_size * 2 * board_size * width_ratio;
        }
    }

    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    auto command_buffer = SDL_AcquireGPUCommandBuffer(device);
    auto copy_pass = SDL_BeginGPUCopyPass(command_buffer);

    SDL_GPUTransferBufferLocation transfer_buffer_location = {
        .transfer_buffer = transfer_buffer,
        .offset = 0
    };

    SDL_GPUBufferRegion buffer_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(PositionColorVertex) * 6 * 81
    };

    SDL_UploadToGPUBuffer(copy_pass, &transfer_buffer_location, &buffer_region, false);

    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(command_buffer);
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    while(true) {
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                goto exit;
            }
        }

        // auto cmd_buffer = SDL_AcquireGPUCommandBuffer(device);
        // if (!cmd_buffer) {
        //     SDL_ERROR("Couldn't acquire gpu command buffer");
        // }

        // SDL_GPUTexture *swapchain_tex;
        // if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, window, &swapchain_tex, nullptr, nullptr)) {
        //     SDL_ERROR("Couldn't acquire swapchain texture");
        // }

        // if (swapchain_tex) {
        //     SDL_GPUColorTargetInfo color_target_info = { 0 };
        //     color_target_info.texture = swapchain_tex;
        //     color_target_info.clear_color = { 0.1f, 0.1f, 0.1f, 1.0f };
        //     color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        //     color_target_info.store_op = SDL_GPU_STOREOP_STORE;

        //     auto render_pass = SDL_BeginGPURenderPass(cmd_buffer, &color_target_info, 1, nullptr);

        //     SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
        //     SDL_GPUBufferBinding buffer_binding = {
        //         .buffer = vertex_buffer, 
        //         .offset = 0
        //     };
        //     SDL_BindGPUVertexBuffers(render_pass, 0, &buffer_binding, 1);
        //     SDL_DrawGPUPrimitives(render_pass, 6 * 81, 1, 0, 0);

        //     SDL_EndGPURenderPass(render_pass);
        // }

        // SDL_SubmitGPUCommandBuffer(cmd_buffer);

        
    }

exit:
    SDL_ReleaseGPUBuffer(device, vertex_buffer);
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyWindow(window);
    SDL_DestroyGPUDevice(device);
    TTF_Quit();

    return 0;
}

Shader LoadShader(SDL_GPUDevice *device, const char *name) {
    SDL_ShaderCross_ShaderStage stage;
	if (SDL_strstr(name, ".vert")) {
		stage = SDL_SHADERCROSS_SHADERSTAGE_VERTEX;
	} else if (SDL_strstr(name, ".frag")) {
		stage = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;
	} else {
        SDL_Log("Invalid shader name: %s", name);
        exit(1);
    }

    static char shader_path[256];
    SDL_snprintf(shader_path, sizeof(shader_path), "%s/shaders/%s", RES_FOLDER_PATH, name);
    size_t shader_code_size;
    void* shader_code = SDL_LoadFile(shader_path, &shader_code_size);
    if (!shader_code) {
        SDL_ERROR("Couldn't load shader file");
    }

    SDL_ShaderCross_HLSL_Info hlsl_infos = {
        .source = (char*)shader_code,
        .entrypoint = "main",
        .include_dir = nullptr,
        .defines = nullptr,
        .shader_stage = stage,
        .enable_debug = true,
        .name = nullptr,
        .props = 0,
    };

    Shader shader;
    shader.data = SDL_ShaderCross_CompileGraphicsShaderFromHLSL(device, &hlsl_infos, &shader.metadata);

    if (!shader.data) {
        SDL_ERROR("couldn't create shader");
    }

    SDL_free(shader_code);
    return shader;
}
