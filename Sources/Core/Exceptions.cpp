#include <PCH.h>
#include <Core/Exceptions.h>
#include <Core/Utility.h>

namespace sy
{
    Exception::Exception(const std::string& ansiExceptionMessage) :
        message(utils::AnsiToWString(ansiExceptionMessage))
    {
    }
}