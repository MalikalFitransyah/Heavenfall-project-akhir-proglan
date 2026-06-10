#include "Arrow.hpp"
#include <cmath>

Arrow::Arrow(sf::Texture& texture, sf::Vector2f position, sf::Vector2f direction)
    : mSprite(texture), mFrame(LOOP_START)
{
    mSprite.setOrigin({FRAME_W / 2.f, FRAME_H / 2.f});
    mSprite.setScale({4.f, 4.f});
    mSprite.setTextureRect(sf::IntRect({LOOP_START * FRAME_W, 0}, {FRAME_W, FRAME_H}));
    mSprite.setPosition(position);

    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    mSprite.setRotation(sf::degrees(angle));

    mVelocity = direction * 2000.f;
}

void Arrow::update(float dt)
{
    mSprite.move(mVelocity * dt);

    if (mAnimClock.getElapsedTime().asSeconds() > 0.08f)
    {
        mFrame++;
        if (mFrame >= LOOP_END) mFrame = LOOP_START;
        mSprite.setTextureRect(sf::IntRect({mFrame * FRAME_W, 0}, {FRAME_W, FRAME_H}));
        mAnimClock.restart();
    }
}

void Arrow::draw(sf::RenderWindow& window)
{
    window.draw(mSprite);
}

bool Arrow::isOutOfBounds(float mapW, float mapH) const
{
    sf::Vector2f p = mSprite.getPosition();
    return (p.x < -100 || p.x > mapW + 100 || p.y < -100 || p.y > mapH + 100);
}

sf::FloatRect Arrow::getHitbox() const
{
    sf::Vector2f p = mSprite.getPosition();
    return sf::FloatRect({p.x - 15.f, p.y - 15.f}, {30.f, 30.f});
}

sf::Vector2f Arrow::getPosition() const
{
    return mSprite.getPosition();
}