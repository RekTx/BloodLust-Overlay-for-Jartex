#pragma once

extern class gridgraphic;

class Player {

private:

	std::string name;
	std::string guildTag;
	std::string serverRank;

	int level;
	int wins;
	int losses;
	int gamesPlayed;
	int finals;
	int beds;
	float fkdr;
	float wlr;
	int wsLifetime;

	bool isNicked;
	bool isHacker;

public:

	Player(std::string, std::string, std::string, std::string, bool);
	~Player();

	// main function ig
	void initProfile(std::string&, std::string&, std::string&, bool&);

	// gets data from api and stores it to local variables
	void initBw(std::string&, std::string&, bool&);
	void initSw(std::string&, std::string&, bool&);
	
	// stores default hard coded values in local variables
	void initTest(std::string&); // for testing purposes (no api calls)
	void initProfileTest(); // for testing purposes (no api calls)

	friend gridgraphic;
};