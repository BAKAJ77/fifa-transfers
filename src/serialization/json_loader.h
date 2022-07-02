#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

class JSONLoader
{
private:
	std::string fileName;
	std::fstream fileStream;
	nlohmann::json root;
public:
	JSONLoader() = default;
	JSONLoader(const std::string_view& fileName);
	~JSONLoader();

	// Opens the specified JSON file.
	// If the JSON file doesn't exist, an empty JSON file will be created.
	void Open(const std::string_view& fileName);

	// Writes the JSON data stored into the JSON file and closes it.
	// Note that you don't need to call this function manually as it is automatically called by the destructor.
	// Also note that when you call this function, any JSON data that was loaded is still kept until the JSON Loader object 
	// is destroyed.
	void Close();

	// Returns the root of the loaded JSON data structure.
	nlohmann::json& GetRoot();

	// Returns the file name of the JSON file loaded.
	const std::string& GetFileName() const;
};

#endif