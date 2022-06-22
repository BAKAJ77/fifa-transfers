#include <util/timestamp.h>

#include <GLFW/glfw3.h>
#include <type_traits>
#include <ctime>

namespace Util
{
	std::string GetTimestampStr()
	{
		// Get the current date and time data
		std::time_t currentTime = std::time(nullptr);
		std::tm timeData;

		localtime_s(&timeData, &currentTime);

		// Now gather date and time data into one string
		std::string generatedTimestamp = std::to_string(timeData.tm_mday) + "/" + std::to_string(timeData.tm_mon + 1) + "/" +
			std::to_string(timeData.tm_year + 1900) + " " + std::to_string(timeData.tm_hour) + ":" +
			std::to_string(timeData.tm_min) + ":" + std::to_string(std::min(timeData.tm_sec, 59));

		// Convert singular digits in timestamp string to double digits e.g. "3/9/2002 ..." to "03/09/2002 ..."
		auto iterator = generatedTimestamp.begin();
		uint16_t digitCount = 0;

		while (iterator != generatedTimestamp.end())
		{
			if (*iterator == '/' || *iterator == ':' || *iterator == ' ') // Reached date/time value seperator?
			{
				if (digitCount == 1)
				{
					generatedTimestamp.insert(iterator - 1, '0');
					iterator = generatedTimestamp.begin();
					continue;
				}
				else
					digitCount = 0;
			}
			else
				digitCount++;

			if (iterator == generatedTimestamp.end() - 1) // Reached the end of the timestamp string?
			{
				if (digitCount == 1)
				{
					generatedTimestamp.insert(iterator, '0');
					iterator = generatedTimestamp.begin();
					break;
				}
			}
				
			iterator++;
		}

		return generatedTimestamp;
	}

	float GetSecondsSinceEpoch()
	{
		return (float)glfwGetTime();
	}
}