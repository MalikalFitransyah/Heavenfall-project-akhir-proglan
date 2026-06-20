#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "StageManager.hpp"
#include "MainMenu.hpp"
#include "StoryScreen.hpp"
#include "WinScreen.hpp"

enum class GameState { MainMenu, Story, FadeToGame, Playing, FadeToGameOver, GameOver, Win };

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
    void startGame();
    void resetGame();
    void buildUI();
    void buildGameOverUI();
    void updateItemHUD();

    sf::RenderWindow mWindow;
    static constexpr float MAP_W = 2560.f;
    static constexpr float MAP_H = 1440.f;

    sf::Texture mIdleTex, mRightTex, mLeftTex;
    sf::Texture mBgTex, mGateTex;
    sf::Texture mArrowTex;
    sf::Texture mEnemyTex, mDespawnTex;
    sf::Texture mTerroreyeTex, mMaskTex, mWingTex, mSpawnFxTex;
    sf::Texture mBossTex, mBossAtkTex, mLaserTex, mBossDespawnTex;

    sf::Sprite* mBackground;
    sf::Font    mFont, mCinzelReg, mCinzelBold;

    Player*       mPlayer;
    StageManager* mStageManager;
    MainMenu*     mMainMenu;
    StoryScreen*  mStoryScreen;

    GameState mState;

    sf::RectangleShape mFadeRect;
    float              mFadeAlpha;

    sf::RectangleShape mOverlay;
    sf::Text           mGameOverTitle;
    sf::RectangleShape mBtnRestart;
    sf::Text           mTxtRestart;
    sf::RectangleShape mBtnQuit;
    sf::Text           mTxtQuit;
    float              mGameOverFade;

    sf::Text           mStageHUD;
    sf::Sprite         mHudTerrorEye, mHudMask, mHudWing;
    bool               mShowTerrorEye, mShowMask, mShowWing;

    WinScreen* mWinScreen;

    sf::Clock mDeltaClock;
};