#include "Game.hpp"
#include "WinScreen.hpp"
#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>

Game::Game()
    : mWindow(sf::VideoMode({(unsigned)MAP_W,(unsigned)MAP_H}),"Heaven Fall")
    , mBackground(nullptr), mPlayer(nullptr), mStageManager(nullptr)
    , mMainMenu(nullptr), mStoryScreen(nullptr)
    , mState(GameState::MainMenu)
    , mFadeRect({MAP_W,MAP_H}), mFadeAlpha(0.f)
    , mOverlay({MAP_W,MAP_H})
    , mGameOverTitle(mCinzelBold,"",72)
    , mBtnRestart({380.f,75.f}), mTxtRestart(mCinzelReg,"",44)
    , mBtnQuit({380.f,75.f}),    mTxtQuit(mCinzelReg,"",44)
    , mGameOverFade(0.f)
    , mStageHUD(mCinzelReg,"",44)
    , mHudTerrorEye(mTerroreyeTex), mHudMask(mMaskTex), mHudWing(mWingTex)
    , mShowTerrorEye(false), mShowMask(false), mShowWing(false)
    , mWinScreen(nullptr)
{
    auto load = [](sf::Texture& tex, const std::string& path){
        if (!tex.loadFromFile(path)){ std::cout<<"GAGAL LOAD: "<<path<<"\n"; exit(-1); }
    };

    load(mIdleTex,      "assets/player_idle.png");
    load(mRightTex,     "assets/player_walk_right.png");
    load(mLeftTex,      "assets/player_walk_left.png");
    load(mBgTex,        "assets/background.png");
    load(mGateTex,      "assets/gate.png");
    load(mArrowTex,     "assets/arrow.png");
    load(mEnemyTex,     "assets/enemy.png");
    load(mDespawnTex,   "assets/miaracle.png");
    load(mTerroreyeTex, "assets/terroreye.png");
    load(mMaskTex,      "assets/blasphemousmask.png");
    load(mWingTex,      "assets/devilwing.png");
    load(mSpawnFxTex,   "assets/itemspawn.png");
    load(mBossTex,      "assets/boss.png");
    load(mBossAtkTex,   "assets/bossatk.png");
    load(mLaserTex,     "assets/Laser.png");
    load(mBossDespawnTex, "assets/boss_despawn.png");

    if (!mFont.openFromFile("assets/font.ttf"))
    { std::cout<<"GAGAL LOAD: assets/font.ttf\n"; exit(-1); }
    if (!mCinzelReg.openFromFile("assets/cinzel_regular.ttf"))
    { std::cout<<"GAGAL LOAD: assets/cinzel_regular.ttf\n"; exit(-1); }
    if (!mCinzelBold.openFromFile("assets/cinzel_bold.ttf"))
    { std::cout<<"GAGAL LOAD: assets/cinzel_bold.ttf\n"; exit(-1); }

    mBackground = new sf::Sprite(mBgTex);
    mBackground->setScale({MAP_W/mBgTex.getSize().x, MAP_H/mBgTex.getSize().y});

    mMainMenu    = new MainMenu(mGateTex, mCinzelReg, mCinzelBold, MAP_W, MAP_H);
    mStoryScreen = new StoryScreen(mCinzelReg, MAP_W, MAP_H);

    mFadeRect.setFillColor(sf::Color(0,0,0,0));

    buildUI();
    buildGameOverUI();
}

Game::~Game()
{
    delete mBackground; delete mPlayer;
    delete mStageManager; delete mMainMenu; delete mStoryScreen;
    delete mWinScreen;
}

void Game::startGame()
{
    mPlayer = new Player(mIdleTex, mRightTex, mLeftTex, mArrowTex);
    mStageManager = new StageManager(
        &mEnemyTex, &mDespawnTex,
        &mTerroreyeTex, &mMaskTex, &mWingTex, &mSpawnFxTex,
        &mBossTex, &mBossAtkTex, &mLaserTex, &mBossDespawnTex,
        mCinzelReg, MAP_W, MAP_H
    );
}

