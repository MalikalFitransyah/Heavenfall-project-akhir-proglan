#pragma once
#include <SFML/Graphics.hpp>

enum class BossPhase { Idle, Moving, Charging, AttackPlus, AttackX, Dead, Despawning };

// Satu beam laser dari boss
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
    bool isDespawnDone() const; // true setelah animasi + 2 detik selesai

    // Cek apakah laser kena player
    bool checkLaserHit(sf::FloatRect playerHitbox);

    sf::FloatRect getHitbox()   const;
    sf::Vector2f  getPosition() const;

private:
    void startAttack();
    void updateMoving(float dt, sf::Vector2f playerCenter);
    void updateCharging(float dt);
    void updateAttack(float dt, sf::Vector2f playerCenter);
    void fireLasers(bool plusPattern); // true=+, false=X
    void updateLasers(float dt);
    void fireAllDirections();


    sf::Texture* mLaserTex;
    sf::Texture* mDespawnTex;

    sf::Sprite   mSprite;     // boss idle
    sf::Sprite   mAtkSprite;  // boss saat attack
    bool         mShowAtk;

    int          mHp;
    bool         mDead;

    BossPhase    mPhase;
    sf::Clock    mPhaseClock;
    sf::Clock    mAttackClock;  // timer 8 detik antar serangan
    sf::Clock    mChargeTimer;  // timer charge laser

    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
    sf::Clock    mHitClock;
    bool         mHitFlash;

    // Animasi boss attack (4 frame)
    int          mAtkFrame;
    sf::Clock    mAtkAnimClock;

    // Laser beams — max 4 beam per serangan
    std::vector<LaserBeam> mBeams;

    // Fase serangan: 0 = belum serang, 1 = + done, 2 = X done
    int          mAttackStage;

    // Laser animation state
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