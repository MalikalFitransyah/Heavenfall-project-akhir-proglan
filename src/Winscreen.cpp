#include "WinScreen.hpp"
#include <cstdint>

WinScreen::WinScreen(sf::RenderWindow& window, sf::Font& font)
    : mWindow(window)
    , mOverlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y))
    , mWellDone(font, "WELL DONE", 80)
    , mContinue(font, "- PRESS ANY KEY TO CONTINUE -", 24)
    , mFadeAlpha(0.f)
    , mFadeDone(false)
    , mBlinkTimer(0.f)
    , mShowContinue(false)
{
    float w = (float)window.getSize().x;
    float h = (float)window.getSize().y;

    mOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    mWellDone.setFillColor(sf::Color(220, 210, 180));
    mWellDone.setLetterSpacing(1.5f);
    sf::FloatRect wb = mWellDone.getLocalBounds();
    mWellDone.setOrigin({ wb.size.x / 2.f, wb.size.y / 2.f });
    mWellDone.setPosition({ w / 2.f, h / 2.f - 50.f });

    mContinue.setFillColor(sf::Color(160, 150, 130));
    mContinue.setLetterSpacing(2.f);
    sf::FloatRect cb = mContinue.getLocalBounds();
    mContinue.setOrigin({ cb.size.x / 2.f, cb.size.y / 2.f });
    mContinue.setPosition({ w / 2.f, h / 2.f + 60.f });
}

void WinScreen::update(float dt)
{
    if (!mFadeDone)
    {
        mFadeAlpha += 160.f * dt;
        if (mFadeAlpha >= 255.f)
        {
            mFadeAlpha = 255.f;
            mFadeDone  = true;
        }
        mOverlay.setFillColor(sf::Color(0, 0, 0, (uint8_t)mFadeAlpha));
    }
    else
    {
        // Blink "press any key"
        mBlinkTimer += dt;
        if (mBlinkTimer >= 0.5f)
        {
            mBlinkTimer   = 0.f;
            mShowContinue = !mShowContinue;
        }
    }
}

void WinScreen::draw()
{
    mWindow.draw(mOverlay);
    if (mFadeDone)
    {
        mWindow.draw(mWellDone);
        if (mShowContinue)
            mWindow.draw(mContinue);
    }
}

bool WinScreen::isInputReady() const { return mFadeDone; }
