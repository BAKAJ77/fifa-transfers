#include <core/application_core.h>
//#include <serialization/json_loader.h>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <sstream>

int main(int argc, char** argv)
{
	ApplicationCore appCore;

	// CSV TO JSON CONVERSION CODE //
	/*std::vector<std::vector<std::string>> csvLines;
	std::string fileLine;

	std::ifstream file("sofifa-players-scraper.csv");
	while (std::getline(file, fileLine))
	{
		if (fileLine.find("Free") == std::string::npos)
		{
			std::istringstream inputStream(fileLine);
			std::string token;

			std::vector<std::string> csvTokens;
			while (std::getline(inputStream, token, ','))
				csvTokens.emplace_back(token);

			csvLines.emplace_back(csvTokens);
		}
	}

	JSONLoader jsonFile("players.json");
	for (int i = 0; i < csvLines.size(); i++)
	{
		jsonFile.GetRoot()[std::to_string(i)]["name"] = csvLines[i][0];
		jsonFile.GetRoot()[std::to_string(i)]["nation"] = csvLines[i][1];
		jsonFile.GetRoot()[std::to_string(i)]["age"] = std::stoi(csvLines[i][2]);
		jsonFile.GetRoot()[std::to_string(i)]["overall"] = std::stoi(csvLines[i][3]);
		jsonFile.GetRoot()[std::to_string(i)]["potential"] = std::stoi(csvLines[i][4]);
		jsonFile.GetRoot()[std::to_string(i)]["clubID"] = csvLines[i][5];

		if (csvLines[i][6].find("On loan") != std::string::npos)
			jsonFile.GetRoot()[std::to_string(i)]["expiryYear"] = 2024;
		else
			jsonFile.GetRoot()[std::to_string(i)]["expiryYear"] = std::stoi(csvLines[i][6].substr(7, 4));

		jsonFile.GetRoot()[std::to_string(i)]["preferredFoot"] = csvLines[i][7];
		jsonFile.GetRoot()[std::to_string(i)]["positionID"] = csvLines[i][8];

		const std::string value = csvLines[i][9], wage = csvLines[i][10], rc = csvLines[i][11];
		int iValue, iWage, iRC;

		if (value.back() == 'M')
			iValue = std::stof(value.substr(0, value.size() - 1)) * 1000000;
		else if (value.back() == 'K')
			iValue = std::stof(value.substr(0, value.size() - 1)) * 1000;
		else
			iValue = std::stof(value);

		if (wage.back() == 'M')
			iWage = std::stof(wage.substr(0, wage.size() - 1)) * 1000000;
		else if (wage.back() == 'K')
			iWage = std::stof(wage.substr(0, wage.size() - 1)) * 1000;
		else
			iWage = std::stof(wage);

		if (rc.back() == 'M')
			iRC = std::stof(rc.substr(0, rc.size() - 1)) * 1000000;
		else if (rc.back() == 'K')
			iRC = std::stof(rc.substr(0, rc.size() - 1)) * 1000;
		else
			iRC = std::stof(rc);

		jsonFile.GetRoot()[std::to_string(i)]["value"] = iValue;
		jsonFile.GetRoot()[std::to_string(i)]["wage"] = iWage;
		jsonFile.GetRoot()[std::to_string(i)]["releaseClause"] = iRC;
	}*/

	/*JSONLoader playerFile("players.json"), clubsFile("data/clubs.json"), positionsFile("data/positions.json");

	int playerID = 0;
	while (playerFile.GetRoot().contains(std::to_string(playerID)))
	{
		int clubID = 0, positionsID = 0;
		while (clubsFile.GetRoot().contains(std::to_string(clubID)))
		{
			if (clubsFile.GetRoot()[std::to_string(clubID)]["name"] == playerFile.GetRoot()[std::to_string(playerID)]["clubID"])
			{
				playerFile.GetRoot()[std::to_string(playerID)]["clubID"] = clubID;
				break;
			}

			clubID++;
		}

		while (positionsFile.GetRoot().contains(std::to_string(positionsID)))
		{
			if (positionsFile.GetRoot()[std::to_string(positionsID)]["position"] == playerFile.GetRoot()[std::to_string(playerID)]["positionID"])
			{
				playerFile.GetRoot()[std::to_string(playerID)]["positionID"] = positionsID;
				break;
			}

			positionsID++;
		}

		playerID++;
	}*/

	return 0;
}