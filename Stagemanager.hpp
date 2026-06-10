#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.hpp"

enum class StageState
{
    Playing,
    StageClear,
    AllClear
};

struct StageData
{
    int totalEnemies;
};

class StageManager
{
public:
    StageManager(sf::Texture* walkTex, sf::Texture* despawnTex,
                 sf::Font& font, float mapW, float mapH);

    void update(float dt, sf::Vector2f playerCenter);
    void draw(sf::RenderWindow& window);

    std::vector<Enemy>& getEnemies();
    StageState          getState() const;
    int                 getCurrentStage() const;

    void nextStage();
    void reset();

private:
    void spawnEnemy(sf::Vector2f playerCenter);
    void updateStageClearTimer(float dt);

    sf::Texture* mWalkTex;
    sf::Texture* mDespawnTex;

    std::vector<Enemy>    mEnemies;
    std::vector<StageData> mStages;

    int          mCurrentStage;
    int          mSpawnedCount;
    int          mKilledCount;
    sf::Clock    mSpawnClock;
    float        mSpawnInterval;

    StageState   mState;
    sf::Clock    mClearClock;
    float        mClearDuration;

    float        mMapW;
    float        mMapH;

    // UI Stage Clear
    sf::Text     mStageClearText;
    sf::Text     mStageNameText;
};