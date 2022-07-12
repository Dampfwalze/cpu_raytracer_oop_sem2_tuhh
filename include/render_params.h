#ifndef RENDER_PARAMS_HPP
#define RENDER_PARAMS_HPP

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    struct RenderParams
    {
        m::u64vec2 tileSize;
        std::optional<m::u64vec2> logPixel;
    };

} // namespace rt

#endif // RENDER_PARAMS_HPP