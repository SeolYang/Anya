#include <PCH.h>
#include <Resources/ResourceLoader.h>
#include <Core/CommandLineParser.h>

namespace anya
{
    std::shared_ptr<Text> ResourceLoader::LoadTextFile(std::filesystem::path absolutePath)
    {
        return std::shared_ptr<Text>();
    }

    std::shared_ptr<Text> ResourceLoader::LoadTextFile(CommandLineParser& cmdLineParser, fs::path relativePath)
    {
        auto absolutePath = cmdLineParser.ExecutablePath();
        absolutePath /= relativePath;
        return std::move(LoadTextFile(absolutePath));
    }

    std::shared_ptr<Model> ResourceLoader::LoadModelFile(std::filesystem::path absolutePath)
    {
        return std::shared_ptr<Model>();
    }

    std::shared_ptr<Model> ResourceLoader::LoadModelFile(CommandLineParser& cmdLineParser, fs::path relativePath)
    {
        auto absolutePath = cmdLineParser.ExecutablePath();
        absolutePath /= relativePath;
        return std::move(LoadModelFile(absolutePath));
    }

    std::shared_ptr<Image> ResourceLoader::LoadImageFile(std::filesystem::path absolutePath)
    {
        return std::shared_ptr<Image>();
    }

    std::shared_ptr<Image> ResourceLoader::LoadImageFile(CommandLineParser& cmdLineParser, fs::path relativePath)
    {
        auto absolutePath = cmdLineParser.ExecutablePath();
        absolutePath /= relativePath;
        return std::move(LoadImageFile(absolutePath));
    }
}