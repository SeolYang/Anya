#include <PCH.h>
#include <Core/CommandLineParser.h>

namespace anya
{
	CommandLineParser::CommandLineParser(int argc, char** argv)
	{
		assert(argc > 0);

		// parentPath/executable.exe
		executablePath = std::filesystem::path{ argv[0] }.parent_path();

		for (int idx = 1; idx < argc; ++idx)
		{
			if (auto bIsInvalidArgument = !Argument(argv[idx]); bIsInvalidArgument)
			{
				// @TODO: Error message here for invalid argument.
			}
		}
	}

	bool CommandLineParser::Argument(char* argument)
	{
		if (strcmp(argument, "-enable_debug_layer") == 0)
		{
			bShouldEnableDebugLayer = true;
			return true;
		}

		return false;
	}
}