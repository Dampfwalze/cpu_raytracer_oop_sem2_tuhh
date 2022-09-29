#ifndef RENDER_PARAMS_HPP
#define RENDER_PARAMS_HPP

#include <rtmath.h>
#include <optional>

namespace rt
{
    namespace m = math;

    struct RenderParams
    {
        m::u64vec2 tileSize;
        float      mixingFactor = 1.88f;

        int recursionDeph = 3;

        std::optional<m::u64vec2> logPixel;

        // Tone mapping
        enum ToneMappingAlgorithm
        {
            None,
            Reinhard,
            Exposure,
            ToneMappingAlgorithm_COUNT,
        } toneMappingAlgorithm = Reinhard;
        float exposure = 1.0f;
        float gamma = 2.2f;
        float scale = 1.0f;
    };

    inline const char *toneMappingAlgorithmToString(RenderParams::ToneMappingAlgorithm alg)
    {
        switch (alg)
        {
        case RenderParams::Reinhard:
            return "Reinhard";
        case RenderParams::Exposure:
            return "Exposure";
        default:
            return "None";
        }
    }

} // namespace rt

#endif // RENDER_PARAMS_HPP