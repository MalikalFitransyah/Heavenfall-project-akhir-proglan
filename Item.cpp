#include "Item.hpp"

Item::Item(sf::Texture* itemTex, sf::Texture* spawnTex,
           sf::Vector2f position, ItemType type)
    : mSprite(*itemTex)
    , mSpawnEffect(*spawnTex)
    , mFrame(0)
    , mSpawnFrame(0)
    , mSpawnDone(false)
    , mCollected(false)
    , mType(type)
{
    mSprite.setTextureRect(sf::IntRect({0, 0}, {ITEM_W, ITEM_H}));
    mSprite.setOrigin({ITEM_W / 2.f, ITEM_H / 2.f});
    mSprite.setScale({2.f, 2.f}); // lebih kecil dari sebelumnya (3.f)
    mSprite.setPosition(position);

    mSpawnEffect.setTextureRect(sf::IntRect({0, 0}, {SPAWN_W, SPAWN_H}));
    mSpawnEffect.setOrigin({SPAWN_W / 2.f, SPAWN_H / 2.f});
    mSpawnEffect.setScale({3.f, 3.f});
    mSpawnEffect.setPosition(position);
}

void Item::update(float dt)
{
    if (mCollected) return;

    if (!mSpawnDone)
    {
        if (mSpawnClock.getElapsedTime().asSeconds() > 0.08f)
        {
            mSpawnFrame++;
            mSpawnClock.restart();
            if (mSpawnFrame >= SPAWN_FRAMES)
            {
                mSpawnDone  = true;
                mSpawnFrame = SPAWN_FRAMES - 1;
            }
            mSpawnEffect.setTextureRect(sf::IntRect(
                {mSpawnFrame * SPAWN_W, 0}, {SPAWN_W, SPAWN_H}
            ));
        }
    }

    int maxFrames = (mType == ItemType::TerrorEye) ? 6 : 1;
    if (maxFrames > 1 && mAnimClock.getElapsedTime().asSeconds() > 0.12f)
    {
        mFrame = (mFrame + 1) % maxFrames;
        mSprite.setTextureRect(sf::IntRect(
            {mFrame * ITEM_W, 0}, {ITEM_W, ITEM_H}
        ));
        mAnimClock.restart();
    }
}

void Item::draw(sf::RenderWindow& window)
{
    if (mCollected) return;
    if (!mSpawnDone) window.draw(mSpawnEffect);
    window.draw(mSprite);
}

void     Item::collect()          { mCollected = true; }
bool     Item::isCollected() const { return mCollected; }
bool     Item::shouldRemove() const { return mCollected; }
ItemType Item::getType()     const { return mType; }

sf::FloatRect Item::getHitbox() const
{
    sf::Vector2f p = mSprite.getPosition();
    return sf::FloatRect({p.x - 40.f, p.y - 40.f}, {80.f, 80.f});
}