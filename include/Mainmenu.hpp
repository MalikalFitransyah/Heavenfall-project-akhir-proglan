#pragma once
#include <SFML/Graphics.hpp>

enum class MenuResult { None, Play, Quit };

class MainMenu
{
public:
    MainMenu(sf::Texture& bgTex, sf::Font& fontRegular, sf::Font& fontBold,
             float mapW, float mapH);

    void handleEvent(const sf::Event& event, sf::Vector2f mouse);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    MenuResult getResult() const;

private:
    void updateHover(sf::Vector2f mouse);

    float mMapW, mMapH;

    sf::Sprite         mBg;
    sf::RectangleShape mVignette;
    sf::RectangleShape mLeftDark;

    sf::Text           mTitle;
    sf::Text           mTitleShadow;
    sf::Text           mPlayText;
    sf::Text           mQuitText;
    sf::RectangleShape mSelector;

    int        mHoveredIndex;
    MenuResult mResult;
    bool       mPlayPressed;

    float      mTitleAlpha;
    float      mAlphaDir;

    // Fade in (buka menu)
    sf::RectangleShape mFadeOverlay;
    float              mFadeAlpha;
    bool               mFadeDone;

    // Fade out (saat play ditekan)
    bool               mFadingOut;
    float              mFadeOutAlpha;
};