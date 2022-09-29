#include <file_dialog.h>

#include <nfd.h>

namespace rt
{
    namespace FileDialog
    {
        std::optional<std::filesystem::path> fileDialog(const std::string_view &filterList, const std::string_view &defaultPath)
        {
            nfdchar_t  *outPath = nullptr;
            nfdresult_t result = NFD_OpenDialog(filterList.data(), defaultPath.data(), &outPath);
            switch (result)
            {
            case NFD_OKAY:
            {
                auto path = std::filesystem::path(outPath);
                delete outPath;
                return path;
            }
            case NFD_CANCEL:
                return std::nullopt;

            case NFD_ERROR:
                throw std::runtime_error(NFD_GetError());

            default:
                throw std::runtime_error("Default case hit");
            }
        }

        std::optional<std::filesystem::path> folderDialog(const std::string_view &defaultPath)
        {
            nfdchar_t  *outPath = nullptr;
            nfdresult_t result = NFD_PickFolder(defaultPath.data(), &outPath);
            switch (result)
            {
            case NFD_OKAY:
            {
                auto path = std::filesystem::path(outPath);
                delete outPath;
                return path;
            }
            case NFD_CANCEL:
                return std::nullopt;

            case NFD_ERROR:
                throw std::runtime_error(NFD_GetError());

            default:
                throw std::runtime_error("Default case hit");
            }
        }

        std::optional<std::filesystem::path> saveDialog(const std::string_view &filterList, const std::string_view &defaultPath)
        {
            nfdchar_t  *outPath = nullptr;
            nfdresult_t result = NFD_SaveDialog(filterList.data(), defaultPath.data(), &outPath);
            switch (result)
            {
            case NFD_OKAY:
            {
                auto path = std::filesystem::path(outPath);
                delete outPath;
                return path;
            }
            case NFD_CANCEL:
                return std::nullopt;

            case NFD_ERROR:
                throw std::runtime_error(NFD_GetError());

            default:
                throw std::runtime_error("Default case hit");
            }
        }

    } // namespace FileDialog
} // namespace rt