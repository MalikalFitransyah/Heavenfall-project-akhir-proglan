#include "Enemy.hpp"
#include <cmath>

Enemy::Enemy(sf::Texture* walkTex, sf::Texture* despawnTex,
             sf::Vector2f position, sf::Vector2f direction)
    : mDespawnTex(despawnTex)
    , mSprite(*walkTex)
    , mFrame(0)
    , mDying(false)
    , mDead(false)
    , mDyingFrame(0)
{
    mSprite.setTextureRect(sf::IntRect({0, 0}, {FRAME_W, FRAME_H}));
    mSprite.setOrigin({FRAME_W / 2.f, FRAME_H / 2.f});
    mSprite.setScale(sf::Vector2f(3.f, 3.f));
    mSprite.setPosition(position);

    mVelocity = direction * SPEED;
}

void Enemy::update(float dt, sf::Vector2f playerCenter)
{
    if (mDying)
    {
        if (mDyingClock.getElapsedTime().asSeconds() > 0.10f)
        {
            mDyingFrame++;
            mDyingClock.restart();

            if (mDyingFrame >= DIE_FRAMES)
            {
                mDead = true;
                return;
            }

            mSprite.setTextureRect(sf::IntRect(
                {mDyingFrame * FRAME_W, 0},
                {FRAME_W, FRAME_H}
            ));
        }
        return;
    }

    // Update arah ke player setiap frame
    sf::Vector2f dir = playerCenter - mSprite.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) dir /= len;
    mVelocity = dir * SPEED;

    mSprite.move(mVelocity * dt);

    // Animasi jalan
    if (mAnimClock.getElapsedTime().asSeconds() > 0.15f)
    {
        mFrame = (mFrame + 1) % WALK_FRAMES;
        mSprite.setTextureRect(sf::IntRect(
            {mFrame * FRAME_W, 0},
            {FRAME_W, FRAME_H}
        ));
        mAnimClock.restart();
    }
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(mSprite);
}

void Enemy::triggerDeath()
{
    mDying      = true;
    mDyingFrame = 0;
    mSprite.setTexture(*mDespawnTex);
    mSprite.setTextureRect(sf::IntRect({0, 0}, {FRAME_W, FRAME_H}));
    mVelocity = {0.f, 0.f};
    mDyingClock.restart();
}

bool Enemy::isDying() const { return mDying; }
bool Enemy::isDead()  const { return mDead;  }

sf::FloatRect Enemy::getHitbox() const
{
    sf::Vector2f p = mSprite.getPosition();
    return sf::FloatRect({p.x - 55.f, p.y - 55.f}, {110.f, 110.f});
}