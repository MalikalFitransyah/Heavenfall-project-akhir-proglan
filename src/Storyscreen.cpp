#include "StoryScreen.hpp"
#include <cstdint>

// ============================================================
// GANTI TEKS STORY DI SINI
// ============================================================
static const std::string STORY_TEXT =
    "In a realm beyond death, countless lost souls endlessly pursue the paradise they desire above all else, ETHERNOAH.\n"
    "Yet only a chosen few are granted passage through its sacred gates. Among millions of wandering spirits, one soul\n"
    "is repeatedly selected in every cycle: The Blessed One, the bearer of a fate unlike any other. Celebrated as the final\n"
    "sanctuary of peace, ETHERNOAH hides a darker truth beneath its radiant facade. The realm is plagued by sin and hypocrisy,\n"
    "its once-pure inhabitants consumed by regret so profound that they have transformed into twisted beings known as Miracles.\n" 
    "Bound by destiny, The Blessed One must confront these fallen souls, shoulder the burden of their sins, and purify them,\n" 
    "restoring them to the sacred purity they once possessed.\n";
// ============================================================

StoryScreen::StoryScreen(sf::Font& font, float mapW, float mapH)
    : mMapW(mapW), mMapH(mapH)
    , mBg({mapW, mapH})
    , mStoryText(font, STORY_TEXT, 25)
    , mPromptText(font, "- PRESS TO CONTINUE -", 20)
    , mFade({mapW, mapH})
    , mFadeAlpha(255.f)
    , mPhase(Phase::FadeIn)
    , mResult(StoryResult::None)
    , mPromptVisible(true)
    , mClicked(false)
{
    mBg.setFillColor(sf::Color::Black);

    // Story text — tengah layar, warna krem
    mStoryText.setFillColor(sf::Color(210, 200, 180));
    mStoryText.setLineSpacing(1.6f);
    mStoryText.setLetterSpacing(1.2f);
    sf::FloatRect tb = mStoryText.getLocalBounds();
    mStoryText.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
    mStoryText.setPosition({mapW / 2.f, mapH / 2.f - 60.f});

    // Prompt teks di bawah
    mPromptText.setFillColor(sf::Color(160, 150, 130));
    mPromptText.setLetterSpacing(2.f);
    sf::FloatRect pb = mPromptText.getLocalBounds();
    mPromptText.setOrigin({pb.size.x / 2.f, pb.size.y / 2.f});
    mPromptText.setPosition({mapW / 2.f, mapH - 120.f});

    // Fade overlay
    mFade.setFillColor(sf::Color(0, 0, 0, 255));
}

void StoryScreen::update(float dt)
{
    // Blink prompt
    if (mBlinkClock.getElapsedTime().asSeconds() > 0.6f)
    {
        mPromptVisible = !mPromptVisible;
        mBlinkClock.restart();
    }

    if (mPhase == Phase::FadeIn)
    {
        mFadeAlpha -= 200.f * dt;
        if (mFadeAlpha <= 0.f)
        {
            mFadeAlpha = 0.f;
            mPhase = Phase::Show;
        }
        mFade.setFillColor(sf::Color(0, 0, 0, (uint8_t)mFadeAlpha));
    }
    else if (mPhase == Phase::Show)
    {
        // Tunggu klik
        if (mClicked)
        {
            mPhase     = Phase::FadeOut;
            mFadeAlpha = 0.f;
            mClicked   = false;
        }
    }
    else if (mPhase == Phase::FadeOut)
    {
        mFadeAlpha += 200.f * dt;
        if (mFadeAlpha >= 255.f)
        {
            mFadeAlpha = 255.f;
            mPhase     = Phase::Done;
            mResult    = StoryResult::Done;
        }
        mFade.setFillColor(sf::Color(0, 0, 0, (uint8_t)mFadeAlpha));
    }
}

void StoryScreen::draw(sf::RenderWindow& window)
{
    window.draw(mBg);
    window.draw(mStoryText);
    if (mPhase == Phase::Show && mPromptVisible)
        window.draw(mPromptText);
    if (mPhase != Phase::Show)
        window.draw(mFade);
}

void StoryScreen::handleClick()
{
    if (mPhase == Phase::Show)
        mClicked = true;
}

void StoryScreen::reset()
{
    mFadeAlpha = 255.f;
    mPhase     = Phase::FadeIn;
    mResult    = StoryResult::None;
    mClicked   = false;
    mFade.setFillColor(sf::Color(0, 0, 0, 255));
}

StoryResult StoryScreen::getResult() const { return mResult; }