void Game::buildUI()
{
    mStageHUD.setFont(mCinzelBold);
    mStageHUD.setCharacterSize(30);
    mStageHUD.setFillColor(sf::Color::Black);
    mStageHUD.setStyle(sf::Text::Bold);

    float hudScale=1.5f, rightEdge=MAP_W-40.f, topY=40.f, spacing=80.f;

    mHudTerrorEye.setTexture(mTerroreyeTex);
    mHudTerrorEye.setTextureRect(sf::IntRect({0,0},{64,64}));
    mHudTerrorEye.setScale({hudScale,hudScale});
    mHudTerrorEye.setOrigin({64.f,0.f});
    mHudTerrorEye.setPosition({rightEdge,topY});

    mHudMask.setTexture(mMaskTex);
    mHudMask.setTextureRect(sf::IntRect({0,0},{64,64}));
    mHudMask.setScale({hudScale,hudScale});
    mHudMask.setOrigin({64.f,0.f});
    mHudMask.setPosition({rightEdge-spacing,topY});

    mHudWing.setTexture(mWingTex);
    mHudWing.setTextureRect(sf::IntRect({0,0},{64,64}));
    mHudWing.setScale({hudScale,hudScale});
    mHudWing.setOrigin({64.f,0.f});
    mHudWing.setPosition({rightEdge-spacing*2.f,topY});
}

void Game::buildGameOverUI()
{
    mOverlay.setFillColor(sf::Color(0,0,0,0));

    mGameOverTitle.setFont(mCinzelReg);
    mGameOverTitle.setString("YOUR SOUL IS NO LONGER YOURS");
    mGameOverTitle.setCharacterSize(40);
    mGameOverTitle.setFillColor(sf::Color(210,200,180,0));
    mGameOverTitle.setLetterSpacing(1.3f);
    sf::FloatRect tb = mGameOverTitle.getLocalBounds();
    mGameOverTitle.setOrigin({tb.size.x/2.f, tb.size.y/2.f});
    mGameOverTitle.setPosition({MAP_W/2.f, MAP_H/2.f-120.f});

    mBtnRestart.setFillColor(sf::Color(0,0,0,0));
    mBtnRestart.setOutlineColor(sf::Color(0,0,0,0));
    mBtnRestart.setOutlineThickness(0.f);
    mBtnRestart.setOrigin({190.f,37.5f});
    mBtnRestart.setPosition({MAP_W/2.f-260.f, MAP_H/2.f+80.f});

    mTxtRestart.setFont(mCinzelReg);
    mTxtRestart.setString("RESTART");
    mTxtRestart.setCharacterSize(30);
    mTxtRestart.setFillColor(sf::Color(220,210,190,0));
    mTxtRestart.setLetterSpacing(2.f);
    sf::FloatRect rb = mTxtRestart.getLocalBounds();
    mTxtRestart.setOrigin({rb.size.x/2.f, rb.size.y/2.f});
    mTxtRestart.setPosition({MAP_W/2.f-260.f, MAP_H/2.f+80.f});

    mBtnQuit.setFillColor(sf::Color(0,0,0,0));
    mBtnQuit.setOutlineColor(sf::Color(0,0,0,0));
    mBtnQuit.setOutlineThickness(0.f);
    mBtnQuit.setOrigin({190.f,37.5f});
    mBtnQuit.setPosition({MAP_W/2.f+260.f, MAP_H/2.f+80.f});

    mTxtQuit.setFont(mCinzelReg);
    mTxtQuit.setString("QUIT");
    mTxtQuit.setCharacterSize(30);
    mTxtQuit.setFillColor(sf::Color(220,210,190,0));
    mTxtQuit.setLetterSpacing(2.f);
    sf::FloatRect qb = mTxtQuit.getLocalBounds();
    mTxtQuit.setOrigin({qb.size.x/2.f, qb.size.y/2.f});
    mTxtQuit.setPosition({MAP_W/2.f+260.f, MAP_H/2.f+80.f});
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
        if (event->is<sf::Event::Closed>()) mWindow.close();

        if (mState == GameState::MainMenu)
            mMainMenu->handleEvent(*event, mouse);
        else if (mState == GameState::Story)
        {
            if (event->is<sf::Event::MouseButtonPressed>() || event->is<sf::Event::KeyPressed>())
                mStoryScreen->handleClick();
        }
        else if (mState == GameState::Win)
        {
            if (mWinScreen && mWinScreen->isInputReady())
            {
                if (event->is<sf::Event::KeyPressed>() ||
                    event->is<sf::Event::MouseButtonPressed>())
                {
                    delete mWinScreen;
                    mWinScreen = nullptr;
                    delete mPlayer;       mPlayer = nullptr;
                    delete mStageManager; mStageManager = nullptr;
                    mState = GameState::MainMenu;
                    mMainMenu = new MainMenu(mGateTex, mCinzelReg, mCinzelBold, MAP_W, MAP_H);
                }
            }
        }
        else if (mState == GameState::Playing)
        {
            if (auto mb = event->getIf<sf::Event::MouseButtonPressed>())
                if (mb->button == sf::Mouse::Button::Left &&
                    !mPlayer->isCharging() &&
                    mStageManager->getState() == StageState::Playing)
                    mPlayer->startCharge(mouse);
        }
        else if (mState == GameState::GameOver)
        {
            if (auto mb = event->getIf<sf::Event::MouseButtonPressed>())
                if (mb->button == sf::Mouse::Button::Left)
                {
                    if (mBtnRestart.getGlobalBounds().contains(mouse)) resetGame();
                    else if (mBtnQuit.getGlobalBounds().contains(mouse)) mWindow.close();
                }
            if (event->is<sf::Event::MouseMoved>())
            {
                sf::Color ac(255,255,220,255), nc(220,210,190,255);
                mTxtRestart.setFillColor(mBtnRestart.getGlobalBounds().contains(mouse)?ac:nc);
                mTxtQuit.setFillColor(mBtnQuit.getGlobalBounds().contains(mouse)?ac:nc);
            }
        }
    }
}

