
#ifndef QTIP_FRAGMENTSHADER_H
#define QTIP_FRAGMENTSHADER_H

#include <cstdint>
#include <filesystem>
#include <memory>
#include <type_traits>

#include <Q-Tip/Config.h>

#include "Q-Tip/Graphics/Texture.h"

QTIP_CODE_BEGIN
class Renderer;

class FragmentShader {
public:
    FragmentShader(
        Renderer& renderer,
        const std::filesystem::path& shaderPath,
        uint32_t numSamplers = 0,
        uint32_t numUniformBuffers = 0
    );

    ~FragmentShader();

    FragmentShader(const FragmentShader&) = delete;
    FragmentShader& operator=(const FragmentShader&) = delete;

    FragmentShader(FragmentShader&& other) noexcept;
    FragmentShader& operator=(FragmentShader&& other) noexcept;

    void activate();
    void deactivate();

    [[nodiscard]] bool isValid() const;

    bool setUniform(
        uint32_t slot,
        const void* data,
        uint32_t size
    );

    template<typename T>
    bool setUniform(uint32_t slot, const T& value) {
        static_assert(
            std::is_trivially_copyable_v<T>,
            "Shader uniforms must be trivially copyable"
        );

        return setUniform(
            slot,
            &value,
            sizeof(T)
        );
    }

    bool setTexture(
        uint32_t slot,
        const Texture& texture
    );

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

QTIP_CODE_END

#endif // QTIP_FRAGMENTSHADER_H
