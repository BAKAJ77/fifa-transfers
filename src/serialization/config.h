#ifndef CONFIG_H
#define CONFIG_H

#include <util/directory_system.h>
#include <util/logging_system.h>

#include <nlohmann/json.hpp>
#include <string_view>
#include <fstream>

namespace Serialization
{
	// Generates a new default config file.
	extern void GenerateConfigFile();

	// Returns the specified json element's value from the config file.
	template<typename Ty> Ty GetConfigElement(const std::string_view& elementGroupKey, const std::string_view& elementKey);
}

#include <serialization/config.inl>

#endif