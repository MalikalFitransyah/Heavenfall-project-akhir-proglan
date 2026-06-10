#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "StageManager.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void draw();
    void resetGame();
    void buildUI();

    // Window
    sf::RenderWindow mWindow;

    static constexpr float MAP_W = 2560.f;
    static constexpr float MAP_H = 1440.f;

    // Textures
    sf::Texture mIdleTex, mRightTex, mLeftTex;
    sf::Texture mBgTex;
    sf::Texture mArrowTex;
    sf::Texture mEnemyTex;
    sf::Texture mDespawnTex;

    // Sprites (pointer karena sf::Sprite butuh texture saat konstruksi)
    sf::Sprite* mBackground;

    // Font
    sf::Font    mFont;

    // Game objects
    Player*       mPlayer;
    StageManager* mStageManager;

    // State
    bool mGameOver;
    bool mGameActive;

    // UI Game Over
    sf::RectangleShape mOverlay;
    sf::Text           mGameOverText;
    sf::RectangleShape mBtnRestart;
    sf::Text           mTxtRestart;
    sf::RectangleShape mBtnQuit;
    sf::Text           mTxtQuit;

    // UI Stage HUD
    sf::Text           mStageHUD;

    sf::Clock mDeltaClock;
};