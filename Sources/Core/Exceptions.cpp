#include <PCH.h>
#include <Core/Exceptions.h>
#include <Core/Utility.h>

namespace anya
{
    Exception::Exception(const std::string& ansiExceptionMessage) :
        message(AnsiToWString(ansiExceptionMessage))
    {
    }
}