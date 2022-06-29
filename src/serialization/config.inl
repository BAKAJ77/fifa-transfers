#include <serialization/config.h>

template<typename T> void ConfigLoader::SetElement(const std::string_view& elementName, T value, const std::string_view& groupName)
{
	groupName.empty() ?
		this->jsonData[elementName.data()] = value :
		this->jsonData[groupName.data()][elementName.data()] = value;
}

template<typename T> T ConfigLoader::GetElement(const std::string_view& elementName, const std::string_view& groupName)
{
	if (groupName.empty())
		return this->jsonData[elementName.data()];

	return this->jsonData[groupName.data()][elementName.data()];
}