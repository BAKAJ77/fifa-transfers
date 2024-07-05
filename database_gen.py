import json

# Converts a formatted value string to an integer, e.g. 35.5M gets converted to 35500000
def ConvertValueString(valueStr: str):
    output = 0
    if valueStr[-1] == "M":
        output = int(float(valueStr[:len(valueStr) - 1]) * 1000000)
    elif valueStr[-1] == "K":
        output = int(float(valueStr[:len(valueStr) - 1]) * 1000)
    else:
        output = int(valueStr)

    return output

##################################### Club JSON Database Generation #####################################

parsedClubsCSV = []

# Open the club csv database file and tokenise each loaded file line
csvClubDatabase = open("clubDatabase.csv", encoding="utf8")
for fileLine in csvClubDatabase:
    fileLine = fileLine.replace("\n", "")
    tokens = fileLine.split(",")
    parsedClubsCSV.append(tokens)
        
csvClubDatabase.close()

# Create and store a dictionary object for each club using the parsed token data
clubsDatabase = {}

for index in range(len(parsedClubsCSV)):
    tokens = parsedClubsCSV[index]
    clubData = {
        "name": tokens[0],
        "leagueID": tokens[1],
        "transferBudget": 0
    }

    clubsDatabase[str(index)] = clubData

# Sync the league ID for each club to the ones defined in the league JSON database
leagueJsonFile = open("leagues.json")
leagueDatabase = json.loads(leagueJsonFile.read())
leagueJsonFile.close()

for clubID in range(len(clubsDatabase)):
    for leagueID in range(len(leagueDatabase)):
        if leagueDatabase[str(leagueID)]["name"] == clubsDatabase[str(clubID)]["leagueID"]:
            clubsDatabase[str(clubID)]["leagueID"] = leagueID
            break

# Finally dump the player JSON database to file
clubsJsonOutput = json.dumps(clubsDatabase, indent=4)
clubsJsonFile = open("clubs.json", "w")
clubsJsonFile.write(clubsJsonOutput)

#################################### Player JSON Database Generation ####################################

parsedPlayersCSV = []

# Open the player csv database file and tokenise each loaded file line
csvPlayerDatabase = open("playerDatabase.csv", encoding="utf8")
for fileLine in csvPlayerDatabase:
    if fileLine.find("Free") == -1:
        fileLine = fileLine.replace("\n", "")
        tokens = fileLine.split(",")

        if len(tokens) > 12:
            tokens.pop(6)

        parsedPlayersCSV.append(tokens)

csvPlayerDatabase.close()

# Open the clubs and positions JSON database files and load them
clubsJsonFile = open("clubs.json")
positionsJsonFile = open("positions.json")

clubsDatabase = json.loads(clubsJsonFile.read())
positionsDatabase = json.loads(positionsJsonFile.read())

clubsJsonFile.close()
positionsJsonFile.close()

# Create and store a dictionary object for each player using the parsed token data
playerDatabase = {}

playerIDIndex = 0
for tokens in parsedPlayersCSV:
    playerData = {
        "name": tokens[0],
        "nation": tokens[1],
        "age": int(tokens[2]),
        "overall": int(tokens[3]),
        "potential": int(tokens[4]),
        "clubID": tokens[5],
        "expiryYear": tokens[6],
        "preferredFoot": tokens[7],
        "positionID": tokens[8],
        "value": tokens[9],
        "wage": tokens[10],
        "releaseClause": tokens[11]
    }

    # Find the ID of the player's club
    clubFound = False
    for clubID in range(len(clubsDatabase)):
        if clubsDatabase[str(clubID)]["name"] == playerData["clubID"]:
            playerData["clubID"] = clubID
            clubFound = True
            break

    if clubFound == False: # Club wasn't found so skip the player
        continue

    # Find the ID of the player's position
    for positionID in range(len(positionsDatabase)):
        if positionsDatabase[str(positionID)]["position"] == playerData["positionID"]:
            playerData["positionID"] = positionID
            break

    # Convert the player's expiry year string into an integer format
    if playerData["expiryYear"].find("On loan") > -1:
        playerData["expiryYear"] = 2024
    else:
        playerData["expiryYear"] = int(playerData["expiryYear"][7:])

    # Convert transfer, wage and release clause string values to integer format
    playerData["value"] = ConvertValueString(playerData["value"])
    playerData["wage"] = ConvertValueString(playerData["wage"])
    playerData["releaseClause"] = ConvertValueString(playerData["releaseClause"])

    playerDatabase[str(playerIDIndex)] = playerData # Add player data to database
    playerIDIndex += 1

# Finally dump the player JSON database to file
playersJsonOutput = json.dumps(playerDatabase, indent=4)
playersJsonFile = open("players.json", "w")
playersJsonFile.write(playersJsonOutput)

#########################################################################################################