#include "MainMenu.hpp"
#include <cmath>
#include <cstdint>

MainMenu::MainMenu(sf::Texture& bgTex, sf::Font& fontRegular, sf::Font& fontBold,
                   float mapW, float mapH)
    : mMapW(mapW), mMapH(mapH)
    , mBg(bgTex)
    , mVignette({mapW, mapH})
    , mLeftDark({mapW * 0.55f, mapH})
    , mTitle(fontBold, "HEAVENFALL", 130)
    , mTitleShadow(fontBold, "HEAVENFALL", 130)
    , mPlayText(fontRegular, "PLAY", 52)
    , mQuitText(fontRegular, "QUIT", 52)
    , mSelector({220.f, 3.f})
    , mHoveredIndex(0)
    , mResult(MenuResult::None)
    , mPlayPressed(false)
    , mTitleAlpha(255.f)
    , mAlphaDir(-1.f)
    , mFadeOverlay({mapW, mapH})
    , mFadeAlpha(255.f)
    , mFadeDone(false)
    , mFadingOut(false)
    , mFadeOutAlpha(0.f)
{
    mBg.setScale({mapW / bgTex.getSize().x, mapH / bgTex.getSize().y});

    mVignette.setFillColor(sf::Color(0, 0, 0, 100));

    mLeftDark.setPosition({0.f, 0.f});
    mLeftDark.setFillColor(sf::Color(0, 0, 0, 0));

    mTitleShadow.setFillColor(sf::Color(0, 0, 0, 180));
    mTitleShadow.setLetterSpacing(1.5f);
    sf::FloatRect tsb = mTitleShadow.getLocalBounds();
    mTitleShadow.setOrigin({tsb.size.x / 2.f, tsb.size.y / 2.f});
    mTitleShadow.setPosition({mapW * 0.30f + 4.f, mapH * 0.28f + 4.f});

    // Judul
    mTitle.setFillColor(sf::Color(220, 210, 190));
    mTitle.setStyle(sf::Text::Bold);
    mTitle.setLetterSpacing(1.5f);
    sf::FloatRect tb = mTitle.getLocalBounds();
    mTitle.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    mTitle.setPosition({mapW * 0.30f, mapH * 0.28f});

    float menuX   = mapW * 0.13f;
    float menuY   = mapH * 0.50f;
    float spacing = 80.f;

    mPlayText.setFillColor(sf::Color(220, 220, 200));
    mPlayText.setLetterSpacing(2.f);
    mPlayText.setPosition({menuX, menuY});

    mQuitText.setFillColor(sf::Color(150, 150, 140));
    mQuitText.setLetterSpacing(2.f);
    mQuitText.setPosition({menuX, menuY + spacing});

    mSelector.setFillColor(sf::Color(200, 190, 160));
    mSelector.setPosition({menuX, menuY + 62.f});

    mFadeOverlay.setFillColor(sf::Color(0, 0, 0, 255));
}

void MainMenu::handleEvent(const sf::Event& event, sf::Vector2f mouse)
{
    if (!mFadeDone || mFadingOut) return;

    updateHover(mouse);

    if (auto mb = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mb->button == sf::Mouse::Button::Left)
        {
            if (mPlayText.getGlobalBounds().contains(mouse))
            {
                mFadingOut   = true;
                mFadeOutAlpha = 0.f;
                mPlayPressed = true;
            }
            else if (mQuitText.getGlobalBounds().contains(mouse))
            {
                mResult = MenuResult::Quit;
            }
        }
    }

    if (auto kb = event.getIf<sf::Event::KeyPressed>())
    {
        if (kb->code == sf::Keyboard::Key::Up   || kb->code == sf::Keyboard::Key::W)
            mHoveredIndex = 0;
        if (kb->code == sf::Keyboard::Key::Down || kb->code == sf::Keyboard::Key::S)
            mHoveredIndex = 1;
        if (kb->code == sf::Keyboard::Key::Enter || kb->code == sf::Keyboard::Key::Space)
        {
            if (mHoveredIndex == 0) { mFadingOut = true; mFadeOutAlpha = 0.f; mPlayPressed = true; }
            else                      mResult = MenuResult::Quit;
        }
    }
}

void MainMenu::updateHover(sf::Vector2f mouse)
{
    bool onPlay = mPlayText.getGlobalBounds().contains(mouse);
    bool onQuit = mQuitText.getGlobalBounds().contains(mouse);

    if (onPlay) mHoveredIndex = 0;
    if (onQuit) mHoveredIndex = 1;

    if (mHoveredIndex == 0)
    {
        mPlayText.setFillColor(sf::Color::White);
        mQuitText.setFillColor(sf::Color(150, 150, 140));
        sf::FloatRect pb = mPlayText.getLocalBounds();
        sf::Vector2f  pp = mPlayText.getPosition();
        mSelector.setPosition({pp.x, pp.y + 62.f});
        mSelector.setSize({pb.size.x + 20.f, 3.f});
    }
    else
    {
        mPlayText.setFillColor(sf::Color(150, 150, 140));
        mQuitText.setFillColor(sf::Color::White);
        sf::FloatRect qb = mQuitText.getLocalBounds();
        sf::Vector2f  qp = mQuitText.getPosition();
        mSelector.setPosition({qp.x, qp.y + 62.f});
        mSelector.setSize({qb.size.x + 20.f, 3.f});
    }
}

void MainMenu::update(float dt)
{
    if (!mFadeDone)
    {
        mFadeAlpha -= 180.f * dt;
        if (mFadeAlpha <= 0.f) { mFadeAlpha = 0.f; mFadeDone = true; }
        mFadeOverlay.setFillColor(sf::Color(0, 0, 0, (uint8_t)mFadeAlpha));
        return;
    }

    if (mFadingOut)
    {
        mFadeOutAlpha += 220.f * dt;
        if (mFadeOutAlpha >= 255.f)
        {
            mFadeOutAlpha = 255.f;
            mResult       = MenuResult::Play;
        }
        mFadeOverlay.setFillColor(sf::Color(0, 0, 0, (uint8_t)mFadeOutAlpha));
        return;
    }

    mTitleAlpha += mAlphaDir * 25.f * dt;
    if (mTitleAlpha <= 190.f) { mTitleAlpha = 190.f; mAlphaDir =  1.f; }
    if (mTitleAlpha >= 255.f) { mTitleAlpha = 255.f; mAlphaDir = -1.f; }

    sf::Color titleCol = mTitle.getFillColor();
    titleCol.a = (uint8_t)mTitleAlpha;
    mTitle.setFillColor(titleCol);
}

void MainMenu::draw(sf::RenderWindow& window)
{
    window.draw(mBg);
    window.draw(mLeftDark);
    window.draw(mVignette);
    window.draw(mTitleShadow);
    window.draw(mTitle);
    window.draw(mPlayText);
    window.draw(mQuitText);
    window.draw(mSelector);

    if (!mFadeDone || mFadingOut)
        window.draw(mFadeOverlay);
}

MenuResult MainMenu::getResult() const { return mResult; }