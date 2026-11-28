
#include <Q-Tip/Graphics/GPU/FragmentShader.h>
#include <Q-Tip/Graphics/Renderer.h>
#include <Q-Tip/Graphics/Texture.h>

#include <SDL3/SDL.h>

#include <array>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

QTIP_CODE_BEGIN

namespace {

struct ShaderSource {
    std::filesystem::path path;
    SDL_GPUShaderFormat format;
};

std::string sdlError(const std::string& message) {
    return message + ": " + SDL_GetError();
}

std::vector<std::byte> readBinaryFile(
    const std::filesystem::path& path
) {
    std::ifstream file(
        path,
        std::ios::binary | std::ios::ate
    );

    if (!file) {
        throw std::runtime_error(
            "Failed to open shader: " + path.string()
        );
    }

    const std::streampos end = file.tellg();

    if (end < 0) {
        throw std::runtime_error(
            "Failed to determine shader size: " + path.string()
        );
    }

    const auto size = static_cast<std::uintmax_t>(end);

    if (size > std::numeric_limits<std::size_t>::max() ||
        size > static_cast<std::uintmax_t>(
            std::numeric_limits<std::streamsize>::max()
        )) {
        throw std::runtime_error(
            "Shader file is too large: " + path.string()
        );
    }

    std::vector<std::byte> data(
        static_cast<std::size_t>(size)
    );

    file.seekg(0, std::ios::beg);

    if (!data.empty()) {
        file.read(
            reinterpret_cast<char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
    }

    if (!file) {
        throw std::runtime_error(
            "Failed to read shader: " + path.string()
        );
    }

    return data;
}

ShaderSource chooseShader(
    const std::filesystem::path& basePath,
    SDL_GPUShaderFormat formats
) {
    struct Candidate {
        SDL_GPUShaderFormat format;
        const char* extension;
    };

    // Prefer portable SPIR-V, then native platform formats.
    constexpr std::array candidates = {
        Candidate{SDL_GPU_SHADERFORMAT_SPIRV,    ".spv"},
        Candidate{SDL_GPU_SHADERFORMAT_MSL,      ".msl"},
        Candidate{SDL_GPU_SHADERFORMAT_METALLIB, ".metallib"},
        Candidate{SDL_GPU_SHADERFORMAT_DXIL,     ".dxil"},
        Candidate{SDL_GPU_SHADERFORMAT_DXBC,     ".dxbc"}
    };

    for (const Candidate& candidate : candidates) {
        if (!(formats & candidate.format)) {
            continue;
        }

        std::filesystem::path path = basePath;
        path += candidate.extension;

        if (std::filesystem::exists(path)) {
            return {
                std::move(path),
                candidate.format
            };
        }
    }

    throw std::runtime_error(
        "No supported shader file found for: " +
        basePath.string()
    );
}

}

struct FragmentShader::Impl {
    SDL_Renderer* renderer = nullptr;
    SDL_GPUDevice* device = nullptr;

    SDL_GPUShader* shader = nullptr;
    SDL_GPURenderState* state = nullptr;

    SDL_GPUSampler* sampler = nullptr;

    std::vector<SDL_GPUTextureSamplerBinding> samplerBindings;

    bool active = false;

    void initialize(
        const std::filesystem::path& basePath,
        uint32_t numSamplers,
        uint32_t numUniformBuffers
    ) {
        if (!renderer) {
            throw std::runtime_error(
                "FragmentShader received an invalid renderer"
            );
        }

        device = SDL_GetGPURendererDevice(renderer);

        if (!device) {
            throw std::runtime_error(
                "Renderer does not use SDL GPU rendering"
            );
        }

        const SDL_GPUShaderFormat formats =
            SDL_GetGPUShaderFormats(device);

        if (formats == SDL_GPU_SHADERFORMAT_INVALID) {
            throw std::runtime_error(
                "GPU does not support any shader formats"
            );
        }

        const ShaderSource source = chooseShader(
            basePath,
            formats
        );

        const std::vector<std::byte> code =
            readBinaryFile(source.path);

        SDL_GPUShaderCreateInfo shaderInfo{};

        shaderInfo.code_size = code.size();

        shaderInfo.code =
            reinterpret_cast<const Uint8*>(code.data());

        shaderInfo.format = source.format;
        shaderInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;

        shaderInfo.num_samplers = numSamplers;
        shaderInfo.num_uniform_buffers = numUniformBuffers;

        shaderInfo.num_storage_textures = 0;
        shaderInfo.num_storage_buffers = 0;

        shader = SDL_CreateGPUShader(
            device,
            &shaderInfo
        );

        if (!shader) {
            throw std::runtime_error(
                sdlError("Failed to create fragment shader")
            );
        }

        // Create the default sampler.
        if (numSamplers > 0) {
            SDL_GPUSamplerCreateInfo samplerInfo{};

            samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
            samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;

            samplerInfo.mipmap_mode =
                SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;

            samplerInfo.address_mode_u =
                SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

            samplerInfo.address_mode_v =
                SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

            samplerInfo.address_mode_w =
                SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

            sampler = SDL_CreateGPUSampler(
                device,
                &samplerInfo
            );

            if (!sampler) {
                throw std::runtime_error(
                    sdlError("Failed to create GPU sampler")
                );
            }
        }

        SDL_GPURenderStateCreateInfo stateInfo{};

        stateInfo.fragment_shader = shader;

        state = SDL_CreateGPURenderState(
            renderer,
            &stateInfo
        );

        if (!state) {
            throw std::runtime_error(
                sdlError("Failed to create GPU render state")
            );
        }

        samplerBindings.resize(numSamplers);

        for (auto& binding : samplerBindings) {
            binding.sampler = sampler;
        }
    }

