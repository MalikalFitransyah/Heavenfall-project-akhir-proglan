#include "Player.hpp"
#include <cmath>
#include <algorithm>

Player::Player(sf::Texture& idle, sf::Texture& walkRight, sf::Texture& walkLeft,
               sf::Texture& arrowTex)
    : mArrowTex(arrowTex)
    , mIdleTex(idle)
    , mRightTex(walkRight)
    , mLeftTex(walkLeft)
    , mSprite(idle)
    , mFrame(0)
    , mIsCharging(false)
    , mChargeFrame(0)
    , mChargeArrow(arrowTex)
{
    mSprite.setTextureRect(sf::IntRect({0, 0}, {FRAME_W, FRAME_H}));
    mSprite.setScale({SCALE, SCALE});
    mSprite.setPosition({1280.f, 720.f});

    mChargeArrow.setTextureRect(sf::IntRect({0, 0}, {ARROW_W, ARROW_H}));
    mChargeArrow.setOrigin({ARROW_W / 2.f, ARROW_H / 2.f});
    mChargeArrow.setScale({4.f, 4.f});
}

void Player::startCharge(sf::Vector2f mousePos)
{
    if (mIsCharging) return;

    sf::Vector2f center = getCenter();
    sf::Vector2f dir    = mousePos - center;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) dir /= len;

    mChargeDir      = dir;
    mChargeSpawnPos = center + sf::Vector2f(60.f, 0.f);
    mIsCharging     = true;
    mChargeFrame    = 0;
    mChargeClock.restart();

    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
    mChargeArrow.setTextureRect(sf::IntRect({0, 0}, {ARROW_W, ARROW_H}));
    mChargeArrow.setRotation(sf::degrees(angle));
    mChargeArrow.setPosition(mChargeSpawnPos);
}

void Player::fireArrow(sf::Vector2f spawnPos, sf::Vector2f dir)
{
    bool piercing = mBuffs.hasBlasphemous;
    mArrows.emplace_back(mArrowTex, spawnPos, dir, piercing);
}

void Player::updateCharge()
{
    if (!mIsCharging) return;

    // TerrorEye buff: charge lebih cepat
    int maxCharge = mBuffs.hasTerrorEye ? CHARGE_FRAMES_FAST : CHARGE_FRAMES_NORMAL;
    float chargeSpeed = mBuffs.hasTerrorEye ? 0.02f : 0.05f;

    if (mChargeClock.getElapsedTime().asSeconds() > chargeSpeed)
    {
        mChargeFrame++;
        mChargeClock.restart();

        if (mChargeFrame >= maxCharge)
        {
            // DevilWing buff: 3 tembakan menyebar
            if (mBuffs.hasDevilWing)
            {
                // Tengah
                fireArrow(mChargeSpawnPos, mChargeDir);

                // Serong atas (+20 derajat)
                float baseAngle = std::atan2(mChargeDir.y, mChargeDir.x);
                float spread    = 20.f * 3.14159265f / 180.f;

                sf::Vector2f dirUp(
                    std::cos(baseAngle - spread),
                    std::sin(baseAngle - spread)
                );
                sf::Vector2f dirDown(
                    std::cos(baseAngle + spread),
                    std::sin(baseAngle + spread)
                );
                fireArrow(mChargeSpawnPos, dirUp);
                fireArrow(mChargeSpawnPos, dirDown);
            }
            else
            {
                fireArrow(mChargeSpawnPos, mChargeDir);
            }

            mIsCharging  = false;
            mChargeFrame = 0;
        }
        else
        {
            // Clamp frame ke animasi charge yang tersedia (max 8)
            int displayFrame = (mChargeFrame * 8) / maxCharge;
            mChargeArrow.setTextureRect(sf::IntRect(
                {displayFrame * ARROW_W, 0}, {ARROW_W, ARROW_H}
            ));
        }
    }

    mChargeSpawnPos = getCenter() + sf::Vector2f(60.f, 0.f);
    mChargeArrow.setPosition(mChargeSpawnPos);
}

void Player::update(float dt, float mapW, float mapH)
{
    sf::Vector2f move(0.f, 0.f);
    bool moving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    { move.x -= SPEED; mSprite.setTexture(mLeftTex);  moving = true; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    { move.x += SPEED; mSprite.setTexture(mRightTex); moving = true; }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) move.y -= SPEED;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) move.y += SPEED;

    if (!moving) mSprite.setTexture(mIdleTex);

    mSprite.move(move * dt);

    // Map boundary
    sf::Vector2f pos = mSprite.getPosition();
    float pW = FRAME_W * SCALE;
    float pH = FRAME_H * SCALE;
    if (pos.x < 0.f)        pos.x = 0.f;
    if (pos.y < 0.f)        pos.y = 0.f;
    if (pos.x + pW > mapW)  pos.x = mapW - pW;
    if (pos.y + pH > mapH)  pos.y = mapH - pH;
    mSprite.setPosition(pos);

    // Animasi player
    if (mAnimClock.getElapsedTime().asSeconds() > 0.15f)
    {
        mFrame = (mFrame + 1) % ANIM_FRAMES;
        mSprite.setTextureRect(sf::IntRect(
            {mFrame * FRAME_W, 0}, {FRAME_W, FRAME_H}
        ));
        mAnimClock.restart();
    }

    for (auto& a : mArrows) a.update(dt);

    mArrows.erase(
        std::remove_if(mArrows.begin(), mArrows.end(),
            [&](const Arrow& a){ return a.isOutOfBounds(mapW, mapH); }),
        mArrows.end()
    );

    updateCharge();
}

void Player::draw(sf::RenderWindow& window)
{
    for (auto& a : mArrows) a.draw(window);
    if (mIsCharging) window.draw(mChargeArrow);
    window.draw(mSprite);
}

void Player::applyBuff(PlayerBuffs& buffs) { mBuffs = buffs; }
PlayerBuffs& Player::getBuffs()            { return mBuffs; }

std::vector<Arrow>& Player::getArrows()    { return mArrows; }

sf::Vector2f Player::getCenter() const
{
    return mSprite.getPosition() + sf::Vector2f(FRAME_W * SCALE / 2.f, FRAME_H * SCALE / 2.f);
}

sf::FloatRect Player::getHitbox() const
{
    sf::Vector2f p = getCenter();
    return sf::FloatRect({p.x - 30.f, p.y - 30.f}, {60.f, 60.f});
}

bool Player::isCharging() const { return mIsCharging; }