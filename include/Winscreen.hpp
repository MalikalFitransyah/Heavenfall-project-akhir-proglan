#pragma once
#include <SFML/Graphics.hpp>

class WinScreen
{
public:
    WinScreen(sf::RenderWindow& window, sf::Font& font);

    void update(float dt);
    void draw();
    bool isInputReady() const; 

private:
    sf::RenderWindow&  mWindow;
    sf::RectangleShape mOverlay;
    sf::Text           mWellDone;
    sf::Text           mContinue;

    float mFadeAlpha;
    bool  mFadeDone;

    float mBlinkTimer;
    bool  mShowContinue;
};