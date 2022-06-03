#include <PCH.h>
#include <Core/Application.h>

sy::int32 main(int argc, wchar_t** argv)
{
	using namespace sy;
	const auto app = std::make_unique<Application>(TEXT("Anya"), argc, argv);
	return app->Execute();
}