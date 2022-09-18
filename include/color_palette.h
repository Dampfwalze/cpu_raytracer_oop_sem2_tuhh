#include <array>
#include <rtmath.h>

namespace rt
{
    namespace m = math;

    class ColorPalette
    {
    public:
        using Color = m::Color<float>;

    private:
        std::array<Color, 255> m_colors;

    public:
        inline Color       &operator[](unsigned index) { return m_colors[index - 1]; }
        inline const Color &operator[](unsigned index) const { return m_colors[index - 1]; }
    };
} // namespace rt
