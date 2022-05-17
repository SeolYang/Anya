#pragma once
#include <PCH.h>

namespace sy
{
    class Text;
    class Model;
    class Image;
    class CommandLineParser;
    class ResourceLoader
    {
        /**
        * Load Text file from absolute path.
        */
        static std::shared_ptr<Text> LoadTextFile(std::filesystem::path absolutePath);

        /**
        * Load Text file from relative path which is relatively to ExecutablePath of command line parser.
        */
        static std::shared_ptr<Text> LoadTextFile(CommandLineParser& cmdLineParser, fs::path relativePath);

        /**
        * Load Model file from absolute path.
        */
        static std::shared_ptr<Model> LoadModelFile(std::filesystem::path absolutePath);

        /**
        * Load Model file from relative path which is relatively to ExecutablePath of command line parser.
        */
        static std::shared_ptr<Model> LoadModelFile(CommandLineParser& cmdLineParser, fs::path relativePath);

        /**
        * Load Image file from absolute path.
        */
        static std::shared_ptr<Image> LoadImageFile(std::filesystem::path absolutePath);

        /**
        * Load Image file from relative path which is relatively to ExecutablePath of command line parser.
        */
        static std::shared_ptr<Image> LoadImageFile(CommandLineParser& cmdLineParser, fs::path relativePath);
    };
}