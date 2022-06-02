#pragma once
#include <PCH.h>

namespace sy
{
	class CommandLineParser
	{
	public:
		CommandLineParser(int argc, wchar_t** argv);

		[[nodiscard]] std::filesystem::path ExecutablePath() const  noexcept { return executablePath; }
		[[nodiscard]] bool ShouldEnableDebugLayer() const  noexcept { return bShouldEnableDebugLayer; }

	private:
		bool Argument(wchar_t* argument);

	private:
		std::filesystem::path executablePath;

#if defined(DEBUG) || defined(_DEBUG)
		bool bShouldEnableDebugLayer = true;
#else
		bool bShouldEnableDebugLayer = false;
#endif

	};
}