#pragma once
#include <SFML/Graphics.hpp>

enum class BossPhase { Idle, Moving, Charging, AttackPlus, AttackX, Dead, Despawning };

struct LaserBeam
{
    sf::Sprite   sprite;
    sf::Vector2f direction;
    float        length;
    bool         active;
    int          frame;
    sf::Clock    animClock;

    // Hitbox aktif hanya frame 12-20
    bool         canDamage;
    bool         didDamage; // sudah kena player di siklus ini

    LaserBeam(sf::Texture& tex) : sprite(tex), length(0.f),
        active(false), frame(0), canDamage(false), didDamage(false) {}
};

class Boss
{
public:
    Boss(sf::Texture* bossTex, sf::Texture* atkTex, sf::Texture* laserTex,
         sf::Texture* despawnTex,
         sf::Vector2f spawnPos);

    void update(float dt, sf::Vector2f playerCenter);
    void draw(sf::RenderWindow& window);

    void hit();
    bool isDead()   const;
    bool isActive() const;
    bool isInvincible() const;
    bool isDespawnDone() const;

    // Cek apakah laser kena player
    bool checkLaserHit(sf::FloatRect playerHitbox);

    sf::FloatRect getHitbox()   const;
    sf::Vector2f  getPosition() const;

private:
    void startAttack();
    void updateMoving(float dt, sf::Vector2f playerCenter);
    void updateCharging(float dt);
    void updateAttack(float dt, sf::Vector2f playerCenter);
    void fireLasers(bool plusPattern); 
    void updateLasers(float dt);
    void fireAllDirections();


    sf::Texture* mLaserTex;
    sf::Texture* mDespawnTex;

    sf::Sprite   mSprite;     
    sf::Sprite   mAtkSprite; 
    bool         mShowAtk;

    int          mHp;
    bool         mDead;

    BossPhase    mPhase;
    sf::Clock    mPhaseClock;
    sf::Clock    mAttackClock; 
    sf::Clock    mChargeTimer;

    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
    sf::Clock    mHitClock;
    bool         mHitFlash;

    int          mAtkFrame;
    sf::Clock    mAtkAnimClock;

    std::vector<LaserBeam> mBeams;

    int          mAttackStage;

    enum class LaserState { Charge, Hold, Fire, Done };
    LaserState   mLaserState;
    sf::Clock    mLaserStateClock;
    int          mLaserFrame;
    bool         mLaserLooping;
    sf::Clock    mLaserLoopClock;

    // Despawn animasi
    int          mDespawnFrame;
    sf::Clock    mDespawnAnimClock;
    sf::Clock    mDespawnWaitClock;
    bool         mDespawnAnimDone;
    bool         mDespawnDone;

    static const int BOSS_W      = 129;
    static const int BOSS_H      = 129;
    static const int ATK_W       = 129;
    static const int ATK_H       = 129;
    static const int LASER_FW    = 64;
    static const int LASER_FH    = 32;
    static const int LASER_FRAMES = 24;
    static const int DESPAWN_FRAMES = 11;
    static const int DESPAWN_FW  = 129;
    static const int DESPAWN_FH  = 129;
    static constexpr float SPEED = 120.f;
    static constexpr float LASER_LEN = 2800.f; // panjang laser
};