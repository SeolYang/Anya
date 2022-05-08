#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>

class Renderable
{
};

class Visible
{
};

anya::int32 main(int argc, wchar_t** argv)
{
	auto app = std::make_unique<anya::Application>(TEXT("AnyaRenderer"), argc, argv);

	return app->Execute();
}