    void destroy() {
        if (active && renderer) {
            SDL_SetGPURenderState(renderer, nullptr);
            active = false;
        }

        if (state) {
            SDL_DestroyGPURenderState(state);
            state = nullptr;
        }

        if (sampler && device) {
            SDL_ReleaseGPUSampler(device, sampler);
            sampler = nullptr;
        }

        if (shader && device) {
            SDL_ReleaseGPUShader(device, shader);
            shader = nullptr;
        }

        device = nullptr;
        renderer = nullptr;
    }

    ~Impl() {
        destroy();
    }
};

FragmentShader::FragmentShader(
    Renderer& renderer,
    const std::filesystem::path& shaderPath,
    uint32_t numSamplers,
    uint32_t numUniformBuffers
) : _impl(std::make_unique<Impl>()) {
    _impl->renderer = renderer;

    _impl->initialize(
        shaderPath,
        numSamplers,
        numUniformBuffers
    );
}

FragmentShader::~FragmentShader() = default;

FragmentShader::FragmentShader(
    FragmentShader&& other
) noexcept = default;

FragmentShader& FragmentShader::operator=(
    FragmentShader&& other
) noexcept = default;

void FragmentShader::activate() {
    if (!_impl || !_impl->state) {
        throw std::runtime_error(
            "Cannot activate an invalid fragment shader"
        );
    }

    if (!SDL_SetGPURenderState(
        _impl->renderer,
        _impl->state
    )) {
        throw std::runtime_error(
            sdlError("Failed to activate fragment shader")
        );
    }

    _impl->active = true;
}

void FragmentShader::deactivate() {
    if (!_impl || !_impl->renderer) {
        return;
    }

    if (!SDL_SetGPURenderState(
        _impl->renderer,
        nullptr
    )) {
        throw std::runtime_error(
            sdlError("Failed to deactivate fragment shader")
        );
    }

    _impl->active = false;
}

bool FragmentShader::isValid() const {
    return _impl &&
           _impl->shader &&
           _impl->state;
}

bool FragmentShader::setUniform(
    uint32_t slot,
    const void* data,
    uint32_t size
) {
    if (!isValid() || !data || size == 0) {
        return false;
    }

    return SDL_SetGPURenderStateFragmentUniforms(
        _impl->state,
        slot,
        data,
        size
    );
}

bool FragmentShader::setTexture(
    uint32_t slot,
    const Texture& texture
) {
    if (!isValid() ||
        slot >= _impl->samplerBindings.size() ||
        !texture.isValid() ||
        !_impl->sampler) {
        return false;
    }

    SDL_Texture* sdlTexture = texture.nativeHandle();

    if (!sdlTexture) {
        return false;
    }

    const SDL_PropertiesID properties =
        SDL_GetTextureProperties(sdlTexture);

    if (!properties) {
        return false;
    }

    auto* gpuTexture =
        static_cast<SDL_GPUTexture*>(
            SDL_GetPointerProperty(
                properties,
                SDL_PROP_TEXTURE_GPU_TEXTURE_POINTER,
                nullptr
            )
        );

    if (!gpuTexture) {
        return false;
    }

    // Preserve all other sampler slots.
    auto bindings = _impl->samplerBindings;

    bindings[slot].texture = gpuTexture;
    bindings[slot].sampler = _impl->sampler;

    if (!SDL_SetGPURenderStateSamplerBindings(
        _impl->state,
        static_cast<int>(bindings.size()),
        bindings.data()
    )) {
        return false;
    }

    _impl->samplerBindings = std::move(bindings);

    return true;
}

QTIP_CODE_END
