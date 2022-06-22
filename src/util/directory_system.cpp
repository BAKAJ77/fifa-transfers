#include <util/directory_system.h>

#include <filesystem>
#include <fstream>

namespace Util
{
	bool CreateNewDirectory(const std::string_view& directory)
	{
		return std::filesystem::create_directories(directory);
	}

	extern std::string GetGameRequisitesDirectory()
	{
#ifdef _DEBUG
		return std::string(); // Directories are relative to project file so return empty string
#else
		char* directoryBuffer = nullptr;
		size_t bufferSize = 0;
		_dupenv_s(&directoryBuffer, &bufferSize, "APPDATA");

		if (!directoryBuffer) // An error must've occurred
			throw std::exception("Failed to fetch the game's requisites directory");

		std::string directory = directoryBuffer + std::string("/square-run/");
		std::replace(directory.begin(), directory.end(), '\\', '/');

		if (!Util::IsExistingDirectory(directory))
			throw std::exception("The fetched game's requisites directory does not exist");

		return directory;
#endif
	}

	bool IsExistingDirectory(const std::string_view& directory)
	{
		return std::filesystem::exists(directory);
	}

	bool IsExistingFile(const std::string_view& filePath)
	{
		std::ifstream file(filePath.data());
		return !file.fail();
	}
}