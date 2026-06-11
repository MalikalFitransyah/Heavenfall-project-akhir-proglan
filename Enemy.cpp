#include "Enemy.hpp"
#include <cmath>

Enemy::Enemy(sf::Texture* walkTex, sf::Texture* despawnTex,
             sf::Vector2f position, sf::Vector2f direction, int maxHp)
    : mDespawnTex(despawnTex)
    , mSprite(*walkTex)
    , mHp(maxHp)
    , mFrame(0)
    , mDying(false)
    , mDead(false)
    , mDyingFrame(0)
    , mHitFlash(false)
{
    mSprite.setTextureRect(sf::IntRect({0, 0}, {FRAME_W, FRAME_H}));
    mSprite.setOrigin({FRAME_W / 2.f, FRAME_H / 2.f});
    mSprite.setScale(sf::Vector2f(3.f, 3.f));
    mSprite.setPosition(position);
    mVelocity = direction * SPEED;
}

void Enemy::hit()
{
    if (mDying) return;
    mHp--;
    if (mHp <= 0)
    {
        triggerDeath();
    }
    else
    {
        // Flash merah saat kena hit
        mHitFlash = true;
        mHitClock.restart();
        mSprite.setColor(sf::Color(255, 80, 80));
    }
}

void Enemy::update(float dt, sf::Vector2f playerCenter)
{
    // Reset flash setelah 0.1s
    if (mHitFlash && mHitClock.getElapsedTime().asSeconds() > 0.1f)
    {
        mHitFlash = false;
        mSprite.setColor(sf::Color::White);
    }

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
                {mDyingFrame * FRAME_W, 0}, {FRAME_W, FRAME_H}
            ));
        }
        return;
    }

    sf::Vector2f dir = playerCenter - mSprite.getPosition();
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) dir /= len;
    mVelocity = dir * SPEED;
    mSprite.move(mVelocity * dt);

    if (mAnimClock.getElapsedTime().asSeconds() > 0.15f)
    {
        mFrame = (mFrame + 1) % WALK_FRAMES;
        mSprite.setTextureRect(sf::IntRect(
            {mFrame * FRAME_W, 0}, {FRAME_W, FRAME_H}
        ));
        mAnimClock.restart();
    }
}

void Enemy::triggerDeath()
{
    mDying      = true;
    mDyingFrame = 0;
    mSprite.setTexture(*mDespawnTex);
    mSprite.setTextureRect(sf::IntRect({0, 0}, {FRAME_W, FRAME_H}));
    mSprite.setColor(sf::Color::White);
    mVelocity = {0.f, 0.f};
    mDyingClock.restart();
}

void Enemy::draw(sf::RenderWindow& window) { window.draw(mSprite); }
bool Enemy::isDying()    const { return mDying; }
bool Enemy::isDead()     const { return mDead;  }

sf::FloatRect Enemy::getHitbox() const
{
    sf::Vector2f p = mSprite.getPosition();
    return sf::FloatRect({p.x - 55.f, p.y - 55.f}, {110.f, 110.f});
}

sf::Vector2f Enemy::getPosition() const { return mSprite.getPosition(); }

bool Enemy::isInvincible() const
{
    // Invincible selama 0.3 detik setelah kena hit
    return mHitFlash;
}