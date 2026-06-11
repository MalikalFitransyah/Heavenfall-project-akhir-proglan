#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Arrow
{
public:
    Arrow(sf::Texture& texture, sf::Vector2f position,
          sf::Vector2f direction, bool piercing = false);

    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isOutOfBounds(float mapW, float mapH) const;

    sf::FloatRect getHitbox()    const;
    sf::Vector2f  getPosition()  const;
    bool          isPiercing()   const;

private:
    sf::Sprite   mSprite;
    sf::Vector2f mVelocity;
    sf::Clock    mAnimClock;
    int          mFrame;
    bool         mPiercing;

    static const int FRAME_W    = 64;
    static const int FRAME_H    = 64;
    static const int LOOP_START = 8;
    static const int LOOP_END   = 12;
};