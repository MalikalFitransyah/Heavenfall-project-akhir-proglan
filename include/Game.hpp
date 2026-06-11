#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "StageManager.hpp"

class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void draw();
    void resetGame();
    void buildUI();
    void updateItemHUD();

    sf::RenderWindow mWindow;
    static constexpr float MAP_W = 2560.f;
    static constexpr float MAP_H = 1440.f;

    sf::Texture mIdleTex, mRightTex, mLeftTex;
    sf::Texture mBgTex;
    sf::Texture mArrowTex;
    sf::Texture mEnemyTex;
    sf::Texture mDespawnTex;
    sf::Texture mTerroreyeTex;
    sf::Texture mMaskTex;
    sf::Texture mWingTex;
    sf::Texture mSpawnFxTex;

    sf::Sprite* mBackground;
    sf::Font    mFont;

    Player*       mPlayer;
    StageManager* mStageManager;

    bool mGameOver;
    bool mGameActive;

    // Game Over UI
    sf::RectangleShape mOverlay;
    sf::Text           mGameOverText;
    sf::RectangleShape mBtnRestart;
    sf::Text           mTxtRestart;
    sf::RectangleShape mBtnQuit;
    sf::Text           mTxtQuit;

    // HUD tengah atas - stage
    sf::Text           mStageHUD;

    // HUD pojok kanan atas - item yang sudah diambil
    sf::Sprite         mHudTerrorEye;
    sf::Sprite         mHudMask;
    sf::Sprite         mHudWing;
    bool               mShowTerrorEye;
    bool               mShowMask;
    bool               mShowWing;

    sf::Clock mDeltaClock;
};