#include <serialization/json_loader.h>
#include <util/directory_system.h>
#include <util/logging_system.h>

JSONLoader::JSONLoader(const std::string_view& fileName)
{
	this->Open(fileName);
}

JSONLoader::~JSONLoader()
{
	this->Close();
}

void JSONLoader::Open(const std::string_view& fileName)
{
	if (Util::IsExistingFile(Util::GetAppDataDirectory() + fileName.data()))
	{
		// Open the JSON file in read mode only
		this->fileStream.open(Util::GetAppDataDirectory() + fileName.data(), std::ios::in);
		if (this->fileStream.fail())
			LogSystem::GetInstance().OutputLog("Failed to open the JSON file: " + std::string(fileName), Severity::FATAL);

		this->fileStream.seekg(0); // Make sure the read pointer is positioned at the beginning of the file

		// Load the JSON config data
		std::string loadedJsonData, fetchedFileLine;
		while (std::getline(this->fileStream, fetchedFileLine))
			loadedJsonData += fetchedFileLine;

		this->fileStream.close();

		try
		{
			// Parse the loaded json data
			this->root = nlohmann::json::parse(loadedJsonData, nullptr, true, true);
		}
		catch (nlohmann::json::exception& exception) // Catch potential json exceptions thrown
		{
			LogSystem::GetInstance().OutputLog(exception.what(), Severity::FATAL);
		}
	}
	else
	{
		// Create a new JSON file
		this->fileStream.open(Util::GetAppDataDirectory() + fileName.data(), std::ios::out | std::ios::trunc);
		if (this->fileStream.fail())
			LogSystem::GetInstance().OutputLog("Failed to open the JSON file: " + std::string(fileName), Severity::FATAL);

		this->fileStream.close();
	}

	this->fileName = fileName;
}

void JSONLoader::Close()
{
	if (!this->root.empty())
	{
		// Open the JSON file
		this->fileStream.open(Util::GetAppDataDirectory() + fileName.data(), std::ios::out | std::ios::trunc);
		if (this->fileStream.fail())
			LogSystem::GetInstance().OutputLog("Failed to open the JSON file: " + std::string(fileName), Severity::FATAL);

		this->fileStream.seekp(0); // Make sure the write pointer is positioned at the beginning of the file

		this->fileStream << std::setw(4) << this->root;
		this->fileStream.close();
	}
}

void JSONLoader::Clear()
{
	this->root.clear();
}

nlohmann::json& JSONLoader::GetRoot()
{
	return this->root;
}

const std::string& JSONLoader::GetFileName() const
{
	return this->fileName;
}