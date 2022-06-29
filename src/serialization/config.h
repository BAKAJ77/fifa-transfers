#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

class ConfigLoader
{
private:
	std::string fileName;
	std::fstream fileStream;
	nlohmann::json jsonData;
public:
	ConfigLoader() = default;
	ConfigLoader(const std::string_view& fileName);
	~ConfigLoader();

	// Opens the specified config file.
	// If the config file doesn't exist, an empty config file will be created.
	void Open(const std::string_view& fileName);

	// Writes the json data stored into the config file and closes it.
	// Note that you don't need to call this function manually as it is automatically called by the destructor.
	// Also note that when you call this function, any JSON data that was loaded is still kept until the config loader object 
	// is destroyed.
	void Close();

	// Sets the value of the JSON element specified.
	template<typename T>
	void SetElement(const std::string_view& elementName, T value, const std::string_view& groupName = "");

	// Returns the value of the JSON element specified.
	template<typename T>
	T GetElement(const std::string_view& elementName, const std::string_view& groupName = "");

	// Returns TRUE if the specified json group or element exists.
	bool IsEntryExisting(const std::string_view& entryName) const;

	const std::string& GetFileName() const;
};

#include <serialization/config.inl>

#endif