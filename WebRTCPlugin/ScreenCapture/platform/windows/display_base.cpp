#include <cmath>
#include <codecvt>
#include <initguid.h>

#include <boost/process.hpp>

// We have to include boost/process.hpp before display.h due to WinSock.h,
// but that prevents the definition of NTSTATUS so we must define it ourself.
typedef long NTSTATUS;

#include "display.h"
#include "../../../Logger.h"

namespace platf {
	std::vector<std::string>
		display_names(mem_type_e)
	{
		std::vector<std::string> display_names;
		HRESULT status;

		LOG_DEBUG("Detecting monitors...");

		return std::vector<std::string>();
	}
}