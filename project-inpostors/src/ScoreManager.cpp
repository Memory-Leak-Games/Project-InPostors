#include "ScoreManager.h"

#include <fstream>

using json = nlohmann::json;

void ScoreManager::AddScore(int scoreToAdd) {
    score += scoreToAdd;
    OnScoreChanged(score);
}

int ScoreManager::GetScore() const {
    return score;
}

std::vector<ScoreManager::ScoreBoardEntry> ScoreManager::GetScoreBoard(
        const std::string& levelName) {

    std::ifstream file{SCOREBOARD_PATH};
    json scoreboardJson = json::parse(file);

    std::vector<ScoreBoardEntry> scoreboard;

    if (!scoreboardJson.contains(levelName)) {
        return scoreboard;
    }

    for (auto& entry : scoreboardJson[levelName]) {
        scoreboard.push_back({entry["name"], entry["score"]});
    }

    return scoreboard;
}

void ScoreManager::SaveScore(const std::string& levelName, const std::string& playerName, int score) {
    std::ifstream file{SCOREBOARD_PATH};
    json scoreboardJson = json::parse(file);

    if (!scoreboardJson.contains(levelName)) {
        scoreboardJson[levelName] = json::array();
    }

    scoreboardJson[levelName].push_back(
            {{"name", playerName},
             {"score", score}});

    std::ofstream out{SCOREBOARD_PATH};
    out << scoreboardJson.dump(4) << "\n";
}
