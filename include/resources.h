#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <resource_container.h>
#include <voxel_grid.h>

namespace rt
{
    namespace Resources
    {
        class VoxelGridResource : public Resource
        {
        public:
            VoxelGrid grid;

        public:
            VoxelGridResource(const VoxelGrid &grid) : grid(grid) {}
            VoxelGridResource(VoxelGrid &&grid) : grid(std::move(grid)) {}

            inline VoxelGrid *operator->() { return &grid; }
            inline VoxelGrid &operator*() { return grid; }
        };

        class TextureResource : public Resource
        {
        private:
            m::uvec2 m_size;
            int m_channels;
            bool m_isHDR;
            union
            {
                unsigned char *asInt;
                float *asFloat;
            } m_texture;

        public:
            TextureResource(m::uvec2 size, int channels, unsigned char *texture) : m_size(size), m_channels(channels), m_isHDR(false), m_texture{.asInt = texture} {}
            TextureResource(m::uvec2 size, int channels, float *texture) : m_size(size), m_channels(channels), m_isHDR(true), m_texture{.asFloat = texture} {}

            ~TextureResource()
            {
                if (m_isHDR)
                    delete[] m_texture.asFloat;
                else
                    delete[] m_texture.asInt;
            }

            inline bool isHDR() { return m_isHDR; }
            inline m::uvec2 getSize() { return m_size; }
            inline void *get() { return m_texture.asInt; }
            inline int getChannels() { return m_channels; }
            inline m::Color<float> operator[](size_t index)
            {
                if (m_isHDR)
                {
                    auto px = m_texture.asFloat + index * m_channels;
                    return m::Color<float>(px[0], px[1], px[2]);
                }
                else
                {
                    auto px = m_texture.asInt + index * m_channels;
                    return m::Color<float>(px[0], px[1], px[2]) / 255.0f;
                }
            }
            inline m::Color<float> pixelAt(unsigned x, unsigned y) { return (*this)[x + m_size.x * y]; }
            inline m::Color<float> pixelAt(const m::uvec2 &p) { return (*this)[p.x + m_size.x * p.y]; }
        };
    } // namespace Resources
} // namespace rt

#endif // RESOURCES_HPP
