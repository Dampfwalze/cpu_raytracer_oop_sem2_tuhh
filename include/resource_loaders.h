#ifndef RESOURCE_LOADERS_HPP
#define RESOURCE_LOADERS_HPP

#include <resource_container.h>
#include <resources.h>

namespace rt
{
    namespace ResourceLoaders
    {
        class VoxelGridLoader : public ResourceLoader
        {
        public:
            virtual void load(ResourceRef<void> resource, const std::filesystem::path &path) const override;
        };

    } // namespace ResourceLoaders
} // namespace rt

#endif // RESOURCE_LOADERS_HPP