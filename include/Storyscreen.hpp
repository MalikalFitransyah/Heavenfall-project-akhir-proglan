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

    sf::RectangleShape mBg;

    sf::Text mStoryText;
    sf::Text mPromptText; 

    sf::RectangleShape mFade;
    float              mFadeAlpha;

    enum class Phase
    {
        FadeIn,     
        Show,       
        FadeOut,   
        Done
    };

    Phase      mPhase;
    StoryResult mResult;

  
    sf::Clock  mBlinkClock;
    bool       mPromptVisible;

  
    bool       mClicked;
};