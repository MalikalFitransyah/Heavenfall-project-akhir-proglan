#pragma once
#include <SFML/Graphics.hpp>

class Enemy
{
public:
    Enemy(sf::Texture* walkTex, sf::Texture* despawnTex, sf::Vector2f position, sf::Vector2f direction);

    void update(float dt, sf::Vector2f playerCenter);
    void draw(sf::RenderWindow& window);

    void triggerDeath();
    bool isDying()  const;
    bool isDead()   const;

    sf::FloatRect getHitbox() const;

private:
    sf::Texture*  mDespawnTex;

    sf::Sprite   mSprite;
    sf::Vector2f mVelocity;

    int          mFrame;
    sf::Clock    mAnimClock;

    bool         mDying;
    bool         mDead;
    int          mDyingFrame;
    sf::Clock    mDyingClock;

    static const int FRAME_W     = 64;
    static const int FRAME_H     = 64;
    static const int WALK_FRAMES = 4;
    static const int DIE_FRAMES  = 5;
    static constexpr float SPEED = 200.f;
};