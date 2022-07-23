#include <util/data_manip.h>
#include <string>
#include <cmath>

int Util::GetTruncatedSFInteger(int num, int sfPrecision)
{
	std::string tempStr = std::to_string(num);
	int precision = static_cast<int>(std::pow(10, static_cast<int>(tempStr.size()) - sfPrecision));

	double temp = num;
	temp /= precision;

	int finalNum = static_cast<int>(temp) * precision;
	return finalNum;
}

std::string Util::GetFormattedCashString(int cashAmount)
{
	{
		std::string str = std::to_string(cashAmount);
		std::string comma = ",";

		const size_t STR_SIZE = str.size();
		size_t offsetCount = 1;

		for (auto it = str.rbegin(); it != str.rend(); it++)
		{
			if (offsetCount % 3 == 0)
				str.insert(STR_SIZE - offsetCount, comma);

			offsetCount++;
		}

		if (str.front() == ',')
			str.front() = '$';
		else
			str.insert(0, "$");

		// Re-check string for "out of place" comma if the number is negative
		for (auto it = str.begin(); it != str.end(); it++)
		{
			if (*it == ',' && *(it - 1) == '-')
				str.erase(it);
		}

		return str;
	}
}
