#include "Game.hpp"
#include <iostream>

Game::Game()
    : mWindow(sf::VideoMode({(unsigned)MAP_W, (unsigned)MAP_H}), "Heaven Fall")
    , mBackground(nullptr)
    , mPlayer(nullptr)
    , mStageManager(nullptr)
    , mGameOver(false)
    , mGameActive(true)
    , mOverlay({MAP_W, MAP_H})
    , mGameOverText(mFont, "", 120)
    , mBtnRestart({400.f, 80.f})
    , mTxtRestart(mFont, "", 48)
    , mBtnQuit({400.f, 80.f})
    , mTxtQuit(mFont, "", 48)
    , mStageHUD(mFont, "", 40)
{
    // Load textures
    auto load = [](sf::Texture& tex, const std::string& path) {
        if (!tex.loadFromFile(path)) {
            std::cout << "GAGAL LOAD: " << path << "\n";
            exit(-1);
        }
    };

    load(mIdleTex,    "assets/player_idle.png");
    load(mRightTex,   "assets/player_walk_right.png");
    load(mLeftTex,    "assets/player_walk_left.png");
    load(mBgTex,      "assets/background.png");
    load(mArrowTex,   "assets/arrow.png");
    load(mEnemyTex,   "assets/enemy.png");
    load(mDespawnTex, "assets/miaracle.png");

    if (!mFont.openFromFile("assets/font.ttf")) {
        std::cout << "GAGAL LOAD: assets/font.ttf\n";
        exit(-1);
    }

    // Background
    mBackground = new sf::Sprite(mBgTex);
    mBackground->setScale({MAP_W / mBgTex.getSize().x, MAP_H / mBgTex.getSize().y});

    // Game objects
    mPlayer       = new Player(mIdleTex, mRightTex, mLeftTex, mArrowTex);
    mStageManager = new StageManager(&mEnemyTex, &mDespawnTex, mFont, MAP_W, MAP_H);

    buildUI();
}

void Game::buildUI()
{
    // Overlay
    mOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    // Game Over text
    mGameOverText.setFont(mFont);
    mGameOverText.setString("GAME OVER");
    mGameOverText.setCharacterSize(120);
    mGameOverText.setFillColor(sf::Color::White);
    mGameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect b = mGameOverText.getLocalBounds();
    mGameOverText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    mGameOverText.setPosition({MAP_W / 2.f, MAP_H / 2.f - 160.f});

    // Btn Restart
    mBtnRestart.setFillColor(sf::Color(40, 40, 40));
    mBtnRestart.setOutlineColor(sf::Color::White);
    mBtnRestart.setOutlineThickness(3.f);
    mBtnRestart.setOrigin({200.f, 40.f});
    mBtnRestart.setPosition({MAP_W / 2.f, MAP_H / 2.f + 40.f});

    mTxtRestart.setFont(mFont);
    mTxtRestart.setString("RESTART");
    mTxtRestart.setCharacterSize(48);
    mTxtRestart.setFillColor(sf::Color::White);
    sf::FloatRect rb = mTxtRestart.getLocalBounds();
    mTxtRestart.setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
    mTxtRestart.setPosition({MAP_W / 2.f, MAP_H / 2.f + 40.f});

    // Btn Quit
    mBtnQuit.setFillColor(sf::Color(40, 40, 40));
    mBtnQuit.setOutlineColor(sf::Color::White);
    mBtnQuit.setOutlineThickness(3.f);
    mBtnQuit.setOrigin({200.f, 40.f});
    mBtnQuit.setPosition({MAP_W / 2.f, MAP_H / 2.f + 160.f});

    mTxtQuit.setFont(mFont);
    mTxtQuit.setString("QUIT");
    mTxtQuit.setCharacterSize(48);
    mTxtQuit.setFillColor(sf::Color::White);
    sf::FloatRect qb = mTxtQuit.getLocalBounds();
    mTxtQuit.setOrigin({qb.size.x / 2.f, qb.size.y / 2.f});
    mTxtQuit.setPosition({MAP_W / 2.f, MAP_H / 2.f + 160.f});

    // HUD Stage
    mStageHUD.setFont(mFont);
    mStageHUD.setCharacterSize(40);
    mStageHUD.setFillColor(sf::Color::White);
    mStageHUD.setPosition({40.f, 40.f});
}

