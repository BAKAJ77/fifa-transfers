#include <util/data_manip.h>
#include <string>
#include <cmath>

int Util::GetTruncatedSFInteger(int num, int sf_precision)
{
	std::string tempStr = std::to_string(num);
	int precision = static_cast<int>(std::pow(10, static_cast<int>(tempStr.size()) - sf_precision));

	double temp = num;
	temp /= precision;

	int finalNum = static_cast<int>(temp) * precision;
	return finalNum;
}
