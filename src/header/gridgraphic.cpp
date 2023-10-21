#include <sstream>
#include <iomanip>
#include <fstream>
#include <regex>

#include "gridgraphic.h"
#include "raylib.h"

using namespace std;

/*
basicallu just need to add player to players vector, it will display all the players in that vector on overlay itself
*/

// constructor and destructor
gridgraphic::gridgraphic() {
}

gridgraphic::~gridgraphic() {
}

// add string array containing igns to players vector
void gridgraphic::setPlayers(vector<string>& names, string type, string stats, string interval, bool fixedStats) {

	for (int i = 0; i < names.size(); i++) {
		Player temp(names[i], type, stats, interval, fixedStats);
		players.push_back(temp);
	}

}

// add a single ign of player to players
void gridgraphic::addPlayer(string name, string type, string stats, string interval, bool fixedStats) {

	Player temp(name, type, stats, interval, fixedStats);
	players.push_back(temp);

}

// remove a player from the players list using his ign
void gridgraphic::removePlayer(string name) {
	for (int i = 0; i < players.size(); i++) {
		if (players[i].name == name) {
			players.erase(players.begin() + i);
			break;
		}
	}
}

// read data from prominent_players.txt file and push the usernames to promList vector
void gridgraphic::getProminentPlayers(std::vector<std::string>& promList) {

	ifstream fin;
	fin.open("config/prominent_player.txt", ios::app);

	string player;

	if (!fin.is_open())
		return;

	promList.clear();

	while (!fin.eof()) {

		fin >> player;

		if (player != "")
			promList.push_back(player);
	}

}

// check if the ign is in prominent players list
bool gridgraphic::isProminenetPlayer(string targetPlayer, vector<string>& players) {

	for (string& player : players) {
		if (player == targetPlayer)
			return true;
	}

	return false;
}

string gridgraphic::getPlayersLine(string path) {

	string line, temp;
	ifstream file(path);

	while (!file.eof()) {
		getline(file, temp);

		if (temp != "") {
			line = temp;
		}

	}

	file.close();

	if (line.find("[Client thread/INFO]: [CHAT] BedWars") != string::npos)
		return line.substr(line.find("[CHAT]"));
	if (line.find("[Client thread/INFO]: [CHAT] SkyWars") != string::npos)
		return line.substr(line.find("[CHAT]"));
	if (line.find("[Client thread/INFO]: [CHAT]") != string::npos)
		return line.substr(line.find("CHAT]") + 6);

	return "";
}

/*

the above code skips lines if multiple lines are added at once, so if a party joins, it only adds the last one on the chat to overlay.
I was working on it below code is related to that. But as i dont have time, i wont be finishing it. So you can ignore the commented code.

*/

//int gridgraphic::count_lines(string filename) {
//
//	ifstream file(filename);
//	int count = 0;
//	string line;
//	while (getline(file, line)) {
//		count++;
//	}
//	file.close();
//	return count;
//}
//
//vector<string> gridgraphic::get_new_lines(string filename, int old_count) {
//
//	vector<string> new_lines;
//	ifstream file(filename);
//	int current_count = 0;
//	string line;
//	while (getline(file, line)) {
//		current_count++;
//		if (current_count > old_count) {
//			new_lines.push_back(line);
//		}
//	}
//	file.close();
//	return new_lines;
//}
//
//vector<string> gridgraphic::detect_change(string filename) {
//	static int previous_count = count_lines(filename);
//	int current_count = count_lines(filename);
//
//	vector<string> new_lines;
//
//	if (current_count > previous_count) {
//		//cout << "The file was changed. The new lines are:\n";
//		new_lines = get_new_lines(filename, previous_count);
//
//		previous_count = current_count;
//	}
//	else
//		new_lines.push_back("null");
//
//	return new_lines;
//}

