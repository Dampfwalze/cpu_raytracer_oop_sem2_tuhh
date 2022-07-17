#include <resource_loaders.h>

#include <fstream>
#include <stdexcept>

namespace rt
{
    namespace ResourceLoaders
    {
        template <typename T>
        inline T read(std::ifstream &stream)
        {
            T buf;
            stream.read((char *)&buf, sizeof(T));
            return buf;
        }

        struct Chunck
        {
            union
            {
                char name[4];
                uint32_t nameAsInt;
            };
            uint32_t contentSize;
            uint32_t childrenSize;
        };

        inline constexpr uint32_t asInt(const char *str)
        {
            return *(int32_t *)str;
        }

        inline Chunck readChunk(std::ifstream &stream)
        {
            Chunck chunk;
            stream.read(chunk.name, sizeof(chunk.name));
            stream.read((char *)&chunk.contentSize, sizeof(chunk.contentSize));
            stream.read((char *)&chunk.childrenSize, sizeof(chunk.childrenSize));
            return chunk;
        }

        void VoxelGridLoader::load(ResourceRef<void> resource, const std::filesystem::path &path) const
        {
            std::cout << "Load voxels: " << path << std::endl;

            std::ifstream file(path, std::ios::binary);
            if (!file.is_open())
                throw std::ios_base::failure("Could not open file: \"" + path.string() + "\"");

            char prefix[4];
            file.read(prefix, sizeof(prefix));

            if (asInt(prefix) != asInt("VOX "))
                throw std::ios_base::failure("File is not of type vox!");

            uint32_t version = read<uint32_t>(file);
            if (version != 150)
                throw std::ios_base::failure("File version differs from expected, 150 expected, got: " + version);

            Chunck main = readChunk(file);
            if (main.nameAsInt != asInt("MAIN"))
                throw std::ios_base::failure("File has wrong format!");

            Chunck sizeChunk = readChunk(file);
            if (sizeChunk.nameAsInt == asInt("PACK"))
                throw std::ios_base::failure("Multiple models are not supported!");
            if (sizeChunk.nameAsInt != asInt("SIZE"))
                throw std::ios_base::failure("File has wrong format!");

            m::u64vec3 size(read<uint32_t>(file),
                            read<uint32_t>(file),
                            read<uint32_t>(file));

            VoxelGrid grid(size);

            Chunck xyziChunk = readChunk(file);
            if (xyziChunk.nameAsInt != asInt("XYZI"))
                throw std::ios_base::failure("File has wrong format!");

            uint32_t numVoxels = read<uint32_t>(file);

            for (size_t i = 0; i < numVoxels; i++)
            {
                m::u8vec4 voxel = read<m::u8vec4>(file);
                grid.at(voxel.xyz()).isVoxel = true;
            }

            resource.submit(std::make_unique<Resources::VoxelGridResource>(std::move(grid)));
        }
    }
}