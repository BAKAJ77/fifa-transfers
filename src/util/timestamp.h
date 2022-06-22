#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <string>

namespace Util
{
	// Returns the retrieved current date and time.
	extern std::string GetTimestampStr();

	// Returns the elapsed time, in seconds, since the GLFW initialization.
	// It uses the highest-resolution monotonic time source on each operating system.
	extern float GetSecondsSinceEpoch();
}

#endif
