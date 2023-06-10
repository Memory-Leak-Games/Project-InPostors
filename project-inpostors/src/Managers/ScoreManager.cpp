#include "Managers/ScoreManager.h"

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

void ScoreManager::AddScore(int scoreToAdd) {
    score += scoreToAdd;
    OnScoreChanged(score);
}

int ScoreManager::GetScore() const {
    return score;
}

std::set<ScoreManager::ScoreBoardEntry> ScoreManager::GetScoreBoard(
        const std::string& levelName) {
    
    json scoreboardJson;

    if (exists(std::filesystem::path(SCOREBOARD_PATH))) {
        std::ifstream file{SCOREBOARD_PATH};
        scoreboardJson = json::parse(file);
    } else {
        scoreboardJson = json::object();
    }

    std::set<ScoreBoardEntry> scoreboard;

    if (!scoreboardJson.contains(levelName)) {
        return scoreboard;
    }

    for (auto& entry : scoreboardJson[levelName]) {
        scoreboard.insert({entry["name"], entry["score"]});
    }

    return scoreboard;
}

#include "iostream"

void ScoreManager::SaveScore(const std::string& levelName, const std::string& playerName, int score) {
    json scoreboardJson;

    if (exists(std::filesystem::path(SCOREBOARD_PATH))) {
        std::ifstream file{SCOREBOARD_PATH};
        scoreboardJson = json::parse(file);
    } else {
        scoreboardJson = json::object();
    }

    if (!scoreboardJson.contains(levelName)) {
        scoreboardJson[levelName] = json::array();
    }

    scoreboardJson[levelName].push_back(
            {{"name", playerName},
             {"score", score}});

    std::ofstream out{SCOREBOARD_PATH};
    out << scoreboardJson.dump(4) << "\n";
}
