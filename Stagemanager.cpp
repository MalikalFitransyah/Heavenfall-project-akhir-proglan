#include "StageManager.hpp"
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>

StageManager::StageManager(sf::Texture* walkTex,    sf::Texture* despawnTex,
                           sf::Texture* terroreyeTex, sf::Texture* maskTex,
                           sf::Texture* wingTex,      sf::Texture* spawnFxTex,
                           sf::Font& font, float mapW, float mapH)
    : mWalkTex(walkTex), mDespawnTex(despawnTex)
    , mTerroreyeTex(terroreyeTex), mMaskTex(maskTex)
    , mWingTex(wingTex), mSpawnFxTex(spawnFxTex)
    , mCurrentStage(0), mSpawnedCount(0)
    , mSpawnInterval(2.0f)
    , mState(StageState::Playing)
    , mClearDuration(3.0f)
    , mMapW(mapW), mMapH(mapH)
    , mStageClearText(font, "STAGE CLEAR!", 100)
    , mStageNameText(font, "", 60)
{
    // totalEnemies, spawnStart, spawnMin, enemyHp
    mStages = {
        { 15, 2.0f, 1.2f, 1 },
        { 30, 1.8f, 0.9f, 2 },
        { 50, 1.5f, 0.6f, 3 },
        { 80, 1.2f, 0.3f, 4 },
    };

    mStageClearText.setFillColor(sf::Color::White);
    mStageClearText.setStyle(sf::Text::Bold);
    sf::FloatRect b = mStageClearText.getLocalBounds();
    mStageClearText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    mStageClearText.setPosition({mapW / 2.f, mapH / 2.f - 80.f});

    mStageNameText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect b2 = mStageNameText.getLocalBounds();
    mStageNameText.setOrigin({b2.size.x / 2.f, b2.size.y / 2.f});
    mStageNameText.setPosition({mapW / 2.f, mapH / 2.f + 40.f});
}

void StageManager::update(float dt, sf::Vector2f playerCenter)
{
    if (mState == StageState::StageClear)
    {
        updateStageClearTimer(dt);
        for (auto& item : mItems) item.update(dt);
        return;
    }
    if (mState == StageState::AllClear) return;

    int total = mStages[mCurrentStage].totalEnemies;

    if (mSpawnedCount < total)
    {
        if (mSpawnClock.getElapsedTime().asSeconds() > mSpawnInterval)
        {
            mSpawnClock.restart();
            spawnEnemy(playerCenter);
            mSpawnedCount++;

            float minInterval = mStages[mCurrentStage].spawnIntervalMin;
            if (mSpawnInterval > minInterval) mSpawnInterval -= 0.05f;
            if (mSpawnInterval < minInterval) mSpawnInterval = minInterval;
        }
    }

    for (auto& e : mEnemies) e.update(dt, playerCenter);

    for (size_t i = 0; i < mEnemies.size(); i++)
    {
        if (mEnemies[i].isDead())
        {
            sf::Vector2f deathPos = mEnemies[i].getPosition();
            mEnemies.erase(mEnemies.begin() + i);
            i--;

            if (mSpawnedCount >= total && mEnemies.empty())
                dropItem(deathPos);
        }
    }

    for (auto& item : mItems) item.update(dt);
    mItems.erase(
        std::remove_if(mItems.begin(), mItems.end(),
            [](const Item& it){ return it.shouldRemove(); }),
        mItems.end()
    );

    // Stage selesai jika semua musuh mati DAN item sudah diambil (atau tidak ada item)
    if (mSpawnedCount >= total && mEnemies.empty() && mItems.empty())
    {
        if (mCurrentStage + 1 < (int)mStages.size())
        {
            mState = StageState::StageClear;
            mClearClock.restart();

            std::string next = "STAGE " + std::to_string(mCurrentStage + 2) + " INCOMING...";
            mStageNameText.setString(next);
            sf::FloatRect b = mStageNameText.getLocalBounds();
            mStageNameText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            mStageNameText.setPosition({mMapW / 2.f, mMapH / 2.f + 40.f});
        }
        else
        {
            mState = StageState::AllClear;
        }
    }
}

void StageManager::updateStageClearTimer(float dt)
{
    if (mClearClock.getElapsedTime().asSeconds() > mClearDuration)
        nextStage();
}

void StageManager::dropItem(sf::Vector2f position)
{
    ItemType     type;
    sf::Texture* tex = nullptr;

    if      (mCurrentStage == 0) { type = ItemType::TerrorEye;       tex = mTerroreyeTex; }
    else if (mCurrentStage == 1) { type = ItemType::BlasphemousMask;  tex = mMaskTex;      }
    else if (mCurrentStage == 2) { type = ItemType::DevilWing;        tex = mWingTex;      }
    else return;

    mItems.emplace_back(tex, mSpawnFxTex, position, type);
}

void StageManager::draw(sf::RenderWindow& window)
{
    for (auto& e    : mEnemies) e.draw(window);
    for (auto& item : mItems)   item.draw(window);

    if (mState == StageState::StageClear)
    {
        window.draw(mStageClearText);
        window.draw(mStageNameText);
    }
}

void StageManager::spawnEnemy(sf::Vector2f playerCenter)
{
    int side = std::rand() % 4;
    float ex, ey;
    if (side == 0)      { ex = (float)(std::rand() % (int)mMapW); ey = -80.f; }
    else if (side == 1) { ex = (float)(std::rand() % (int)mMapW); ey = mMapH + 80.f; }
    else if (side == 2) { ex = -80.f; ey = (float)(std::rand() % (int)mMapH); }
    else                { ex = mMapW + 80.f; ey = (float)(std::rand() % (int)mMapH); }

    sf::Vector2f pos(ex, ey);
    sf::Vector2f dir = playerCenter - pos;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) dir /= len;

    int hp = mStages[mCurrentStage].enemyHp;
    mEnemies.emplace_back(mWalkTex, mDespawnTex, pos, dir, hp);
}

void StageManager::nextStage()
{
    mCurrentStage++;
    mSpawnedCount  = 0;
    mSpawnInterval = mStages[mCurrentStage].spawnIntervalStart;
    mState         = StageState::Playing;
    mSpawnClock.restart();
}

void StageManager::reset()
{
    mEnemies.clear();
    mItems.clear();
    mCurrentStage  = 0;
    mSpawnedCount  = 0;
    mSpawnInterval = mStages[0].spawnIntervalStart;
    mState         = StageState::Playing;
    mSpawnClock.restart();
}

std::vector<Enemy>& StageManager::getEnemies() { return mEnemies; }
std::vector<Item>&  StageManager::getItems()    { return mItems;   }
StageState          StageManager::getState()    const { return mState; }
int StageManager::getCurrentStage()             const { return mCurrentStage; }