void Game::updateItemHUD()
{
    auto& buffs    = mPlayer->getBuffs();
    mShowTerrorEye = buffs.hasTerrorEye;
    mShowMask      = buffs.hasBlasphemous;
    mShowWing      = buffs.hasDevilWing;
}

void Game::update(float dt)
{
    if (mState == GameState::MainMenu)
    {
        mMainMenu->update(dt);
        if (mMainMenu->getResult() == MenuResult::Play)
        { mStoryScreen->reset(); mState = GameState::Story; }
        if (mMainMenu->getResult() == MenuResult::Quit) mWindow.close();
        return;
    }

    if (mState == GameState::Story)
    {
        mStoryScreen->update(dt);
        if (mStoryScreen->getResult() == StoryResult::Done)
        {
            startGame();
            mFadeAlpha = 255.f;
            mFadeRect.setFillColor(sf::Color(0,0,0,255));
            mState = GameState::FadeToGame;
        }
        return;
    }

    if (mState == GameState::FadeToGame)
    {
        mFadeAlpha -= 200.f*dt;
        if (mFadeAlpha <= 0.f){ mFadeAlpha=0.f; mState=GameState::Playing; }
        mFadeRect.setFillColor(sf::Color(0,0,0,(uint8_t)mFadeAlpha));
        return;
    }

    if (mState == GameState::Playing)
    {
        mPlayer->update(dt, MAP_W, MAP_H);
        mStageManager->update(dt, mPlayer->getCenter());

        int stage = mStageManager->getCurrentStage()+1;
        std::string stageStr = (stage > 4) ? "STAGE ???" : "STAGE "+std::to_string(stage);
        mStageHUD.setString(stageStr);
        sf::FloatRect sb = mStageHUD.getLocalBounds();
        mStageHUD.setOrigin({sb.size.x/2.f,0.f});
        mStageHUD.setPosition({MAP_W/2.f,30.f});

        updateItemHUD();

        auto& arrows  = mPlayer->getArrows();
        auto& enemies = mStageManager->getEnemies();
        Boss* boss    = mStageManager->getBoss();

        // Arrow vs enemy
        for (size_t i = 0; i < arrows.size(); i++)
        {
            bool hit = false;
            for (size_t j = 0; j < enemies.size(); j++)
            {
                if (!enemies[j].isDying() && !enemies[j].isInvincible() &&
                    arrows[i].getHitbox().findIntersection(enemies[j].getHitbox()))
                {
                    enemies[j].hit();
                    if (!arrows[i].isPiercing()){ hit=true; break; }
                }
            }
            //arrow vs boss
           if (!hit && boss && !boss->isDead() && !boss->isInvincible() &&
    arrows[i].getHitbox().findIntersection(boss->getHitbox()))
{
    boss->hit();
    hit = true; // arrow selalu hilang saat kena boss, tidak peduli piercing
}

            if (hit){ arrows.erase(arrows.begin()+i); i--; }
        }

        // Player pickup item
        auto& items = mStageManager->getItems();
        for (auto& item : items)
        {
            if (!item.isCollected() &&
                item.getHitbox().findIntersection(mPlayer->getHitbox()))
            {
                item.collect();
                auto& buffs = mPlayer->getBuffs();
                if      (item.getType()==ItemType::TerrorEye)      buffs.hasTerrorEye  =true;
                else if (item.getType()==ItemType::BlasphemousMask) buffs.hasBlasphemous=true;
                else if (item.getType()==ItemType::DevilWing)       buffs.hasDevilWing  =true;
            }
        }

        // Enemy vs player
        for (auto& e : enemies)
            if (!e.isDying() && e.getHitbox().findIntersection(mPlayer->getHitbox()))
            { mState=GameState::FadeToGameOver; mGameOverFade=0.f; return; }

            // Boss laser vs player
        if (boss && !boss->isDead())
        {
            if (boss->checkLaserHit(mPlayer->getHitbox()))
            { mState=GameState::FadeToGameOver; mGameOverFade=0.f; return; }

            // Boss body vs player
            if (boss->getHitbox().findIntersection(mPlayer->getHitbox()))
            { mState=GameState::FadeToGameOver; mGameOverFade=0.f; return; }
        }

        // Boss mati → Win screen
        if (mStageManager->getState() == StageState::AllClear && !mWinScreen)
        {
            mWinScreen = new WinScreen(mWindow, mCinzelReg);
            mState = GameState::Win;
        }

        return;
    }

    if (mState == GameState::Win)
    {
        if (mWinScreen) mWinScreen->update(dt);
        return;
    }

    if (mState == GameState::FadeToGameOver)
    {
        mGameOverFade += 160.f*dt;
        if (mGameOverFade > 255.f) mGameOverFade = 255.f;

        uint8_t a = (uint8_t)mGameOverFade;
        mOverlay.setFillColor(sf::Color(0,0,0,std::min(a,(uint8_t)255)));

        if (mGameOverFade > 160.f)
        {
            uint8_t uiA = (uint8_t)((mGameOverFade-160.f)/95.f*255.f);
            mGameOverTitle.setFillColor(sf::Color(210,200,180,uiA));
            mBtnRestart.setOutlineColor(sf::Color(180,170,150,uiA));
            mBtnQuit.setOutlineColor(sf::Color(180,170,150,uiA));
            mTxtRestart.setFillColor(sf::Color(220,210,190,uiA));
            mTxtQuit.setFillColor(sf::Color(220,210,190,uiA));
        }

        if (mGameOverFade >= 255.f) mState = GameState::GameOver;
        return;
    }
}

