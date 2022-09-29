#ifndef FILE_DIALOG_HPP
#define FILE_DIALOG_HPP

#include <filesystem>
#include <optional>
#include <string>

namespace rt
{
    namespace FileDialog
    {
        std::optional<std::filesystem::path> fileDialog(const std::string_view &filterList = "", const std::string_view &defaultPath = "");
        std::optional<std::filesystem::path> folderDialog(const std::string_view &defaultPath = "");
        std::optional<std::filesystem::path> saveDialog(const std::string_view &filterList = "", const std::string_view &defaultPath = "");
    } // namespace FileDialog
} // namespace rt

#endif // FILE_DIALOG_HPP