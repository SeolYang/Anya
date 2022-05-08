#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>

anya::int32 main(int argc, wchar_t** argv)
{
	auto app = std::make_unique<anya::Application>(TEXT("AnyaRenderer"), argc, argv);
	std::wcout << app->Title() << std::endl;

	return app->Execute();
}