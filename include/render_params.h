#ifndef RENDER_PARAMS_HPP
#define RENDER_PARAMS_HPP

#include <rtmath.h>

namespace rt
{
    struct RenderParams
    {
        math::Vec2<size_t> outputSize;
        math::Vec2<size_t> tileSize;
    };

} // namespace rt

#endif // RENDER_PARAMS_HPP