#pragma once
#include <PCH.h>

namespace anya
{
	class CommandLineParser
	{
	public:
		CommandLineParser(int argc, char** argv);

		inline std::filesystem::path ExecutablePath() const { return executablePath; }
		inline bool ShouldEnableDebugLayer() const { return bShouldEnableDebugLayer; }

	private:
		bool Argument(char* argument);

	private:
		std::filesystem::path executablePath;
		bool bShouldEnableDebugLayer = false;

	};
}