void Game::run()
{
    while (mWindow.isOpen())
    {
        float dt = mDeltaClock.restart().asSeconds();
        processEvents();
        update(dt);
        draw();
    }
}

void Game::processEvents()
{
    sf::Vector2i mousePix = sf::Mouse::getPosition(mWindow);
    sf::Vector2f mouse    = mWindow.mapPixelToCoords(mousePix);

    while (auto event = mWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            mWindow.close();

        if (event->is<sf::Event::MouseButtonPressed>())
        {
            auto mb = event->getIf<sf::Event::MouseButtonPressed>();
            if (mb && mb->button == sf::Mouse::Button::Left)
            {
                if (mGameOver)
                {
                    if (mBtnRestart.getGlobalBounds().contains(mouse))
                        resetGame();
                    else if (mBtnQuit.getGlobalBounds().contains(mouse))
                        mWindow.close();
                }
                else if (mGameActive && !mPlayer->isCharging()
                         && mStageManager->getState() == StageState::Playing)
                {
                    mPlayer->startCharge(mouse);
                }
            }
        }

        if (event->is<sf::Event::MouseMoved>())
        {
            mBtnRestart.setFillColor(mBtnRestart.getGlobalBounds().contains(mouse)
                ? sf::Color(80, 80, 80) : sf::Color(40, 40, 40));
            mBtnQuit.setFillColor(mBtnQuit.getGlobalBounds().contains(mouse)
                ? sf::Color(80, 80, 80) : sf::Color(40, 40, 40));
        }
    }
}

void Game::update(float dt)
{
    if (!mGameActive || mGameOver) return;

    mPlayer->update(dt, MAP_W, MAP_H);
    mStageManager->update(dt, mPlayer->getCenter());

    // Update HUD
    int stage = mStageManager->getCurrentStage() + 1;
    mStageHUD.setString("STAGE " + std::to_string(stage));

    // Cek arrow vs enemy
    auto& arrows  = mPlayer->getArrows();
    auto& enemies = mStageManager->getEnemies();

    for (size_t i = 0; i < arrows.size(); i++)
    {
        for (size_t j = 0; j < enemies.size(); j++)
        {
            if (!enemies[j].isDying() &&
                arrows[i].getHitbox().findIntersection(enemies[j].getHitbox()))
            {
                enemies[j].triggerDeath();
                arrows.erase(arrows.begin() + i);
                i--;
                break;
            }
        }
    }

    // Cek enemy vs player
    sf::FloatRect playerHitbox = mPlayer->getHitbox();
    for (auto& e : enemies)
    {
        if (!e.isDying() &&
            e.getHitbox().findIntersection(playerHitbox))
        {
            mGameOver   = true;
            mGameActive = false;
            return;
        }
    }
}

void Game::draw()
{
    mWindow.clear();
    mWindow.draw(*mBackground);

    mStageManager->draw(mWindow);
    mPlayer->draw(mWindow);

    // HUD
    if (!mGameOver)
        mWindow.draw(mStageHUD);

    // Game Over UI
    if (mGameOver)
    {
        mWindow.draw(mOverlay);
        mWindow.draw(mGameOverText);
        mWindow.draw(mBtnRestart);
        mWindow.draw(mTxtRestart);
        mWindow.draw(mBtnQuit);
        mWindow.draw(mTxtQuit);
    }

    mWindow.display();
}

void Game::resetGame()
{
    delete mPlayer;
    delete mStageManager;

    mPlayer       = new Player(mIdleTex, mRightTex, mLeftTex, mArrowTex);
    mStageManager = new StageManager(&mEnemyTex, &mDespawnTex, mFont, MAP_W, MAP_H);

    mGameOver   = false;
    mGameActive = true;
}