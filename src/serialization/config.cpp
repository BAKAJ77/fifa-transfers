#include <serialization/config.h>

namespace Serialization
{
	void GenerateConfigFile()
	{
		// Create and setup the json object
		nlohmann::json jsonObject = 
		{
			{ "window", 
				{ 
					{ "resolution", { 1600, 900 } },
					{ "fullscreen", false },
					{ "vsync", false }
				}
			},
			{ "graphics",
				{
					{ "numSamplesMSAA", 2 },
					{ "gamma", 2.2f },
					{ "textQuality", 100 }
				}
			}
		};

		// Write the json data to the new config file
		std::ofstream configFile(Util::GetAppDataDirectory() + "config.json", std::ios::trunc);
		configFile << std::setw(4) << jsonObject;
	}
}