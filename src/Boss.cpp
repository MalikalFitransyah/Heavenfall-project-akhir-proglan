#include "Boss.hpp"
#include <cmath>
#include <vector>

static const float PI = 3.14159265f;

Boss::Boss(sf::Texture* bossTex, sf::Texture* atkTex, sf::Texture* laserTex,
           sf::Texture* despawnTex,
           sf::Vector2f spawnPos)
    : mLaserTex(laserTex)
    , mDespawnTex(despawnTex)
    , mSprite(*bossTex)
    , mAtkSprite(*atkTex)
    , mShowAtk(false)
    , mHp(300)
    , mDead(false)
    , mPhase(BossPhase::Moving)
    , mPosition(spawnPos)
    , mVelocity(0.f, 0.f)
    , mAtkFrame(0)
    , mAttackStage(0)
    , mLaserState(LaserState::Charge)
    , mLaserFrame(0)
    , mLaserLooping(false)
    , mHitFlash(false)
    , mDespawnFrame(0)
    , mDespawnAnimDone(false)
    , mDespawnDone(false)
{
    // Boss idle sprite
    mSprite.setTextureRect(sf::IntRect({0,0},{BOSS_W,BOSS_H}));
    mSprite.setOrigin({BOSS_W/2.f, BOSS_H/2.f});
    mSprite.setScale({3.f, 3.f});
    mSprite.setPosition(mPosition);

    // Boss atk sprite
    mAtkSprite.setTextureRect(sf::IntRect({0,0},{ATK_W,ATK_H}));
    mAtkSprite.setOrigin({ATK_W/2.f, ATK_H/2.f});
    mAtkSprite.setScale({3.f, 3.f});
    mAtkSprite.setPosition(mPosition);

    mAttackClock.restart();
}

void Boss::hit()
{
    if (mDead) return;
    mHp--;
    mSprite.setColor(sf::Color(255, 80, 80));
    mAtkSprite.setColor(sf::Color(255, 80, 80));
    if (mHp <= 0)
    {
        mDead  = true;
        mPhase = BossPhase::Despawning;
        mDespawnFrame     = 0;
        mDespawnAnimDone  = false;
        mDespawnDone      = false;
        mBeams.clear();
        mDespawnAnimClock.restart();
    }
}

bool Boss::isDead()        const { return mDead; }
bool Boss::isActive()      const { return !mDead; }
bool Boss::isDespawnDone() const { return mDespawnDone; }

void Boss::startAttack()
{
    mPhase       = BossPhase::Charging;
    mShowAtk     = true;
    mAtkFrame    = 0;
    mAttackStage = 0;
    mLaserFrame  = 0;
    mLaserState  = LaserState::Charge;
    mLaserLooping = false;
    mBeams.clear();
    mAtkAnimClock.restart();
    mChargeTimer.restart();
    mLaserStateClock.restart();
}

void Boss::updateMoving(float dt, sf::Vector2f playerCenter)
{
    sf::Vector2f dir = playerCenter - mPosition;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    if (len != 0) dir /= len;
    mVelocity = dir * SPEED;
    mPosition += mVelocity * dt;

    // Cek waktu 5 detik untuk serangan
    if (mAttackClock.getElapsedTime().asSeconds() >= 5.f)
    {
        mAttackClock.restart();
        startAttack();
    }
}

void Boss::updateCharging(float dt)
{
    // Animasi boss atk frame 1-3 (charge)
    if (mAtkAnimClock.getElapsedTime().asSeconds() > 0.03f)
    {
        if (mAtkFrame < 3) mAtkFrame++;
        mAtkSprite.setTextureRect(sf::IntRect({mAtkFrame*ATK_W,0},{ATK_W,ATK_H}));
        mAtkAnimClock.restart();
    }

    // tembak setelah 1 detik charge
    if (mChargeTimer.getElapsedTime().asSeconds() >= 1.0f)
    {
        // Tentukan pola: stage 0 = +, stage 1 = X
        bool plusPattern = (mAttackStage % 2 == 0);
        fireLasers(plusPattern);
        mLaserFrame     = 0;
        mLaserState     = LaserState::Charge;
        mLaserLooping   = false;
        mLaserStateClock.restart();
        mPhase          = BossPhase::AttackPlus;
    }
}

