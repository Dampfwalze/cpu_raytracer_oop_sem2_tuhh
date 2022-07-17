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
    } // namespace Resources
} // namespace rt

#endif // RESOURCES_HPP
