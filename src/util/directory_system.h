#ifndef DIRECTORY_SYSTEM_H
#define DIRECTORY_SYSTEM_H

#include <string>
#include <string_view>

namespace Util
{
	// Creates directories which doesnt exist in the directories path given.
	// Returns TRUE if successful, else FALSE is returned.
	extern bool CreateNewDirectory(const std::string_view& directory);

	// Returns the path to the parent directory to deriving directories owned by the game such as /assets, /data etc.
	extern std::string GetGameRequisitesDirectory();

	// Returns TRUE if the directory at the path given exists, if it doesn't then FALSE is returned.
	extern bool IsExistingDirectory(const std::string_view& directory);

	// Returns TRUE if the file specified exists, else FALSE is returned.
	extern bool IsExistingFile(const std::string_view& filePath);
}

#endif
