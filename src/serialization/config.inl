#include <serialization/config.h>

template<typename Ty> Ty Serialization::GetConfigElement(const std::string_view& elementGroupKey, const std::string_view& elementKey)
{
	// Open the json file and load the contents
	std::ifstream jsonFile(Util::GetAppDataDirectory() + "config.json");
	if (jsonFile.fail())
		LogSystem::GetInstance().OutputLog("Couldn't open the app config file", Severity::FATAL);

	std::string jsonData, fileLine;
	while (std::getline(jsonFile, fileLine))
		jsonData += fileLine;

	Ty elementData; // This will hold the retrieved json element value

	try
	{
		// Parse the loaded json data
		const nlohmann::json loadedJSON = nlohmann::json::parse(jsonData, nullptr, true, true);

		// Retrieve and return the requested json element
		elementData = loadedJSON.at(elementGroupKey.data()).at(elementKey.data()).get<Ty>();
	}
	catch (nlohmann::json::exception& exception) // Catch potential json exceptions thrown
	{
		LogSystem::GetInstance().OutputLog(exception.what(), Severity::FATAL);
	}

	return elementData;
}