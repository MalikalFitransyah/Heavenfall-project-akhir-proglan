#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({2560, 1440}), "Heaven Fall");

    sf::Texture idleTexture, rightTexture, leftTexture;

    if (!idleTexture.loadFromFile("assets/player_idle.png"))
    { std::cout << "GAGAL LOAD: assets/player_idle.png\n"; return -1; }
    if (!rightTexture.loadFromFile("assets/player_walk_right.png"))
    { std::cout << "GAGAL LOAD: assets/player_walk_right.png\n"; return -1; }
    if (!leftTexture.loadFromFile("assets/player_walk_left.png"))
    { std::cout << "GAGAL LOAD: assets/player_walk_left.png\n"; return -1; }

    const int frameWidth  = 48;
    const int frameHeight = 48;

    sf::Sprite player(idleTexture);
    player.setTextureRect(sf::IntRect({0,0},{frameWidth,frameHeight}));

    float scale = 4.f;
    player.setScale({scale, scale});
    player.setPosition({1280.f, 720.f});

    float speed = 500.f;

    const float MAP_W = 2560.f;
    const float MAP_H = 1440.f;

    // ================= BACKGROUND =================
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("assets/background.png"))
    { std::cout << "GAGAL LOAD: assets/background.png\n"; return -1; }

    sf::Sprite bg(bgTexture);
    bg.setScale({MAP_W / bgTexture.getSize().x, MAP_H / bgTexture.getSize().y});

    // ================= ARROW =================
    sf::Texture arrowTexture;
    if (!arrowTexture.loadFromFile("assets/arrow.png"))
    { std::cout << "GAGAL LOAD: assets/arrow.png\n"; return -1; }

    const int aW = 64, aH = 64;
    const int CHARGE_FRAMES = 8;
    const int LOOP_START    = 8;
    const int LOOP_END      = 12;

    std::vector<sf::Sprite>   arrows;
    std::vector<sf::Vector2f> velocities;
    std::vector<int>          arrowFrames;
    std::vector<sf::Clock>    arrowAnimClocks;

    sf::Clock deltaClock;
    sf::Clock playerAnimClock;
    int playerFrame = 0;

    // ================= CHARGING =================
    bool         isCharging    = false;
    int          chargeFrame   = 0;
    sf::Clock    chargeClock;
    sf::Vector2f chargeDir;
    sf::Vector2f chargeSpawnPos;

    sf::Sprite chargeArrow(arrowTexture);
    chargeArrow.setTextureRect(sf::IntRect({0,0},{aW,aH}));
    chargeArrow.setOrigin({aW / 2.f, aH / 2.f});
    chargeArrow.setScale({4.f, 4.f});

    // ================= ENEMY =================
    sf::Texture enemyTexture;
    if (!enemyTexture.loadFromFile("assets/enemy.png"))
    { std::cout << "GAGAL LOAD: assets/enemy.png\n"; return -1; }

    sf::Texture despawnTexture;
    if (!despawnTexture.loadFromFile("assets/miaracle.png"))
    { std::cout << "GAGAL LOAD: assets/miaracle.png\n"; return -1; }

    const int eW = 64, eH = 64;
    const float enemySpeed = 200.f;

    struct Enemy
    {
        sf::Sprite   sprite;
        sf::Vector2f velocity;
        int          frame;
        sf::Clock    animClock;
        bool         dying      = false;
        int          dyingFrame = 0;
        sf::Clock    dyingClock;

        Enemy(sf::Texture& tex) : sprite(tex), frame(0)
        {
            sprite.setTextureRect(sf::IntRect({0,0},{64,64}));
            sprite.setOrigin({64 / 2.f, 64 / 2.f});
            sprite.setScale(sf::Vector2f(3.f, 3.f));
        }
    };

    std::vector<Enemy> enemies;

    sf::Clock spawnClock;
    float spawnInterval = 2.0f;

    // ================= FONT & GAME OVER UI =================
    sf::Font font;
    if (!font.openFromFile("assets/font.ttf"))
    { std::cout << "GAGAL LOAD: assets/font.ttf\n"; return -1; }

    bool gameOver   = false;
    bool gameActive = true;

    sf::RectangleShape overlay({MAP_W, MAP_H});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    sf::Text gameOverText(font, "GAME OVER", 120);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setStyle(sf::Text::Bold);
    sf::FloatRect goB = gameOverText.getLocalBounds();
    gameOverText.setOrigin({goB.size.x / 2.f, goB.size.y / 2.f});
    gameOverText.setPosition({MAP_W / 2.f, MAP_H / 2.f - 160.f});

    sf::RectangleShape btnRestart({400.f, 80.f});
    btnRestart.setFillColor(sf::Color(40, 40, 40));
    btnRestart.setOutlineColor(sf::Color::White);
    btnRestart.setOutlineThickness(3.f);
    btnRestart.setOrigin({200.f, 40.f});
    btnRestart.setPosition({MAP_W / 2.f, MAP_H / 2.f + 40.f});

    sf::Text txtRestart(font, "RESTART", 48);
    txtRestart.setFillColor(sf::Color::White);
    sf::FloatRect rb = txtRestart.getLocalBounds();
    txtRestart.setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
    txtRestart.setPosition({MAP_W / 2.f, MAP_H / 2.f + 40.f});

    sf::RectangleShape btnQuit({400.f, 80.f});
    btnQuit.setFillColor(sf::Color(40, 40, 40));
    btnQuit.setOutlineColor(sf::Color::White);
    btnQuit.setOutlineThickness(3.f);
    btnQuit.setOrigin({200.f, 40.f});
    btnQuit.setPosition({MAP_W / 2.f, MAP_H / 2.f + 160.f});

    sf::Text txtQuit(font, "QUIT", 48);
    txtQuit.setFillColor(sf::Color::White);
    sf::FloatRect qb = txtQuit.getLocalBounds();
    txtQuit.setOrigin({qb.size.x / 2.f, qb.size.y / 2.f});
    txtQuit.setPosition({MAP_W / 2.f, MAP_H / 2.f + 160.f});

    auto resetGame = [&]()
    {
        player.setPosition({1280.f, 720.f});
        arrows.clear();
        velocities.clear();
        arrowFrames.clear();
        arrowAnimClocks.clear();
        enemies.clear();
        isCharging    = false;
        chargeFrame   = 0;
        gameOver      = false;
        gameActive    = true;
        spawnInterval = 2.0f;
        spawnClock.restart();
    };

    // ================= LOOP =================
    while (window.isOpen())
    {
        float dt = deltaClock.restart().asSeconds();

        sf::Vector2i mousePix = sf::Mouse::getPosition(window);
        sf::Vector2f mouse    = window.mapPixelToCoords(mousePix);

        // ================= EVENT =================
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();
                if (mb && mb->button == sf::Mouse::Button::Left)
                {
                    if (gameOver)
                    {
                        if (btnRestart.getGlobalBounds().contains(mouse))
                            resetGame();
                        else if (btnQuit.getGlobalBounds().contains(mouse))
                            window.close();
                    }
                    else if (gameActive && !isCharging)
                    {
                        sf::Vector2f playerCenter = player.getPosition()
                            + sf::Vector2f(frameWidth * scale / 2.f, frameHeight * scale / 2.f);

                        sf::Vector2f dir = mouse - playerCenter;
                        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                        if (len != 0) dir /= len;

                        chargeDir      = dir;
                        chargeSpawnPos = playerCenter + sf::Vector2f(60.f, 0.f);

                        isCharging  = true;
                        chargeFrame = 0;
                        chargeClock.restart();

                        float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
                        chargeArrow.setTextureRect(sf::IntRect({0,0},{aW,aH}));
                        chargeArrow.setRotation(sf::degrees(angle));
                        chargeArrow.setPosition(chargeSpawnPos);
                    }
                }
            }

            if (event->is<sf::Event::MouseMoved>())
            {
                btnRestart.setFillColor(btnRestart.getGlobalBounds().contains(mouse)
                    ? sf::Color(80, 80, 80) : sf::Color(40, 40, 40));
                btnQuit.setFillColor(btnQuit.getGlobalBounds().contains(mouse)
                    ? sf::Color(80, 80, 80) : sf::Color(40, 40, 40));
            }
        }

        if (gameActive && !gameOver)
        {
            // ================= CHARGING UPDATE =================
            if (isCharging)
            {
                if (chargeClock.getElapsedTime().asSeconds() > 0.05f)
                {
                    chargeFrame++;
                    chargeClock.restart();

                    if (chargeFrame >= CHARGE_FRAMES)
                    {
                        sf::Sprite arrow(arrowTexture);
                        arrow.setOrigin({aW / 2.f, aH / 2.f});
                        arrow.setScale({4.f, 4.f});
                        arrow.setTextureRect(sf::IntRect({LOOP_START * aW, 0},{aW, aH}));

                        float angle = std::atan2(chargeDir.y, chargeDir.x) * 180.f / 3.14159265f;
                        arrow.setRotation(sf::degrees(angle));
                        arrow.setPosition(chargeSpawnPos);

                        arrows.push_back(arrow);
                        velocities.push_back(chargeDir * 2000.f);
                        arrowFrames.push_back(LOOP_START);
                        arrowAnimClocks.push_back(sf::Clock());

                        isCharging  = false;
                        chargeFrame = 0;
                    }
                    else
                    {
                        chargeArrow.setTextureRect(sf::IntRect({chargeFrame * aW, 0},{aW, aH}));
                    }
                }

                sf::Vector2f playerCenter = player.getPosition()
                    + sf::Vector2f(frameWidth * scale / 2.f, frameHeight * scale / 2.f);
                chargeSpawnPos = playerCenter + sf::Vector2f(60.f, 0.f);
                chargeArrow.setPosition(chargeSpawnPos);
            }

            // ================= MOVEMENT =================
            sf::Vector2f move(0.f, 0.f);
            bool moving = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            { move.x -= speed; player.setTexture(leftTexture);  moving = true; }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            { move.x += speed; player.setTexture(rightTexture); moving = true; }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) move.y -= speed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) move.y += speed;

            if (!moving) player.setTexture(idleTexture);

            player.move(move * dt);

            // Map boundary
            sf::Vector2f pos = player.getPosition();
            float pW = frameWidth  * scale;
            float pH = frameHeight * scale;
            if (pos.x < 0.f)        pos.x = 0.f;
            if (pos.y < 0.f)        pos.y = 0.f;
            if (pos.x + pW > MAP_W) pos.x = MAP_W - pW;
            if (pos.y + pH > MAP_H) pos.y = MAP_H - pH;
            player.setPosition(pos);

            // ================= PLAYER ANIMATION =================
            if (playerAnimClock.getElapsedTime().asSeconds() > 0.15f)
            {
                playerFrame = (playerFrame + 1) % 4;
                player.setTextureRect(sf::IntRect(
                    {playerFrame * frameWidth, 0},
                    {frameWidth, frameHeight}
                ));
                playerAnimClock.restart();
            }

            // ================= SPAWN ENEMY =================
            if (spawnClock.getElapsedTime().asSeconds() > spawnInterval)
            {
                spawnClock.restart();
                if (spawnInterval > 0.5f) spawnInterval -= 0.05f;

                Enemy e(enemyTexture);

                int side = std::rand() % 4;
                float ex, ey;
                if (side == 0)      { ex = (float)(std::rand() % (int)MAP_W); ey = -80.f; }
                else if (side == 1) { ex = (float)(std::rand() % (int)MAP_W); ey = MAP_H + 80.f; }
                else if (side == 2) { ex = -80.f; ey = (float)(std::rand() % (int)MAP_H); }
                else                { ex = MAP_W + 80.f; ey = (float)(std::rand() % (int)MAP_H); }

                e.sprite.setPosition({ex, ey});

                sf::Vector2f playerCenter = player.getPosition()
                    + sf::Vector2f(frameWidth * scale / 2.f, frameHeight * scale / 2.f);
                sf::Vector2f dir = playerCenter - sf::Vector2f(ex, ey);
                float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len != 0) dir /= len;

                e.velocity = dir * enemySpeed;
                enemies.push_back(std::move(e));
            }

            // ================= ENEMY UPDATE =================
            sf::FloatRect playerBounds = player.getGlobalBounds();

            sf::Vector2f playerCenter = player.getPosition()
                + sf::Vector2f(frameWidth * scale / 2.f, frameHeight * scale / 2.f);

            for (size_t i = 0; i < enemies.size(); i++)
            {
                if (!enemies[i].dying)
                {
                    // Update arah ke player setiap frame
                    sf::Vector2f enemyPos = enemies[i].sprite.getPosition();
                    sf::Vector2f dir = playerCenter - enemyPos;
                    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (len != 0) dir /= len;
                    enemies[i].velocity = dir * enemySpeed;
                }

                enemies[i].sprite.move(enemies[i].velocity * dt);

                if (enemies[i].dying)
                {
                    // Animasi despawn
                    if (enemies[i].dyingClock.getElapsedTime().asSeconds() > 0.10f)
                    {
                        enemies[i].dyingFrame++;
                        enemies[i].dyingClock.restart();

                        if (enemies[i].dyingFrame >= 5)
                        {
                            enemies.erase(enemies.begin() + i);
                            i--;
                            continue;
                        }

                        enemies[i].sprite.setTextureRect(sf::IntRect(
                            {enemies[i].dyingFrame * eW, 0},
                            {eW, eH}
                        ));
                    }
                }
                else
                {
                    // Animasi jalan normal
                    if (enemies[i].animClock.getElapsedTime().asSeconds() > 0.15f)
                    {
                        enemies[i].frame = (enemies[i].frame + 1) % 4;
                        enemies[i].sprite.setTextureRect(sf::IntRect(
                            {enemies[i].frame * eW, 0},
                            {eW, eH}
                        ));
                        enemies[i].animClock.restart();
                    }

                    // Hitbox kecil enemy vs player
                    sf::Vector2f ePos = enemies[i].sprite.getPosition();
                    sf::FloatRect enemyHitbox(
                        {ePos.x - 30.f, ePos.y - 30.f},
                        {60.f, 60.f}
                    );

                    sf::Vector2f pPos = player.getPosition()
                        + sf::Vector2f(frameWidth * scale / 2.f, frameHeight * scale / 2.f);
                    sf::FloatRect playerHitbox(
                        {pPos.x - 30.f, pPos.y - 30.f},
                        {60.f, 60.f}
                    );

                    if (enemyHitbox.findIntersection(playerHitbox))
                    {
                        gameOver   = true;
                        gameActive = false;
                    }
                }
            }

            // ================= ARROWS UPDATE =================
            for (size_t i = 0; i < arrows.size(); i++)
            {
                arrows[i].move(velocities[i] * dt);

                if (arrowAnimClocks[i].getElapsedTime().asSeconds() > 0.08f)
                {
                    arrowFrames[i]++;
                    if (arrowFrames[i] >= LOOP_END) arrowFrames[i] = LOOP_START;
                    arrows[i].setTextureRect(sf::IntRect(
                        {arrowFrames[i] * aW, 0},
                        {aW, aH}
                    ));
                    arrowAnimClocks[i].restart();
                }

                bool hit = false;
                for (size_t j = 0; j < enemies.size(); j++)
                {
                  
sf::Vector2f aPos = arrows[i].getPosition();
sf::FloatRect arrowHitbox(
    {aPos.x - 15.f, aPos.y - 15.f},
    {30.f, 30.f}
);

// Hitbox kecil enemy
sf::Vector2f ePos2 = enemies[j].sprite.getPosition();
sf::FloatRect enemyHitbox2(
    {ePos2.x - 55.f, ePos2.y - 55.f},
    {110.f, 110.f}
);

if (!enemies[j].dying &&
    arrowHitbox.findIntersection(enemyHitbox2))
                    {
                        // Mulai animasi despawn
                        enemies[j].dying      = true;
                        enemies[j].dyingFrame = 0;
                        enemies[j].sprite.setTexture(despawnTexture);
                        enemies[j].sprite.setTextureRect(sf::IntRect({0,0},{eW,eH}));
                        enemies[j].velocity   = {0.f, 0.f};
                        enemies[j].dyingClock.restart();
                        hit = true;
                        break;
                    }
                }

                if (hit ||
                    arrows[i].getPosition().x < -100 ||
                    arrows[i].getPosition().x > MAP_W + 100 ||
                    arrows[i].getPosition().y < -100 ||
                    arrows[i].getPosition().y > MAP_H + 100)
                {
                    arrows.erase(arrows.begin() + i);
                    velocities.erase(velocities.begin() + i);
                    arrowFrames.erase(arrowFrames.begin() + i);
                    arrowAnimClocks.erase(arrowAnimClocks.begin() + i);
                    i--;
                }
            }
        }
        window.clear();
        window.draw(bg);

        if (!gameOver)
        {
            for (auto &e : enemies) window.draw(e.sprite);
            if (isCharging) window.draw(chargeArrow);
            for (auto &a : arrows) window.draw(a);
            window.draw(player);
        }
        else
        {
            for (auto &e : enemies) window.draw(e.sprite);
            for (auto &a : arrows)  window.draw(a);
            window.draw(player);
            window.draw(overlay);
            window.draw(gameOverText);
            window.draw(btnRestart);
            window.draw(txtRestart);
            window.draw(btnQuit);
            window.draw(txtQuit);
        }

        window.display();
    }

    return 0;
}