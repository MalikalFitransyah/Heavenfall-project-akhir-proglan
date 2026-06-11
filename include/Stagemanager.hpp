#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Enemy.hpp"
#include "Item.hpp"

enum class StageState { Playing, StageClear, AllClear };

struct StageData
{
    int   totalEnemies;
    float spawnIntervalStart;
    float spawnIntervalMin;
    int   enemyHp;           // HP musuh per stage
};

class StageManager
{
public:
    StageManager(sf::Texture* walkTex, sf::Texture* despawnTex,
                 sf::Texture* terroreyeTex, sf::Texture* maskTex,
                 sf::Texture* wingTex,      sf::Texture* spawnFxTex,
                 sf::Font& font, float mapW, float mapH);

    void update(float dt, sf::Vector2f playerCenter);
    void draw(sf::RenderWindow& window);

    std::vector<Enemy>& getEnemies();
    std::vector<Item>&  getItems();
    StageState          getState()        const;
    int                 getCurrentStage() const;

    void nextStage();
    void reset();

private:
    void spawnEnemy(sf::Vector2f playerCenter);
    void dropItem(sf::Vector2f position);
    void updateStageClearTimer(float dt);

    sf::Texture* mWalkTex;
    sf::Texture* mDespawnTex;
    sf::Texture* mTerroreyeTex;
    sf::Texture* mMaskTex;
    sf::Texture* mWingTex;
    sf::Texture* mSpawnFxTex;

    std::vector<Enemy>     mEnemies;
    std::vector<Item>      mItems;
    std::vector<StageData> mStages;

    int          mCurrentStage;
    int          mSpawnedCount;
    sf::Clock    mSpawnClock;
    float        mSpawnInterval;

    StageState   mState;
    sf::Clock    mClearClock;
    float        mClearDuration;

    float        mMapW;
    float        mMapH;

    sf::Text     mStageClearText;
    sf::Text     mStageNameText;
};