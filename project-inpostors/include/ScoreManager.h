#pragma once

#define SCOREBOARD_PATH RUNTIME_FILES_PATH "scoreboard.json"

class ScoreManager { 
private:
    int score = 0;
    
public:
    struct ScoreBoardEntry {
        std::string playerName;
        int score;
    };

    eventpp::CallbackList<void(int newScore)> OnScoreChanged;

    ScoreManager() = default;

    void AddScore(int scoreToAdd);
    [[nodiscard]] int GetScore() const;

    static std::vector<ScoreBoardEntry> GetScoreBoard(const std::string& levelName);
    static void SaveScore(const std::string& levelName, const std::string& playerName, int score);
};