void Boss::fireLasers(bool plusPattern)
{
    mBeams.clear();

    // 4 arah
    std::vector<sf::Vector2f> dirs;
    if (plusPattern)
    {
        dirs = {
            { 1.f,  0.f},
            {-1.f,  0.f}, 
            { 0.f,  1.f}, 
            { 0.f, -1.f},
        };
    }
    else
    {
        float d = 1.f / std::sqrt(2.f);
        dirs = {
            { d,  d}, 
            {-d,  d}, 
            { d, -d}, 
            {-d, -d},
        };
    }

    for (auto& dir : dirs)
    {
        LaserBeam beam(*mLaserTex);
        beam.direction = dir;
        beam.active    = true;
        beam.frame     = 0;
        beam.canDamage = false;
        beam.didDamage = false;

        // Posisi & rotasi laser
        float angle = std::atan2(dir.y, dir.x) * 180.f / PI;
        beam.sprite.setOrigin({0.f, LASER_FH / 2.f});
        beam.sprite.setScale({-(LASER_LEN / LASER_FW), 12.f});
        beam.sprite.setRotation(sf::degrees(angle));
        beam.sprite.setTextureRect(sf::IntRect({0,0},{LASER_FW,LASER_FH}));
        beam.sprite.setPosition(mPosition);

        mBeams.push_back(std::move(beam));
    }
}

void Boss::fireAllDirections()
{
    mBeams.clear();
    float PI = 3.14159265f;
    // 8 arah: 0, 45, 90, 135, 180, 225, 270, 315 derajat
    for (int i = 0; i < 8; i++)
    {
        float angle = i * 45.f * PI / 180.f;
        sf::Vector2f dir(std::cos(angle), std::sin(angle));

        LaserBeam beam(*mLaserTex);
        beam.direction = dir;
        beam.active    = true;
        beam.frame     = 0;
        beam.canDamage = false;
        beam.didDamage = false;

        float deg = i * 45.f;
        beam.sprite.setOrigin({0.f, LASER_FH / 2.f});
        beam.sprite.setScale({-(LASER_LEN / LASER_FW), 12.f});
        beam.sprite.setRotation(sf::degrees(deg));
        beam.sprite.setTextureRect(sf::IntRect({0,0},{LASER_FW,LASER_FH}));
        beam.sprite.setPosition(mPosition);

        mBeams.push_back(std::move(beam));
    }
}

void Boss::updateAttack(float dt, sf::Vector2f playerCenter)
{
    updateLasers(dt);

    // Selesai satu pola laser
    if (mLaserState == LaserState::Done)
    {
        mAttackStage++;
        if (mAttackStage == 1)
{
    fireLasers(false);
    mLaserFrame     = 0;
    mLaserState     = LaserState::Charge;
    mLaserLooping   = false;
    mLaserStateClock.restart();
}
else if (mAttackStage == 2)
{
    fireAllDirections();
    mLaserFrame     = 0;
    mLaserState     = LaserState::Charge;
    mLaserLooping   = false;
    mLaserStateClock.restart();
}
else
{
    // Selesai semua serangan
    mBeams.clear();
    mShowAtk     = false;
    mAtkFrame    = 0;
    mPhase       = BossPhase::Moving;
    mAttackClock.restart();
}
    }
}

void Boss::updateLasers(float dt)
{
    float elapsed = mLaserStateClock.getElapsedTime().asSeconds();

    if (mLaserState == LaserState::Charge)
    {
        // Frame 0-10: charge lambat (0.05s per frame)
        if (elapsed > 0.01f)
        {
            mLaserFrame++;
            mLaserStateClock.restart();
            if (mLaserFrame >= 11)
            {
                mLaserFrame = 10;
                mLaserState = LaserState::Hold;
                mLaserStateClock.restart();
            }
        }
    }
    else if (mLaserState == LaserState::Hold)
    {
        // Stuck di frame 10 selama 1.5 detik
        mLaserFrame = 10;
        if (elapsed >= 0.5f)
        {
            mLaserState = LaserState::Fire;
            mLaserStateClock.restart();
           
        }
    }
    else if (mLaserState == LaserState::Fire)
    {
        float elapsed2 = mLaserStateClock.getElapsedTime().asSeconds();

        float interval;
        if      (mLaserFrame < 18)  interval = 0.04f;
        else if (mLaserFrame < 20)  interval = 0.2f;
        else                         interval = 0.04f;

        if (elapsed2 > interval)
        {
            mLaserFrame++;
            mLaserStateClock.restart();
            if (mLaserFrame >= 24)
            {
                mLaserFrame = 23;
                mLaserState = LaserState::Done;
            }
        }

        // Frame 11-19 bisa damage player
        for (auto& beam : mBeams)
            beam.canDamage = (mLaserFrame >= 11 && mLaserFrame < 20);
    }

    // Update texture semua beam
    for (auto& beam : mBeams)
    {
        beam.sprite.setTextureRect(sf::IntRect(
            {mLaserFrame * LASER_FW, 0}, {LASER_FW, LASER_FH}
        ));
        beam.sprite.setPosition(mPosition);
    }
}

