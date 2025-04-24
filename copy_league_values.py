import json

oldLeagueDatabase = {}
newLeagueDatabase = {}

# Load the databases
with open("old_leagues.json") as file:
    oldLeagueDatabase = json.load(file)

with open("leagues.json") as file:
    newLeagueDatabase = json.load(file)

# Copy the values from the old databases to the new ones
for newLeagueID in range(len(newLeagueDatabase)):
    for oldLeagueID in range(len(oldLeagueDatabase)):
        oldLeague = oldLeagueDatabase[str(oldLeagueID)]
        newLeague = newLeagueDatabase[str(newLeagueID)]

        if newLeague["name"] == oldLeague["name"]:
            newLeague["tier"] = oldLeague["tier"]
            newLeague["titleBonus"] = oldLeague["titleBonus"]
            newLeague["autoPromotionThreshold"] = oldLeague["autoPromotionThreshold"]
            newLeague["playoffsThreshold"] = oldLeague["playoffsThreshold"]
            newLeague["relegationThreshold"] = oldLeague["relegationThreshold"]
            newLeague["supported"] = oldLeague["supported"]
            newLeague["linkedCompetitions"] = oldLeague["linkedCompetitions"]

with open("leagues.json", "w") as file:
    file.write(json.dumps(newLeagueDatabase, indent=4))