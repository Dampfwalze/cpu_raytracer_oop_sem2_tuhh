#ifndef RENDER_PARAMS_HPP
#define RENDER_PARAMS_HPP

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    struct RenderParams
    {
        m::u64vec2 outputSize;
        m::u64vec2 tileSize;
    };

} // namespace rt

#endif // RENDER_PARAMS_HPP