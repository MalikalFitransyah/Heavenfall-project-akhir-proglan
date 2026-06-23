#include "StageManager.hpp"
#include <cstdlib>
#include <cmath>
#include <string>
#include <algorithm>

StageManager::StageManager(sf::Texture* walkTex,    sf::Texture* despawnTex,
                           sf::Texture* terroreyeTex, sf::Texture* maskTex,
                           sf::Texture* wingTex,      sf::Texture* spawnFxTex,
                           sf::Texture* bossTex,      sf::Texture* bossAtkTex,
                           sf::Texture* laserTex,     sf::Texture* bossDespawnTex,
                           sf::Font& font, float mapW, float mapH)
    : mWalkTex(walkTex), mDespawnTex(despawnTex)
    , mTerroreyeTex(terroreyeTex), mMaskTex(maskTex)
    , mWingTex(wingTex), mSpawnFxTex(spawnFxTex)
    , mBossTex(bossTex), mBossAtkTex(bossAtkTex), mLaserTex(laserTex)
    , mBossDespawnTex(bossDespawnTex)
    , mBoss(nullptr)
    , mCurrentStage(0), mSpawnedCount(0)
    , mSpawnInterval(2.0f)
    , mState(StageState::Playing)
    , mClearDuration(3.0f)
    , mMapW(mapW), mMapH(mapH)
    , mStageClearText(font, "STAGE CLEAR!", 100)
    , mStageNameText(font, "", 60)
    , mBossWarningText(font, "WARNING", 120)
    , mShowBossWarn(false)
    , mBossSpawned(false)
{
    // totalEnemies, spawnStart, spawnMin, enemyHp, isBossStage
    mStages = {
        { 15, 2.0f, 1.2f, 1, false },  // Stage 1
        { 30, 1.8f, 0.9f, 2, false },  // Stage 2
        { 50, 1.5f, 0.6f, 3, false },  // Stage 3
        { 80, 1.2f, 0.3f, 4, false },  // Stage 4
        {  0, 0.0f, 0.0f, 0, true  },  // Stage 5 - BOSS
    };

    mStageClearText.setFillColor(sf::Color::White);
    mStageClearText.setStyle(sf::Text::Bold);
    sf::FloatRect b = mStageClearText.getLocalBounds();
    mStageClearText.setOrigin({b.size.x/2.f, b.size.y/2.f});
    mStageClearText.setPosition({mapW/2.f, mapH/2.f - 80.f});

    mStageNameText.setFillColor(sf::Color(200,200,200));

    // Boss warning
    mBossWarningText.setFillColor(sf::Color(200, 30, 30));
    mBossWarningText.setStyle(sf::Text::Bold);
    sf::FloatRect wb = mBossWarningText.getLocalBounds();
    mBossWarningText.setOrigin({wb.size.x/2.f, wb.size.y/2.f});
    mBossWarningText.setPosition({mapW/2.f, mapH/2.f});
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

    auto& stage = mStages[mCurrentStage];

    // ===== BOSS STAGE =====
if (stage.isBossStage)
{
    if (!mBossSpawned && !mShowBossWarn)
    {
        mShowBossWarn = true;
        mBossWarnClock.restart();
    }

    if (mShowBossWarn && mBossWarnClock.getElapsedTime().asSeconds() > 2.f)
    {
        mShowBossWarn = false;
        mBossSpawned  = true;
        spawnBoss(playerCenter);
    }
        if (mBoss)
        {
            mBoss->update(dt, playerCenter);

            if (mBoss->isDead() && mBoss->isDespawnDone())
            {
                delete mBoss;
                mBoss  = nullptr;
                mState = StageState::AllClear;
            }
        }

        for (auto& item : mItems) item.update(dt);
        mItems.erase(
            std::remove_if(mItems.begin(), mItems.end(),
                [](const Item& it){ return it.shouldRemove(); }),
            mItems.end()
        );
        return;
    }

    // ===== NORMAL STAGE =====
    int total = stage.totalEnemies;

    if (mSpawnedCount < total)
    {
        if (mSpawnClock.getElapsedTime().asSeconds() > mSpawnInterval)
        {
            mSpawnClock.restart();
            spawnEnemy(playerCenter);
            mSpawnedCount++;

            float minInterval = stage.spawnIntervalMin;
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

    if (mSpawnedCount >= total && mEnemies.empty() && mItems.empty())
    {
        if (mCurrentStage + 1 < (int)mStages.size())
        {
            mState = StageState::StageClear;
            mClearClock.restart();

            std::string next = "STAGE " + std::to_string(mCurrentStage + 2) + " INCOMING...";
            if (mCurrentStage + 1 == 4) next = "??? INCOMING...";
            mStageNameText.setString(next);
            sf::FloatRect b = mStageNameText.getLocalBounds();
            mStageNameText.setOrigin({b.size.x/2.f, b.size.y/2.f});
            mStageNameText.setPosition({mMapW/2.f, mMapH/2.f + 40.f});
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

    if      (mCurrentStage == 0) { type = ItemType::TerrorEye;      tex = mTerroreyeTex; }
    else if (mCurrentStage == 1) { type = ItemType::BlasphemousMask; tex = mMaskTex;      }
    else if (mCurrentStage == 2) { type = ItemType::DevilWing;       tex = mWingTex;      }
    else return;

    mItems.emplace_back(tex, mSpawnFxTex, position, type);
}

void StageManager::draw(sf::RenderWindow& window)
{
    for (auto& e    : mEnemies) e.draw(window);
    for (auto& item : mItems)   item.draw(window);
    if (mBoss) mBoss->draw(window);

    if (mState == StageState::StageClear)
    {
        window.draw(mStageClearText);
        window.draw(mStageNameText);
    }

    if (mShowBossWarn)
    {
        // Blink warning
        float t = mBossWarnClock.getElapsedTime().asSeconds();
        float alpha = (std::sin(t * 8.f) > 0) ? 255.f : 0.f;
        sf::Color c = mBossWarningText.getFillColor();
        c.a = (uint8_t)alpha;
        mBossWarningText.setFillColor(c);
        window.draw(mBossWarningText);
    }
}

void StageManager::spawnEnemy(sf::Vector2f playerCenter)
{
    int side = std::rand() % 4;
    float ex, ey;
    if (side == 0)      { ex = (float)(std::rand()%(int)mMapW); ey = -80.f; }
    else if (side == 1) { ex = (float)(std::rand()%(int)mMapW); ey = mMapH+80.f; }
    else if (side == 2) { ex = -80.f; ey = (float)(std::rand()%(int)mMapH); }
    else                { ex = mMapW+80.f; ey = (float)(std::rand()%(int)mMapH); }

    sf::Vector2f pos(ex, ey);
    sf::Vector2f dir = playerCenter - pos;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (len != 0) dir /= len;

    int hp = mStages[mCurrentStage].enemyHp;
    mEnemies.emplace_back(mWalkTex, mDespawnTex, pos, dir, hp);
}

void StageManager::spawnBoss(sf::Vector2f playerCenter)
{
    sf::Vector2f spawnPos(mMapW/2.f, -200.f);
    mBoss = new Boss(mBossTex, mBossAtkTex, mLaserTex, mBossDespawnTex, spawnPos);
}

void StageManager::nextStage()
{
    mCurrentStage++;
    mSpawnedCount  = 0;
    if (!mStages[mCurrentStage].isBossStage)
        mSpawnInterval = mStages[mCurrentStage].spawnIntervalStart;
    mState = StageState::Playing;
    mSpawnClock.restart();
}

void StageManager::reset()
{
    mEnemies.clear();
    mItems.clear();
    delete mBoss;
    mBoss          = nullptr;
    mCurrentStage  = 0;
    mSpawnedCount  = 0;
    mSpawnInterval = mStages[0].spawnIntervalStart;
    mState         = StageState::Playing;
    mShowBossWarn  = false;
    mSpawnClock.restart();
}

std::vector<Enemy>& StageManager::getEnemies() { return mEnemies; }
std::vector<Item>&  StageManager::getItems()    { return mItems;   }
Boss*               StageManager::getBoss()     { return mBoss;    }
StageState          StageManager::getState()    const { return mState; }
int StageManager::getCurrentStage()             const { return mCurrentStage; }