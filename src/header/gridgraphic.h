#pragma once
#include <string>
#include <vector>
#include <iostream>

#include "Player.h"

class gridgraphic {

private:

	// a player vector that stores all the player objects currently being displayed on the overlay screen
	std::vector<Player> players;
	// ign of user
	std::string userName;

public:

	gridgraphic();
	~gridgraphic();

	// functions to add a player from a string containing ign or a string array containing multiple igns
	void setPlayers(std::vector<std::string>&, std::string, std::string, std::string, bool);
	void addPlayer(std::string, std::string, std::string, std::string, bool);
	void removePlayer(std::string);

	// get the prominent player list from the txt file
	void getProminentPlayers(std::vector<std::string>&);
	bool isProminenetPlayer(std::string, std::vector<std::string>&);

	// get the last line from the log file, check if its a line containing players igns, if so then return it, else return empty
	std::string getPlayersLine(std::string);

	// main function
	void drawGrid(std::string, std::string, float&, bool&, bool&, bool&, std::string&, std::string&, bool&, bool&, std::string&, std::string&);
};