bool Boss::checkLaserHit(sf::FloatRect playerHitbox)
{
    for (auto& beam : mBeams)
    {
        if (!beam.canDamage || beam.didDamage) continue;

        // Cek apakah player berada di jalur laser
        // Hitung proyeksi player ke garis laser
        sf::Vector2f toPlayer = sf::Vector2f(
            playerHitbox.position.x + playerHitbox.size.x / 2.f,
            playerHitbox.position.y + playerHitbox.size.y / 2.f
        ) - mPosition;

        float dot = toPlayer.x * beam.direction.x + toPlayer.y * beam.direction.y;
        if (dot < 0) continue; // di belakang boss

        // Jarak tegak lurus dari garis laser
        float cross = std::abs(toPlayer.x * beam.direction.y - toPlayer.y * beam.direction.x);

        if (cross < 60.f && dot < LASER_LEN)
        {
            beam.didDamage = true;
            return true;
        }
    }
    return false;
}

void Boss::update(float dt, sf::Vector2f playerCenter)
{
    // Handle despawn animasi
    if (mPhase == BossPhase::Despawning)
    {
        if (!mDespawnAnimDone)
        {
            if (mDespawnAnimClock.getElapsedTime().asSeconds() > 0.1f)
            {
                mDespawnFrame++;
                mDespawnAnimClock.restart();
                if (mDespawnFrame >= DESPAWN_FRAMES)
                {
                    mDespawnFrame    = DESPAWN_FRAMES - 1;
                    mDespawnAnimDone = true;
                    mDespawnWaitClock.restart();
                }
            }
        }
        else
        {
            // Tunggu 2 detik setelah animasi selesai
            if (mDespawnWaitClock.getElapsedTime().asSeconds() >= 2.f)
                mDespawnDone = true;
        }
        return;
    }

    if (mDead) return;

    // Reset flash
    if (mSprite.getColor() != sf::Color::White)
    {
        mSprite.setColor(sf::Color::White);
        mAtkSprite.setColor(sf::Color::White);
    }

// Reset flash setelah 0.15 detik
if (mHitFlash && mHitClock.getElapsedTime().asSeconds() > 0.15f)
{
    mHitFlash = false;
    mSprite.setColor(sf::Color::White);
    mAtkSprite.setColor(sf::Color::White);
}

// Reset justFired setelah 1 frame
    if      (mPhase == BossPhase::Moving)   updateMoving(dt, playerCenter);
    else if (mPhase == BossPhase::Charging) updateCharging(dt);
    else if (mPhase == BossPhase::AttackPlus || mPhase == BossPhase::AttackX)
        updateAttack(dt, playerCenter);

    mSprite.setPosition(mPosition);
    mAtkSprite.setPosition(mPosition);
}

void Boss::draw(sf::RenderWindow& window)
{
    // Despawn animasi
    if (mPhase == BossPhase::Despawning && !mDespawnAnimDone)
    {
        sf::Sprite despawnSprite(*mDespawnTex);
        despawnSprite.setTextureRect(sf::IntRect(
            sf::Vector2i(mDespawnFrame * DESPAWN_FW, 0),
            sf::Vector2i(DESPAWN_FW, DESPAWN_FH)
        ));
        despawnSprite.setOrigin(sf::Vector2f(DESPAWN_FW / 2.f, DESPAWN_FH / 2.f));
        despawnSprite.setScale(sf::Vector2f(3.f, 3.f));
        despawnSprite.setPosition(mPosition);
        window.draw(despawnSprite);
        return;
    }

    if (mDead) return;

    // Gambar laser di belakang boss
    for (auto& beam : mBeams)
        if (beam.active) window.draw(beam.sprite);

    // Gambar boss
    if (mShowAtk) window.draw(mAtkSprite);
    else          window.draw(mSprite);
}

sf::FloatRect Boss::getHitbox() const
{
    return sf::FloatRect(
        {mPosition.x - 80.f, mPosition.y - 80.f},
        {160.f, 160.f}
    );
}

sf::Vector2f Boss::getPosition() const { return mPosition; }
bool Boss::isInvincible() const { return mHitFlash; }