void Game::draw()
{
    mWindow.clear(sf::Color::Black);

    if (mState == GameState::MainMenu)
        mMainMenu->draw(mWindow);
    else if (mState == GameState::Story)
        mStoryScreen->draw(mWindow);
    else if (mState == GameState::FadeToGame)
    {
        mWindow.draw(*mBackground);
        mWindow.draw(mFadeRect);
    }
    else if (mState == GameState::Playing)
    {
        mWindow.draw(*mBackground);
        mStageManager->draw(mWindow);
        mPlayer->draw(mWindow);
        mWindow.draw(mStageHUD);
        if (mShowTerrorEye) mWindow.draw(mHudTerrorEye);
        if (mShowMask)      mWindow.draw(mHudMask);
        if (mShowWing)      mWindow.draw(mHudWing);
    }
    else if (mState == GameState::Win)
    {
        mWindow.draw(*mBackground);
        mStageManager->draw(mWindow);
        mPlayer->draw(mWindow);
        if (mWinScreen) mWinScreen->draw();
    }
    else if (mState==GameState::FadeToGameOver || mState==GameState::GameOver)
    {
        mWindow.draw(*mBackground);
        mStageManager->draw(mWindow);
        mPlayer->draw(mWindow);
        mWindow.draw(mOverlay);
        mWindow.draw(mGameOverTitle);
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

    mPlayer = new Player(mIdleTex, mRightTex, mLeftTex, mArrowTex);
    mStageManager = new StageManager(
        &mEnemyTex, &mDespawnTex,
        &mTerroreyeTex, &mMaskTex, &mWingTex, &mSpawnFxTex,
        &mBossTex, &mBossAtkTex, &mLaserTex, &mBossDespawnTex,
        mCinzelReg, MAP_W, MAP_H
    );

    mState=GameState::Playing;
    mShowTerrorEye=mShowMask=mShowWing=false;
    mGameOverFade=0.f;
    mOverlay.setFillColor(sf::Color(0,0,0,0));
    mGameOverTitle.setFillColor(sf::Color(210,200,180,0));
    mBtnRestart.setOutlineColor(sf::Color(180,170,150,0));
    mBtnQuit.setOutlineColor(sf::Color(180,170,150,0));
    mTxtRestart.setFillColor(sf::Color(220,210,190,0));
    mTxtQuit.setFillColor(sf::Color(220,210,190,0));
}