void gridgraphic::drawGrid(string path, string type, float& opacity, bool& topMost, bool& topBar, bool& autoClose, string& stats, string& interval, bool& fixedStats, bool& selfStats, string& userName, string& sortby) {

	// creating the window
	
	float x = 0.0f, y = 0.0f;
	int amTitle = 10;

	int yFac = 30;

	if (type == "bw")
		amTitle = 10;
	else
		amTitle = 8;

	int screenWidth = (amTitle * 100) - 60;
	float origWidth = screenWidth;
	int screenHeight = 2 * yFac + (players.size() * yFac);

	// setting the raylib window
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	if (!topBar)
		SetConfigFlags(FLAG_WINDOW_UNDECORATED);

	InitWindow(screenWidth, screenHeight, "BloodLust Overlay");

	SetExitKey(KEY_NULL);
	SetTargetFPS(30);
	SetWindowOpacity(opacity);
	if (topMost)
		SetWindowState(FLAG_WINDOW_TOPMOST);

	int fontSize = 16;
	int playerCount = 0;
	Font arialFont = LoadFontEx("C:\\Windows\\Fonts\\arialbd.ttf", fontSize, 0, 0);
	Font arialRoundFont = LoadFontEx("C:\\Windows\\Fonts\\calibrib.ttf", fontSize, 0, 0);

	bool shouldResize = false;
	bool shouldEdit = true;
	bool doAddPlayers = false;
	bool isMinimized = false;
	bool hotkeyIsPressed = false;

	string lastLine, oldLine = "r";

	vector<string> wordsTemp;
	vector<string> prominentPlayers;
	int indexT = 0;

	int offsetX = 0;
	int offsetY = 0;
	bool dragging = false;

	system("cls");
	while (!WindowShouldClose()) {

		// hotkey to hide/show the overlay, TODO: make it customizeable later
		
		if (IsKeyDown(KEY_LEFT_ALT) and IsKeyDown(KEY_G)) {

			hotkeyIsPressed = true;

		}

		if (IsKeyUp(KEY_LEFT_ALT) and IsKeyUp(KEY_G) and hotkeyIsPressed) {

			hotkeyIsPressed = false;

			if (isMinimized) {
				isMinimized = false;
				shouldResize = true;
			}
			else {
				isMinimized = true;
				shouldResize = true;
			}

		}

		// update
		
		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();

		float scaleX = (float)screenWidth / origWidth;
		float scaleY = (float)screenHeight / 60.0f;

		// enabling draging from every where
		// make it so it moves the window when mouse is pressed and dragged on the window anywhere

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			// Check if the mouse is clicked within the window bounds
			if (CheckCollisionPointRec(GetMousePosition(), { 0, 0, (float)screenWidth, (float)screenHeight }))
			{
				dragging = true;
				offsetX = GetMouseX() - GetWindowPosition().x;
				offsetY = GetMouseY() - GetWindowPosition().y;
			}
		}
		else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
		{
			dragging = false;
		}

		// Move the window while dragging
		if (dragging)
		{
			SetWindowPosition(GetMouseX() - offsetX, GetMouseY() - offsetY);
		}

		// done

		// getting lines and checking if there are igns in them along with adding it to the vector
		lastLine = getPlayersLine(path);

		// minimize the overlay window when game is about to start, optional
		if (lastLine.find("The game will start in 1 second.") != string::npos and lastLine.find("[CHAT]") != string::npos and autoClose) {

			if (!IsWindowMinimized()) {
				MinimizeWindow();
			}

			lastLine = "";
		}

		// if lastline is not a line from bedwars or skywars chat, it will be ""
		if (lastLine != "" and lastLine != oldLine) {

			// when u press space and tab in chat, it gives a line containing all usernames in tab seperated by commas.
			// so creating a pattern of combination of alphabets, numbers and underscores ending with commas.
			regex pattern("[a-zA-Z0-9_]+(, )");
			smatch match;

			// checking if the pattern is matching
			if (regex_search(lastLine, match, pattern)) {

				string temp = lastLine;
				wordsTemp.clear();

				// putting the ign on overlay if selfstats are on
				if (userName != "NAN" and selfStats)
					wordsTemp.push_back(userName);
				
				// seprating the igns as words instead of a single line and putting them in wordsTemp vector
				string word = "";
				for (int i = 0; i < temp.length() + 1; i++) {
					if (temp[i] == ' ' and temp[i - 1] == ',')
						continue;
					// this is to fix a bug, dont mind
					else if (temp[i] == ' ') {
						goto end;
					}
					else if (temp[i] == ',' or temp[i] == '\0') {
						wordsTemp.push_back(word);
						word = "";
					}
					else
						word += temp[i];
				}

				shouldResize = true;
				doAddPlayers = true;

			}
			// add or remove players to the overlay if they join the game after you
			else if (lastLine.find("[CHAT] BedWars") != string::npos) {
				
				// joining message
				auto joinPos = lastLine.find("has joined!");
				auto leavePos = lastLine.find("has quit!");

				if (joinPos != string::npos || leavePos != string::npos) {

					string temp = lastLine.substr(lastLine.find("BedWars") + 10);

					string word = "";
					for (int i = 0; i < temp.length() + 1; i++) {
						if (temp[i] == ' ') {
							break;
						}
						else
							word += temp[i];
					}

					if (joinPos != string::npos)
						addPlayer(word, type, stats, interval, fixedStats);
					else if (leavePos != string::npos) {
						removePlayer(word);

						for (int i = 0; i < wordsTemp.size(); i++) {
							if (wordsTemp[i] == word) {
								wordsTemp.erase(wordsTemp.begin() + i);
								break;
							}
						}

					}

					shouldResize = true;
				}
				else if (lastLine.find("[CHAT] SkyWars") != string::npos) {

					// joining message
					auto joinPos = lastLine.find("has joined!");
					auto leavePos = lastLine.find("has quit!");

					if (joinPos != string::npos || leavePos != string::npos) {

						string temp = lastLine.substr(lastLine.find("SkyWars") + 10);

						string word = "";
						for (int i = 0; i < temp.length() + 1; i++) {
							if (temp[i] == ' ') {
								break;
							}
							else
								word += temp[i];
						}

						if (joinPos != string::npos)
							addPlayer(word, type, stats, interval, fixedStats);
						else if (leavePos != string::npos) {
							removePlayer(word);

							for (int i = 0; i < wordsTemp.size(); i++) {
								if (wordsTemp[i] == word) {
									wordsTemp.erase(wordsTemp.begin() + i);
									break;
								}
							}

						}

						shouldResize = true;

					}

				}

			}
		}

		oldLine = lastLine;

		//

		// adding players to the prominentList from file every -- seconds
		getProminentPlayers(prominentPlayers);


	end:

		// adding players to the vector
		// doesnt add all player at once, adds them one name per hydration so the overlay remains smooth

		if (doAddPlayers) {

			if (indexT < wordsTemp.size()) {

				if (indexT == 0)
					players.clear();

				if (wordsTemp[indexT] == userName and selfStats) {
					addPlayer(wordsTemp[indexT], type, stats, interval, fixedStats);
					shouldResize = true;
					indexT++;
				}
				else if (wordsTemp[indexT] != userName) {
					addPlayer(wordsTemp[indexT], type, stats, interval, fixedStats);
					shouldResize = true;
					indexT++;
				}
				else
					indexT++;

			}
			else {
				doAddPlayers = false;
				indexT = 0;
			}

		}

		playerCount = players.size();

		if (shouldResize) {

			if (isMinimized)
				screenHeight = 1 * yFac + (0 * yFac);
			else
				screenHeight = 2 * yFac + (players.size() * yFac);
			shouldResize = false;

			SetWindowSize(screenWidth, screenHeight);
		}

		// drawing
		
		// all the drawing stuff using raylib
		if (shouldEdit) {

			BeginDrawing();
			ClearBackground(BLACK);

			// sorting players

			if (sortby == "bylevel") {

			// sort by level
				sort(players.begin(), players.end(), [](const Player& lhs, const Player& rhs) {
					return lhs.level > rhs.level;
					});

			}
			else if (sortby == "bywin") {

			// sort by wins
				sort(players.begin(), players.end(), [](const Player& lhs, const Player& rhs) {
					return lhs.wins > rhs.wins;
					});

			}
			else if (sortby == "bywlr") {

			// sort by wlr
				sort(players.begin(), players.end(), [](const Player& lhs, const Player& rhs) {
					return lhs.wlr > rhs.wlr;
					});

			}
			else if (sortby == "byfkdr") {

			// sort by fkdr
				sort(players.begin(), players.end(), [](const Player& lhs, const Player& rhs) {
					return lhs.fkdr > rhs.fkdr;
					});

			}
			else if (sortby == "byws") {

			// sort by ws
				sort(players.begin(), players.end(), [](const Player& lhs, const Player& rhs) {
					return lhs.wsLifetime > rhs.wsLifetime;
					});

			}

			Color tempColor;
			Color borderColor;
			bool hasBorder = false;
			x = 0; y = 10;

			string playerCountStr = "Player Count: " + to_string(players.size());
			DrawTextEx(arialFont, playerCountStr.c_str(), { x + ((screenWidth / 2) - 100) , y }, fontSize* scaleX, 2, WHITE);

			if (!isMinimized) {

				if (type == "bw") {

					y += yFac - 5;
					DrawTextEx(arialFont, "   Level", { x,y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Rank", { x + (100 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Name", { x + (200 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					x = 100;
					DrawTextEx(arialFont, "Wins", { x + (300 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Losses", { x + (370 * scaleX), y }, fontSize* scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Wlr", { x + (450 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Finals", { x + (500 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Beds", { x + (570 * scaleX), y }, fontSize* scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Ws (All)", { x + (630 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Clan", { x + (730 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					y += yFac;

					// draw players


					for (auto player : players) {

						bool isProminent = isProminenetPlayer(player.name, prominentPlayers);
						hasBorder = false;

						if (player.isHacker || isProminent || player.isNicked) {

							// make a rectangle around the player row with red color
							hasBorder = true;
							borderColor = BLACK;
							DrawRectangle(x - 100, y - 5, screenWidth, yFac - 5, WHITE);

						}

						x = 0;

						if (player.level < 5)
							tempColor = WHITE;
						else if (player.level < 10)
							tempColor = GREEN;
						else if (player.level < 15)
							tempColor = { 0, 255, 255, 255 };
						else if (player.level < 20)
							tempColor = PINK;
						else if (player.level < 25)
							tempColor = ORANGE;
						else if (player.level < 30)
							tempColor = YELLOW;
						else if (player.level < 35)
							tempColor = RED;
						else if (player.level < 40) {
							tempColor = WHITE;
						}
						else if (player.level < 45) {
							tempColor = GREEN;
						}
						else if (player.level < 50) {
							tempColor = { 0, 255, 255, 255 };
						}
						else if (player.level < 60) {
							tempColor = PINK;
						}
						else if (player.level < 75) {
							tempColor = ORANGE;
						}
						else if (player.level < 100) {
							tempColor = YELLOW;
						}
						else {
							tempColor = RED;
						}

						if (player.isNicked || player.isHacker) {

							string tempTTT;
							if (player.isNicked)
								tempTTT = " Nick";
							else
								tempTTT = " Hacker";

							if (hasBorder) {

								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x - 1,y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x - 1,y + 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x + 1,y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x + 1,y + 1 }, fontSize* scaleX, 2, borderColor);


							}

							DrawTextEx(arialRoundFont, tempTTT.c_str(), { x,y }, fontSize* scaleX, 2, MAROON);
						}
						else {

							if (hasBorder) {
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x - 1,y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x - 1,y + 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x + 1,y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x + 1,y + 1 }, fontSize* scaleX, 2, borderColor);
							}

							DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x,y }, fontSize* scaleX, 2, tempColor);
						}

						if (player.serverRank == "Crystal")
							tempColor = PINK;
						else if (player.serverRank == "Diamond")
							tempColor = { 0, 255, 255, 255 };
						else if (player.serverRank == "Iron")
							tempColor = WHITE;
						else if (player.serverRank == "Gold")
							tempColor = GOLD;
						else
							tempColor = GRAY;

						if (player.isHacker or player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (100 * scaleX), y }, fontSize * scaleX, 2, RED);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x - 1 + (100 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x - 1 + (100 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 1 + (100 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 1 + (100 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + (100 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						}

						if (hasBorder) {

							DrawTextEx(arialRoundFont, player.name.c_str(), { x - 1 + (200 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x - 1 + (200 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 1 + (200 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 1 + (200 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

						}

						DrawTextEx(arialRoundFont, player.name.c_str(), { x + (200 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						x = 100;

						if (player.wins < 1000)
							tempColor = GRAY;
						else if (player.wins < 2000)
							tempColor = WHITE;
						else if (player.wins < 5000)
							tempColor = YELLOW;
						else if (player.wins < 7000)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (300 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x - 1 + (300 * scaleX), y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x - 1 + (300 * scaleX), y + 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 1 + (300 * scaleX), y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 1 + (300 * scaleX), y + 1 }, fontSize* scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + (300 * scaleX), y }, fontSize* scaleX, 2, tempColor);
						}

						// losses

						if (player.losses < 1000)
							tempColor = GRAY;
						else if (player.losses < 2000)
							tempColor = WHITE;
						else if (player.losses < 3000)
							tempColor = YELLOW;
						else if (player.losses < 5000)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (370 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.losses).c_str(), { x - 1 + (370 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.losses).c_str(), { x - 1 + (370 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.losses).c_str(), { x + 1 + (370 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.losses).c_str(), { x + 1 + (370 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.losses).c_str(), { x + (370 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						}

						//

						if (player.wlr < 2)
							tempColor = GRAY;
						else if (player.wlr < 5)
							tempColor = WHITE;
						else if (player.wlr < 7)
							tempColor = YELLOW;
						else if (player.wlr < 15)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						ostringstream ss;
						ss << fixed << setprecision(1) << player.wlr;
						string fkdrString = ss.str();

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (450 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x - 1 + (450 * scaleX), y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x - 1 + (450 * scaleX), y + 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 1 + (450 * scaleX), y - 1 }, fontSize* scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 1 + (450 * scaleX), y + 1 }, fontSize* scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + (450 * scaleX), y }, fontSize* scaleX, 2, tempColor);
						}

						if (player.finals < 2000)
							tempColor = GRAY;
						else if (player.finals < 5000)
							tempColor = WHITE;
						else if (player.finals < 10000)
							tempColor = YELLOW;
						else if (player.finals < 15000)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (500 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.finals).c_str(), {x - 1 + (500 * scaleX), y - 1}, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.finals).c_str(), { x - 1 + (500 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.finals).c_str(), { x + 1 + (500 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.finals).c_str(), { x + 1 + (500 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.finals).c_str(), { x + (500 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						}

						// beds
						if (player.beds < 1000)
							tempColor = GRAY;
						else if (player.beds < 2500)
							tempColor = WHITE;
						else if (player.beds < 5000)
							tempColor = YELLOW;
						else if (player.beds < 10000)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (570 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.beds).c_str(), { x - 1 + (570 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.beds).c_str(), { x - 1 + (570 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.beds).c_str(), { x + 1 + (570 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.beds).c_str(), { x + 1 + (570 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.beds).c_str(), { x + (570 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						}

						//


						if (player.wsLifetime < 100)
							tempColor = GRAY;
						else if (player.wsLifetime < 150)
							tempColor = WHITE;
						else if (player.wsLifetime < 300)
							tempColor = YELLOW;
						else if (player.wsLifetime < 700)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + (630 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x - 1 + (630 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x - 1 + (630 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 1 + (630 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 1 + (630 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + (630 * scaleX), y }, fontSize * scaleX, 2, tempColor);
						}

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("-").c_str(), { x + (730 * scaleX), y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x - 1 + (730 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x - 1 + (730 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 1 + (730 * scaleX), y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 1 + (730 * scaleX), y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + (730 * scaleX), y }, fontSize * scaleX, 2, { 0, 255, 255, 255 });
						}

						y += yFac;
					}

				}
				else if (type == "sw") {

					y += yFac - 5;
					DrawTextEx(arialFont, "   Level", { x,y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Rank", { x + (100 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Name", { x + (200 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					x = 100;
					DrawTextEx(arialFont, "Wins", { x + (300 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Wlr", { x + (350 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "KDR", { x + (400 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Ws (All)", { x + (470 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					DrawTextEx(arialFont, "Clan", { x + (550 * scaleX), y }, fontSize * scaleX, 2, WHITE);
					y += yFac;

					for (auto player : players) {

						bool isProminent = isProminenetPlayer(player.name, prominentPlayers);
						hasBorder = false;

						if (player.isHacker || isProminent || player.isNicked) {

							// make a rectangle around the player row with red color
							hasBorder = true;
							borderColor = BLACK;
							DrawRectangle(x - 100, y - 5, screenWidth, yFac - 5, WHITE);

						}

						x = 0;

						if (player.level < 5)
							tempColor = WHITE;
						else if (player.level < 10)
							tempColor = GREEN;
						else if (player.level < 15)
							tempColor = { 0, 255, 255, 255 };
						else if (player.level < 20)
							tempColor = PINK;
						else if (player.level < 25)
							tempColor = ORANGE;
						else if (player.level < 30)
							tempColor = YELLOW;
						else if (player.level < 35)
							tempColor = MAROON;
						else if (player.level < 40) {
							tempColor = WHITE;
						}
						else if (player.level < 45) {
							tempColor = GREEN;
						}
						else if (player.level < 50) {
							tempColor = { 0, 255, 255, 255 };
						}
						else if (player.level < 60) {
							tempColor = PINK;
						}
						else if (player.level < 75) {
							tempColor = ORANGE;
						}
						else if (player.level < 100) {
							tempColor = YELLOW;
						}
						else {
							tempColor = MAROON;
						}

						if (player.isNicked || player.isHacker) {

							string tempTTT;
							if (player.isNicked)
								tempTTT = " Nick";
							else
								tempTTT = " Hacker";

							if (hasBorder) {

								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x - 1,y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x - 1,y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x + 1,y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, tempTTT.c_str(), { x + 1,y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, tempTTT.c_str(), { x,y }, fontSize * scaleX, 2, MAROON);
						}
						else {

							if (hasBorder) {
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x - 1,y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x - 1,y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x + 1,y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x + 1,y + 1 }, fontSize * scaleX, 2, borderColor);
							}

							DrawTextEx(arialRoundFont, string("   " + to_string(player.level)).c_str(), { x,y }, fontSize * scaleX, 2, tempColor);
						}

						if (player.serverRank == "Crystal")
							tempColor = PINK;
						else if (player.serverRank == "Diamond")
							tempColor = { 0, 255, 255, 255 };
						else if (player.serverRank == "Iron")
							tempColor = WHITE;
						else if (player.serverRank == "Gold")
							tempColor = GOLD;
						else
							tempColor = GRAY;

						if (player.isNicked or player.isHacker)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 100, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 100 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 100 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 100 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 100 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, player.serverRank.c_str(), { x + 100, y }, fontSize * scaleX, 2, tempColor);
						}

						if (hasBorder) {

							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 200 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 200 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 200 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
							DrawTextEx(arialRoundFont, player.name.c_str(), { x + 200 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

						}

						DrawTextEx(arialRoundFont, player.name.c_str(), { x + 200, y }, fontSize * scaleX, 2, tempColor);
						x = 100;

						if (player.wins < 1000)
							tempColor = GRAY;
						else if (player.wins < 2000)
							tempColor = WHITE;
						else if (player.wins < 3000)
							tempColor = YELLOW;
						else if (player.wins < 5000)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 300, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 300 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 300 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 300 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 300 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.wins).c_str(), { x + 300, y }, fontSize * scaleX, 2, tempColor);
						}

						if (player.wlr < 2)
							tempColor = GRAY;
						else if (player.wlr < 5)
							tempColor = WHITE;
						else if (player.wlr < 7)
							tempColor = YELLOW;
						else if (player.wlr < 15)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						ostringstream ss;
						ss << fixed << setprecision(1) << player.wlr;
						string fkdrString = ss.str();

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 350, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 350 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 350 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 350 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 350 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, fkdrString.c_str(), { x + 350, y }, fontSize * scaleX, 2, tempColor);
						}

						if (player.fkdr < 3)
							tempColor = GRAY;
						else if (player.fkdr < 6)
							tempColor = WHITE;
						else if (player.fkdr < 15)
							tempColor = YELLOW;
						else if (player.fkdr < 25)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						ostringstream ss1;
						ss1 << fixed << setprecision(1) << player.fkdr;
						string wlrString = ss1.str();

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 400, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, wlrString.c_str(), { x + 400 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, wlrString.c_str(), { x + 400 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, wlrString.c_str(), { x + 400 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, wlrString.c_str(), { x + 400 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, wlrString.c_str(), { x + 400, y }, fontSize* scaleX, 2, tempColor);
						}

						if (player.wsLifetime < 100)
							tempColor = GRAY;
						else if (player.wsLifetime < 150)
							tempColor = WHITE;
						else if (player.wsLifetime < 300)
							tempColor = YELLOW;
						else if (player.wsLifetime < 700)
							tempColor = ORANGE;
						else
							tempColor = MAROON;

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 470, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 470 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 470 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 470 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 470 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, to_string(player.wsLifetime).c_str(), { x + 470, y }, fontSize * scaleX, 2, tempColor);
						}

						if (player.isNicked)
							DrawTextEx(arialRoundFont, string("   -").c_str(), { x + 550, y }, fontSize * scaleX, 2, MAROON);
						else {

							if (hasBorder) {

								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 550 - 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 550 - 1, y + 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 550 + 1, y - 1 }, fontSize * scaleX, 2, borderColor);
								DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 550 + 1, y + 1 }, fontSize * scaleX, 2, borderColor);

							}

							DrawTextEx(arialRoundFont, player.guildTag.c_str(), { x + 550, y }, fontSize* scaleX, 2, { 0, 255, 255, 255 });
						}

						y += yFac;
					}

				}

			}

			EndDrawing();
		}

		if (IsKeyPressed(KEY_ESCAPE)) {
			// nothing
			// overriding the built in function so it doesnt close the overlay when u press escape
		}
	}

	UnloadFont(arialRoundFont);
	UnloadFont(arialFont);
}