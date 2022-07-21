#ifndef RENDER_PARAMS_HPP
#define RENDER_PARAMS_HPP

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    struct RenderParams
    {
        m::u64vec2 tileSize;
        float mixingFactor = 1.88f;
        std::optional<m::u64vec2> logPixel;

        // Tone mapping
        float gamma = 2.2f;
        float scale = 1.0f;
    };

} // namespace rt

#endif // RENDER_PARAMS_HPP