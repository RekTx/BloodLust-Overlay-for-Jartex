#include <string>
#include <codecvt>
#include <locale>

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include "json/json.h"

#include "Player.h"

using namespace std;

Player::Player(string ign, string type, string stats, string interval, bool fixedStats) {

    name = ign;
    isNicked = false;
    isHacker = false;
    initProfile(type, stats, interval, fixedStats);
}

Player::~Player() {
}

void Player::initProfile(string& type, string& stats, string& interval, bool& fixedStats) {

    try {

        if (name[0] == '.')
            isHacker = true;


        // making api request in short
        // https://stats.jartexnetwork.com/api/profile/zornk
        web::http::client::http_client client(U("https://stats.jartexnetwork.com"));
        // web::http::client::http_client client(U("https://stats.pika-network.net/"));
        string tempApiLink = "/api/profile/" + name;

        wstring_convert<codecvt_utf8<wchar_t>> converter;
        wstring wideTempApiLink = converter.from_bytes(tempApiLink);

        web::http::http_request request;
        request.set_request_uri(wideTempApiLink);

        auto response = client.request(request).get();

        // if profile doesnt exist, player is nicked, so we dont need to go any further
        if (response.status_code() == 400) {
            initTest(name);
            return;
        }

        // converting to a json style data
        string jsonStr = response.extract_utf8string().get();
        Json::Value json;
        Json::Reader reader;
        reader.parse(jsonStr, json);

        level = stoi(json["rank"]["level"].asString());

        try {
            guildTag = json["clan"]["name"].asString();
            if (guildTag == "")
                guildTag = "-";
        }
        catch (...) {
            guildTag = " ";
        }

        try {

            serverRank = "NON";

            // getting mingames rank specifically
            for (int i = 0; i < json["ranks"].size(); i++) {

                if (json["ranks"][i]["name"].asString() == "games1" or
                    json["ranks"][i]["name"].asString() == "games2" or
                    json["ranks"][i]["name"].asString() == "games3" or
                    json["ranks"][i]["name"].asString() == "games4") {
                    serverRank = json["ranks"][i]["displayName"].asString();
                    if (serverRank == "")
                        serverRank = "NON";
                    if (serverRank == "Titan")
                        continue;
                    break;
                }
            }
        }
        catch (...) {
            serverRank = "NON";
        }
    }
    catch (...) {
        // if there is any error above, give them the default/nick treatment, doesnt mean its a nick tho
        //  could be bcz api timeout
        initTest(name);
        return;
    }

    if (type == "bw")
        initBw(stats, interval, fixedStats);
    else if (type == "sw")
        initSw(stats, interval, fixedStats);
}


void Player::initBw(string& stats, string& interval, bool& fixedStats) {

    // https://stats.jartexnetwork.com/api/profile/zornk/leaderboard?type=bedwars&interval=total&mode=ALL_MODES
    web::http::client::http_client client(U("https://stats.jartexnetwork.com"));
    string tempApiLink = "/api/profile/" + name + "/leaderboard?type=bedwars&interval=" + interval + "&mode=" + stats;
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    wstring wideTempApiLink = converter.from_bytes(tempApiLink);

    web::http::http_request request;
    request.set_request_uri(wideTempApiLink);

    auto response = client.request(request).get();

    // converting it to a json style response
    string jsonStr = response.extract_utf8string().get();
    Json::Value json;
    Json::Reader reader;
    reader.parse(jsonStr, json);

    // initialize the player
    // putting into separate try catch block because if the player doesn't exist, the json will be empty

    try {
        wins = stoi(json["Wins"]["entries"][0]["value"].asString());
    }
    catch (...) {
        wins = 0;
    }

    try {
		losses = stoi(json["Losses"]["entries"][0]["value"].asString());
	}
    catch (...) {
		losses = 0;
	}

    try {
        gamesPlayed = stoi(json["Games played"]["entries"][0]["value"].asString());
    }
    catch (...) {
        gamesPlayed = 0;
    }

    try {
        finals = stoi(json["Final kills"]["entries"][0]["value"].asString());
    }
    catch (...) {
        finals = 0;
    }

    try {
        beds = stoi(json["Beds destroyed"]["entries"][0]["value"].asString());
    }
    catch (...) {
        beds = 0;
    }

    try {
        
        if (fixedStats) {
            if (gamesPlayed - wins <= 0)
                throw 1;

            wlr = (float)wins / (gamesPlayed - wins);
        }
        else {
            if (losses <= 0)
                throw 1;

            wlr = (float)wins / losses;
        }

    }
    catch (...) {
        wlr = wins;
    }

    try {
        wsLifetime = stoi(json["Highest winstreak reached"]["entries"][0]["value"].asString());
    }
    catch (...) {
        wsLifetime = 0;
    }

}

void Player::initSw(string& stats, string& interval, bool& fixedStats) {

    web::http::client::http_client client(U("https://stats.jartexnetwork.com"));
    string tempApiLink = "/api/profile/" + name + "/leaderboard?type=skywars&interval=" + interval + "&mode=" + stats;
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    wstring wideTempApiLink = converter.from_bytes(tempApiLink);

    web::http::http_request request;
    request.set_request_uri(wideTempApiLink);

    auto response = client.request(request).get();

    string jsonStr = response.extract_utf8string().get();
    Json::Value json;
    Json::Reader reader;
    reader.parse(jsonStr, json);

    // initialize the player
    // putting into separate try catch block because if the player doesn't exist, the json will be empty

    try {
        wins = stoi(json["Wins"]["entries"][0]["value"].asString());
    }
    catch (...) {
        wins = 0;
    }

    try {
		losses = stoi(json["Losses"]["entries"][0]["value"].asString());
	}
    catch (...) {
		losses = 0;
	}

    try {
        gamesPlayed = stoi(json["Games played"]["entries"][0]["value"].asString());
    }
    catch (...) {
        gamesPlayed = 0;
    }

    try {
        finals = stoi(json["Kills"]["entries"][0]["value"].asString());
    }
    catch (...) {
        finals = 0;
    }

    try {

        if (fixedStats) {
            if (gamesPlayed - wins <= 0)
                throw 1;

            fkdr = (float)finals / (gamesPlayed - wins);
        }
        else {
            if (losses <= 0)
				throw 1;

            fkdr = (float)finals / losses;
        }
        
    }
    catch (...) {
        fkdr = 0;
    }

    try {

        if (fixedStats) {
            if (gamesPlayed - wins <= 0)
                throw 1;

            wlr = (float)wins / (gamesPlayed - wins);
        }
        else {
			if (losses <= 0)
				throw 1;

			wlr = (float)wins / losses;
		}
    }
    catch (...) {
        wlr = 0;
    }

    try {
        wsLifetime = stoi(json["Highest winstreak reached"]["entries"][0]["value"].asString());
    }
    catch (...) {
        wsLifetime = 0;
    }

}

void Player::initTest(string& ign) {

    isNicked = true;

    wins = 0;
    gamesPlayed = 0;
    finals = 0;
    fkdr = 0;
    beds = 0;
    wlr = 0;
    wsLifetime = 0;
    initProfileTest();
}

void Player::initProfileTest() {
    level = 0;
    guildTag = " ";
    serverRank = "NON";
}