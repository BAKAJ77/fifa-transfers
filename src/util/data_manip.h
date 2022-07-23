#ifndef DATA_MANIP_H
#define DATA_MANIP_H

#include <string>

namespace Util
{
	// Returns integer truncated to the amount of significant figures specified.
	extern int GetTruncatedSFInteger(int num, int sfPrecision);

	// Returns a formatted string representing the integer cash amount specified.
	extern std::string GetFormattedCashString(int cashAmount);
}

#endif