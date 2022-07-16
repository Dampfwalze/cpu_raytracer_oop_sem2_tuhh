#ifndef VOXEL_GRID_HPP
#define VOXEL_GRID_HPP

#include <cstring>

#include <rtmath.h>

namespace rt
{
    namespace m = math;

    class VoxelGrid
    {
    public:
        struct Voxel
        {
            bool isVoxel;
        };

    private:
        m::u64vec3 m_size;

        Voxel *m_grid;

    public:
        inline size_t length() { return m_size.x * m_size.y * m_size.z; }

        VoxelGrid(const m::u64vec3 &size, Voxel *grid)
            : m_size(size), m_grid(grid) {}
        VoxelGrid(const m::u64vec3 &size)
            : VoxelGrid(size, new Voxel[size.x * size.y * size.z])
        {
            std::memset(m_grid, 0, length() * sizeof(Voxel));
        }

        VoxelGrid(const VoxelGrid &other)
            : VoxelGrid(other.m_size, new Voxel[other.m_size.x * other.m_size.y * other.m_size.z])
        {
            std::memcpy(m_grid, other.m_grid, length() * sizeof(Voxel));
        }

        ~VoxelGrid()
        {
            delete[] m_grid;
        }

        inline m::u64vec3 getSize() { return m_size; }

        inline Voxel &operator[](size_t index)
        {
            assert(index < length());
            return m_grid[index];
        }
        inline Voxel &at(m::u64vec3 i)
        {
            return (*this)[i.x + i.y * m_size.x + i.z * m_size.x * m_size.y];
        }
        inline Voxel &at(size_t x, size_t y, size_t z) { return at({x, y, z}); }
    };

} // namespace rt

#endif // VOXEL_GRID_HPP