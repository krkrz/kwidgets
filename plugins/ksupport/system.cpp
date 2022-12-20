#define NOMINMAX
#include <Windows.h>

#include "ncbind.hpp"


class SystemSupport
{
public:
	static void sleep(tjs_int msec) {
		::Sleep(msec);
	}
};

NCB_ATTACH_CLASS(SystemSupport, System)
{
	NCB_METHOD(sleep);
};
