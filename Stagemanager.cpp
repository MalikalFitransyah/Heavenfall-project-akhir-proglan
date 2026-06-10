#include "StageManager.hpp"
#include <cstdlib>
#include <cmath>
#include <string>

StageManager::StageManager(sf::Texture* walkTex, sf::Texture* despawnTex,
                           sf::Font& font, float mapW, float mapH)
    : mWalkTex(walkTex)
    , mDespawnTex(despawnTex)
    , mCurrentStage(0)
    , mSpawnedCount(0)
    , mKilledCount(0)
    , mSpawnInterval(2.0f)
    , mState(StageState::Playing)
    , mClearDuration(3.0f)
    , mMapW(mapW)
    , mMapH(mapH)
    , mStageClearText(font, "STAGE CLEAR!", 100)
    , mStageNameText(font, "", 60)
{
    // Definisi stage: total musuh per stage
    mStages = { {15}, {30}, {50} };

    // Style teks STAGE CLEAR
    mStageClearText.setFillColor(sf::Color::White);
    mStageClearText.setStyle(sf::Text::Bold);
    sf::FloatRect b = mStageClearText.getLocalBounds();
    mStageClearText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    mStageClearText.setPosition({mapW / 2.f, mapH / 2.f - 80.f});

    // Style teks nama stage berikutnya
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
        return;
    }

    if (mState == StageState::AllClear) return;

    // Spawn musuh jika belum mencapai total stage
    int total = mStages[mCurrentStage].totalEnemies;
    if (mSpawnedCount < total)
    {
        if (mSpawnClock.getElapsedTime().asSeconds() > mSpawnInterval)
        {
            mSpawnClock.restart();
            spawnEnemy(playerCenter);
            mSpawnedCount++;
        }
    }

    // Update semua enemy
    for (auto& e : mEnemies)
        e.update(dt, playerCenter);

    // Hapus enemy yang sudah benar-benar mati, hitung yang terbunuh
    for (size_t i = 0; i < mEnemies.size(); i++)
    {
        if (mEnemies[i].isDead())
        {
            mKilledCount++;
            mEnemies.erase(mEnemies.begin() + i);
            i--;
        }
    }

    // Cek apakah stage selesai
    if (mSpawnedCount >= total && mEnemies.empty())
    {
        if (mCurrentStage + 1 < (int)mStages.size())
        {
            mState = StageState::StageClear;
            mClearClock.restart();

            // Update teks stage berikutnya
            std::string nextName = "STAGE " + std::to_string(mCurrentStage + 2) + " INCOMING...";
            mStageNameText.setString(nextName);
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

void StageManager::draw(sf::RenderWindow& window)
{
    for (auto& e : mEnemies) e.draw(window);

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

    mEnemies.emplace_back(mWalkTex, mDespawnTex, pos, dir);
}

void StageManager::nextStage()
{
    mCurrentStage++;
    mSpawnedCount  = 0;
    mSpawnInterval = 1.5f; // sedikit lebih cepat spawn tiap stage
    mState         = StageState::Playing;
    mSpawnClock.restart();
}

void StageManager::reset()
{
    mEnemies.clear();
    mCurrentStage  = 0;
    mSpawnedCount  = 0;
    mKilledCount   = 0;
    mSpawnInterval = 2.0f;
    mState         = StageState::Playing;
    mSpawnClock.restart();
}

std::vector<Enemy>& StageManager::getEnemies()  { return mEnemies; }
StageState          StageManager::getState()     const { return mState; }
int                 StageManager::getCurrentStage() const { return mCurrentStage; }