#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class StoryResult { None, Done };

class StoryScreen
{
public:
    StoryScreen(sf::Font& font, float mapW, float mapH);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void handleClick();
    void reset();

    StoryResult getResult() const;

private:
    float mMapW, mMapH;

    // Background hitam
    sf::RectangleShape mBg;

    // Story text
    sf::Text mStoryText;
    sf::Text mPromptText; // "PRESS TO CONTINUE"

    // Fade overlay
    sf::RectangleShape mFade;
    float              mFadeAlpha;

    enum class Phase
    {
        FadeIn,     // fade masuk dari hitam
        Show,       // tampilkan teks
        FadeOut,    // fade keluar ke hitam
        Done
    };

    Phase      mPhase;
    StoryResult mResult;

    // Prompt blink
    sf::Clock  mBlinkClock;
    bool       mPromptVisible;

    // Klik sudah diterima
    bool       mClicked;
};