#include <resource_loaders.h>

#include <fstream>
#include <stdexcept>

#include <stb_image.h>

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
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open())
                throw IOException(IOException::Type::NotFound);

            char prefix[4];
            file.read(prefix, sizeof(prefix));

            if (asInt(prefix) != asInt("VOX "))
                throw IOException(IOException::Type::WrongType);

            uint32_t version = read<uint32_t>(file);
            if (version != 150)
                throw IOException(IOException::Type::WrongType, "File version differs from expected, 150 expected, got: " + version);

            Chunck main = readChunk(file);
            if (main.nameAsInt != asInt("MAIN"))
                throw IOException(IOException::Type::FileCorrupt, "File has wrong format!");

            Chunck sizeChunk = readChunk(file);
            if (sizeChunk.nameAsInt == asInt("PACK"))
                throw IOException(IOException::Type::WrongType, "Multiple models are not supported!");
            if (sizeChunk.nameAsInt != asInt("SIZE"))
                throw IOException(IOException::Type::FileCorrupt, "File has wrong format!");

            // Parameter evaluation order is undefined by c++ standart!!!!
            m::u64vec3 size;
            size.x = read<uint32_t>(file);
            size.y = read<uint32_t>(file);
            size.z = read<uint32_t>(file);

            VoxelGrid grid(size);

            Chunck xyziChunk = readChunk(file);
            if (xyziChunk.nameAsInt != asInt("XYZI"))
                throw IOException(IOException::Type::FileCorrupt, "File has wrong format!");

            uint32_t numVoxels = read<uint32_t>(file);

            for (size_t i = 0; i < numVoxels; i++)
            {
                m::u8vec4 voxel = read<m::u8vec4>(file);
                grid.at(voxel.xyz()).colorIndex = voxel.w;
            }

            resource.submit(std::make_unique<Resources::VoxelGridResource>(std::move(grid)));
        }

        void TextureLoader::load(ResourceRef<void> resource, const std::filesystem::path &path) const
        {
            auto pathStr = std::move(path.string());

            bool isHDR = stbi_is_hdr(pathStr.c_str());
            if (isHDR)
            {
                int w, h, c;
                auto img = stbi_loadf(pathStr.c_str(), &w, &h, &c, 0);
                if (img == nullptr)
                    throw IOException(IOException::Type::FileCorrupt, stbi_failure_reason());

                resource.submit(std::make_unique<Resources::TextureResource>(m::uvec2(w, h), c, img));
            }
            else
            {
                int w, h, c;
                auto img = stbi_load(pathStr.c_str(), &w, &h, &c, 0);
                if (img == nullptr)
                    throw IOException(IOException::Type::FileCorrupt, stbi_failure_reason());

                resource.submit(std::make_unique<Resources::TextureResource>(m::uvec2(w, h), c, img));
            }
        }
    }
}