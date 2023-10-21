#include <Windows.h>
#include <Lmcons.h>
#include <locale>
#include <codecvt>
#include <fstream>

#include "header/gridgraphic.h"

using namespace std;

// get username of the device to get the exact path of log files
string getUserName() {

	TCHAR username[UNLEN + 1];
	DWORD usernameLen = UNLEN + 1;
	GetUserName((TCHAR*)username, &usernameLen);
	std::wstring usernameWstr(username);
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(usernameWstr);
}

// getting values from the config file
string getPath(string& gamemode, float& opacity, bool& topMost, bool& topBar, bool& autoClose, string& stats, string& interval, bool& fixedStats, bool& selfStats, string& userName, string& sortby) {

	string path, type, origBackupLine;
	ifstream fin;
	bool pathFound = false, gmFound = false, opacFound = false, topFound = false, 
		topBarFound = false, autoCloseFound = false, statsFound = false, intervalFound = false,
		fixedStatsFound = false, selfStatsFound = false, userNameFound = false, sortFound = false;

	try {

		fin.open("config/option.txt");
		if (!fin.is_open())
			throw string("File not found");


		while (!fin.eof()) {

			getline(fin, type);
			origBackupLine = type;

			for (int i = 0; i < type.length(); i++)
				type[i] = tolower(type[i]);

			if (type.find("client: ") != string::npos) {

				pathFound = true;

				if (type.substr(type.find(": ") + 2) == "lunar")
					path = "C:\\Users\\" + getUserName() + "\\.lunarclient\\offline\\multiver\\logs\\latest.log";
				else if (type.substr(type.find(": ") + 2) == "tlauncher")
					path = "C:\\Users\\" + getUserName() + "\\AppData\\Roaming\\.minecraft\\logs\\latest.log";
				else if (type.substr(type.find(": ") + 2) == "badlion")
					path = "C:\\Users\\" + getUserName() + "\\AppData\\Roaming\\.minecraft\\logs\\blclient\\minecraft\\latest.log";
				else if (type.substr(type.find(": ") + 2) == "salwyrr")
					path = "C:\\Users\\" + getUserName() + "\\AppData\\Roaming\\.Salwyrr\\logs\\latest.log";
				else if (type.substr(type.find(": ") + 2) == "cmclient" || type.substr(type.find(": ") + 2) == "cm")
					path = "C:\\Users\\" + getUserName() + "\\.cmclient\\logs\\latest.log";
				else
					path = "C:\\Users\\" + getUserName() + "\\AppData\\Roaming\\.minecraft\\logs\\latest.log";

			}
			else if (type.find("ign: ") != string::npos) {

				userNameFound = true;
				userName = origBackupLine.substr(type.find(": ") + 2);

			}
			else if (type.find("gamemode: ") != string::npos) {

				gmFound = true;

				gamemode = type.substr(type.find(": ") + 2);
				
				if (gamemode == "bedwars" or gamemode == "bw")
					gamemode = "bw";
				else if (gamemode == "skywars" or gamemode == "sw")
					gamemode = "sw";
				else
					gamemode = "bw";

			}
			else if (type.find("selfstats: ") != string::npos) {

				selfStatsFound = true;

				if (type.substr(type.find(": ") + 2) == "true")
					selfStats = true;
				else if (type.substr(type.find(": ") + 2) == "false")
					selfStats = false;
				else
					selfStats = true;

			}
			else if (type.find("sort by: ") != string::npos) {

				sortFound = true;

				sortby = type.substr(type.find(": ") + 2);

				if (sortby == "wins" || sortby == "win")
					sortby = "bywin";
				else if (sortby == "levels" || sortby == "level")
					sortby = "bylevel";
				else if (sortby == "wlr" || sortby == "winlossratio")
					sortby = "bywlr";
				else if (sortby == "fkdr" "" || sortby == "finals")
					sortby = "byfkdr";
				else if (sortby == "winstreak" || sortby == "ws")
					sortby = "byws";
				else if (sortby == "random" || sortby == "r")
					sortby = "randomly";
				else
					sortby = "bylevel";

			}
			else if (type.find("fixedstats: ") != string::npos) {

				fixedStatsFound = true;

				if (type.substr(type.find(": ") + 2) == "true")
					fixedStats = true;
				else if (type.substr(type.find(": ") + 2) == "false")
					fixedStats = false;
				else
					fixedStats = true;

			}
			else if (type.find("opacity: ") != string::npos) {

				opacFound = true;

				try {
					opacity = stoi(type.substr(type.find(": ") + 2));
					opacity /= 100;
				}
				catch (...) {
					opacity = 0.8f;
				}
			}
			else if (type.find("topmost: ") != string::npos) {

				topFound = true;

				if (type.substr(type.find(": ") + 2) == "true")
					topMost = true;
				else if (type.substr(type.find(": ") + 2) == "false")
					topMost = false;
				else
					topMost = true;

			}
			else if (type.find("topbar: ") != string::npos) {

				topBarFound = true;

				if (type.substr(type.find(": ") + 2) == "true")
					topBar = true;
				else if (type.substr(type.find(": ") + 2) == "false")
					topBar = false;
				else
					topBar = true;

			}
			else if (type.find("autoclose: ") != string::npos) {

				autoCloseFound = true;

				if (type.substr(type.find(": ") + 2) == "true")
					autoClose = true;
				else if (type.substr(type.find(": ") + 2) == "false")
					autoClose = false;
				else
					autoClose = false;

			}
			else if (type.find("stats: ") != string::npos) {

				statsFound = true;

				if (type.substr(type.find(": ") + 2) == "solo" or
					type.substr(type.find(": ") + 2) == "1" or
					type.substr(type.find(": ") + 2) == "solos") {
					stats = "SOLO";
				}
				else if (type.substr(type.find(": ") + 2) == "duo" or
					type.substr(type.find(": ") + 2) == "2" or
					type.substr(type.find(": ") + 2) == "dous" or
					type.substr(type.find(": ") + 2) == "doubles" or
					type.substr(type.find(": ") + 2) == "double") {
					stats = "DOUBLES";
				}
				else if (type.substr(type.find(": ") + 2) == "trio" or
					type.substr(type.find(": ") + 2) == "3" or
					type.substr(type.find(": ") + 2) == "trios" or
					type.substr(type.find(": ") + 2) == "triples" or
					type.substr(type.find(": ") + 2) == "triple") {
					stats = "TRIPLES";
				}
				else if (type.substr(type.find(": ") + 2) == "quad" or
					type.substr(type.find(": ") + 2) == "4" or
					type.substr(type.find(": ") + 2) == "quads" or
					type.substr(type.find(": ") + 2) == "four" or
					type.substr(type.find(": ") + 2) == "fours") {
					stats = "QUAD";
				}
				else if (type.substr(type.find(": ") + 2) == "all" or
					type.substr(type.find(": ") + 2) == "0" or
					type.substr(type.find(": ") + 2) == "overall" or
					type.substr(type.find(": ") + 2) == "lifetime" or
					type.substr(type.find(": ") + 2) == "allmode") {
					stats = "ALL_MODES";
				}
				else
					stats = "ALL_MODES";

			}
			else if (type.find("interval: ") != string::npos) {

				intervalFound = true;

				if (type.substr(type.find(": ") + 2) == "week" or
					type.substr(type.find(": ") + 2) == "weekly") {
					interval = "weekly";
				}
				else if (type.substr(type.find(": ") + 2) == "month" or
					type.substr(type.find(": ") + 2) == "monthly") {
					interval = "monthly";
				}
				else if (type.substr(type.find(": ") + 2) == "year" or
					type.substr(type.find(": ") + 2) == "yearly") {
					interval = "yearly";
				}
				else if (type.substr(type.find(": ") + 2) == "all" or
					type.substr(type.find(": ") + 2) == "alltime" or
					type.substr(type.find(": ") + 2) == "lifetime" or
					type.substr(type.find(": ") + 2) == "total") {
					interval = "total";
				}
				else
					interval = "total";

			}
			
		}

		if (!userNameFound)
			userName = "NAN";

		if (!pathFound)
			path = "C:\\Users\\" + getUserName() + "\\AppData\\Roaming\\.minecraft\\logs\\latest.log";

		if (!gmFound)
			gamemode = "bw";

		if (!opacFound)
			opacity = 0.8f;

		if (!topFound)
			topMost = true;

		if(!topBarFound)
			topBar = true;

		if (!autoCloseFound)
			autoClose = false;

		if (!statsFound)
			stats = "ALL_MODES";

		if (!intervalFound)
			interval = "total";

		if (!sortFound)
			sortby = "byLevel";

		if (!fixedStatsFound)
			fixedStats = true;

		if (!selfStatsFound)
			selfStats = true;

	}
	catch (string e) {

		if (e == "File not found")
			return "exit";

	}

	fin.close();

	return path;
}

int main() {

// the line below is for release mode, the int main is for debug mode
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	float opacity;
	string type, path, stats, interval, userName, sortby;
	bool topMost, topBar, autoClose, fixedStats, selfStats;

	path = getPath(type, opacity, topMost, topBar, autoClose, stats, interval, fixedStats, selfStats, userName, sortby);
	if (path == "exit")
		return 1;

	gridgraphic grid;
	// main function for overlay
	grid.drawGrid(path, type, opacity, topMost, topBar, autoClose, stats, interval, fixedStats, selfStats, userName, sortby);

	return 0;
}