#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Arrow.hpp"
#include "PlayerBuffs.hpp"

class Player
{
public:
    Player(sf::Texture& idle, sf::Texture& walkRight, sf::Texture& walkLeft,
           sf::Texture& arrowTex);

    void startCharge(sf::Vector2f mousePos);
    void update(float dt, float mapW, float mapH);
    void draw(sf::RenderWindow& window);

    std::vector<Arrow>& getArrows();
    sf::Vector2f        getCenter() const;
    sf::FloatRect       getHitbox() const;
    bool                isCharging() const;

    void applyBuff(PlayerBuffs& buffs);
    PlayerBuffs& getBuffs();

private:
    void updateCharge();
    void fireArrow(sf::Vector2f spawnPos, sf::Vector2f dir);

    sf::Texture& mArrowTex;
    sf::Texture& mIdleTex;
    sf::Texture& mRightTex;
    sf::Texture& mLeftTex;

    sf::Sprite   mSprite;
    sf::Clock    mAnimClock;
    int          mFrame;

    bool         mIsCharging;
    int          mChargeFrame;
    sf::Clock    mChargeClock;
    sf::Vector2f mChargeDir;
    sf::Vector2f mChargeSpawnPos;
    sf::Sprite   mChargeArrow;

    std::vector<Arrow> mArrows;
    PlayerBuffs        mBuffs;

    static const int   FRAME_W       = 48;
    static const int   FRAME_H       = 48;
    static const int   ANIM_FRAMES   = 4;
    static const int   ARROW_W       = 64;
    static const int   ARROW_H       = 64;
    static const int   LOOP_START    = 8;
    static constexpr float SCALE     = 4.f;
    static constexpr float SPEED     = 500.f;
    static const int   CHARGE_FRAMES_NORMAL = 8;
    static const int   CHARGE_FRAMES_FAST   = 3; // TerrorEye buff
};