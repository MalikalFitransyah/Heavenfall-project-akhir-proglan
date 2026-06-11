#pragma once
#include <SFML/Graphics.hpp>

enum class ItemType { TerrorEye, BlasphemousMask, DevilWing };

class Item
{
public:
    Item(sf::Texture* itemTex, sf::Texture* spawnTex,
         sf::Vector2f position, ItemType type);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    bool isCollected() const;
    bool shouldRemove() const;
    ItemType getType() const;
    sf::FloatRect getHitbox() const;

    void collect();

private:
    sf::Sprite   mSprite;
    sf::Sprite   mSpawnEffect;
    sf::Clock    mAnimClock;
    sf::Clock    mSpawnClock;

    int          mFrame;
    int          mSpawnFrame;
    bool         mSpawnDone;
    bool         mCollected;

    ItemType     mType;

    // terroreye: 6 frames 64x64
    // blasphemousmask: 1 frame 64x64
    // devilwing: 1 frame 64x64
    static const int ITEM_W       = 64;
    static const int ITEM_H       = 64;
    static const int SPAWN_W      = 64;
    static const int SPAWN_H      = 64;
    static const int SPAWN_FRAMES = 15;
};