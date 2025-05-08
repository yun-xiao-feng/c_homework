#include "Scenemain.h"
#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <random>
#include<string>



Scenemain::~Scenemain()
{
}

void Scenemain::init()
{
    // 载入BGM
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
    // 读取UI血量
    uiHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");
    // 字体
    scorefont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
    // 读取音效
    sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["player_explosion"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemy_explosion"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    sounds["get_thing"] = Mix_LoadWAV("assets/sound/eff5.wav");
    std::random_device rd;
    gen = std::mt19937(rd());
    dis = std::uniform_real_distribution<float>(0.0f, 1.0f);

    player.texture = IMG_LoadTexture(game.getInstance().getRenderer(), "assets/image/SpaceShip.png");
    if (player.texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load PLAYER TEXTURE!:%s", SDL_GetError());
    }
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 4;
    player.height /= 4;
    player.position.x = float(game.getWindowWidth()) / 2 - float(player.width) / 2;
    player.position.y = float(game.getWindowHeight()) - float(player.height);

    // 初始化模板
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.height);
    projectilePlayerTemplate.width /= 4;
    projectilePlayerTemplate.height /= 4;

    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;

    projectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-1.png");
    SDL_QueryTexture(projectileEnemyTemplate.texture, NULL, NULL, &projectileEnemyTemplate.width, &projectileEnemyTemplate.height);
    projectileEnemyTemplate.width /= 4;
    projectileEnemyTemplate.height /= 4;

    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.totalFrame = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.width = explosionTemplate.height;

    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture, NULL, NULL, &itemLifeTemplate.width, &itemLifeTemplate.height);
    itemLifeTemplate.width /= 4;
    itemLifeTemplate.height /= 4;
}

void Scenemain::clean()
{ // 清理容器
    for (auto sound : sounds)
    {
        if (sound.second != nullptr)
        {
            Mix_FreeChunk(sound.second);
        }
    }
    for (auto &projectile : projectilesPlayer)
    {
        if (projectile != nullptr)
        {
            delete projectile;
        }
    }
    projectilesPlayer.clear();

    for (auto &enemy : enemies)
    {
        if (enemy != nullptr)
        {
            delete enemy;
        }
    }
    enemies.clear();

    for (auto &projectile : projectilesEnemy)
    {
        if (projectile != nullptr)
        {
            delete projectile;
        }
    }
    items.clear();
    // 清理UI
    if (uiHealth != nullptr)
    {
        SDL_DestroyTexture(uiHealth);
    }
    // 清理字体
    if (scorefont != nullptr)
    {
        TTF_CloseFont(scorefont);
    }

    // 清理模板
    if (player.texture != nullptr)
    {
        SDL_DestroyTexture(player.texture);
    }
    if (projectilePlayerTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
    }
    if (enemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(enemyTemplate.texture);
    }
    if (projectileEnemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(projectileEnemyTemplate.texture);
    }
    if (itemLifeTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(itemLifeTemplate.texture);
    }
    // 清理音乐
    if (bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void Scenemain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerProjectiles(deltaTime);
    updateEnemyProjectiles(deltaTime);
    spawEnemy();
    updateEnemies(deltaTime);
    updatePlayer(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
}

void Scenemain::render()
{ // 渲染玩家子弹
    renderPlayerProjectiles();
    // 渲染敌人子弹
    renderEnemyProjectiles();
    // 渲染玩家
    if (!isDead)
    {
        SDL_Rect playerRect = {static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
        SDL_RenderCopy(game.getInstance().getRenderer(), player.texture, NULL, &playerRect);
    }
    // 渲染敌人
    renderEnemies();
    // 渲染物品
    renderItem();
    // 渲染爆炸效果
    renderExplosions();
    // 渲染UI
    renderUI();
}

void Scenemain::handleEvent(SDL_Event *event)
{
}

void Scenemain::keyboardControl(float deltaTime)
{
    if (isDead)
    {
        return;
    }
    auto keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_W])
    {
        player.position.y -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        player.position.y += deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        player.position.x -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        player.position.x += deltaTime * player.speed;
    }
    // 限制飞机移动范围
    if (player.position.x <= 0)
    {
        player.position.x = 0;
    }
    if (player.position.x >= game.getWindowWidth() - player.width)
    {
        player.position.x = float(game.getWindowWidth()) - float(player.width);
    }
    if (player.position.y <= 0)
    {
        player.position.y = 0;
    }
    if (player.position.y >= game.getWindowHeight() - player.height)
    {
        player.position.y = float(game.getWindowHeight()) - float(player.height);
    }
    // 控制子弹发射
    if (keyboardState[SDL_SCANCODE_J])
    {
        auto currentTime = SDL_GetTicks();
        if (currentTime - player.lastShootTime > player.coolDown)
        {
            shootPlayer();
            player.lastShootTime = currentTime;
        }
    }
}

void Scenemain::shootPlayer()
{
    auto projectile = new ProjectilePlayer(projectilePlayerTemplate);
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    projectilesPlayer.push_back(projectile);

    Mix_PlayChannel(0, sounds["player_shoot"], 0);
}

void Scenemain::updatePlayerProjectiles(float deltaTime)
{
    int margin = 32;
    for (auto it = projectilesPlayer.begin(); it != projectilesPlayer.end();)
    {
        auto projectile = *it;
        projectile->position.y -= projectile->speed * deltaTime;
        if (projectile->position.y + margin < 0)
        {
            delete projectile;
            it = projectilesPlayer.erase(it);
            // SDL_Log("Player projectile removed");
        }
        else
        {
            bool hit = false;
            for (auto enemy : enemies)
            {
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height};
                SDL_Rect projectileRect = {
                    static_cast<int>(projectile->position.x),
                    static_cast<int>(projectile->position.y),
                    projectile->width,
                    projectile->height};
                if (SDL_HasIntersection(&projectileRect, &enemyRect))
                {
                    enemy->currentHealth -= projectile->damage;
                    delete projectile;
                    it = projectilesPlayer.erase(it);
                    hit = true;
                    Mix_PlayChannel(-1, sounds["hit"], 0);
                    break;
                }
            }
            if (!hit)
            {
                ++it;
            }
        }
    }
}

void Scenemain::renderPlayerProjectiles()
{
    for (auto projectile : projectilesPlayer)
    {
        SDL_Rect projectileRect =
            {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height};
        SDL_RenderCopy(game.getRenderer(), projectile->texture, nullptr, &projectileRect);
    }
}

void Scenemain::renderEnemyProjectiles()
{
    for (auto projectile : projectilesEnemy)
    {
        SDL_Rect projectileRect =
            {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height};
        float angle = atan2(projectile->direction.y, projectile->direction.x) * 180 / M_PI - 90;
        SDL_RenderCopyEx(game.getRenderer(), projectile->texture, nullptr, &projectileRect, angle, nullptr, SDL_FLIP_NONE);
        // SDL_RenderCopy(game.getRenderer(),projectile->texture, nullptr, &projectileRect);
    }
}

void Scenemain::spawEnemy()
{
    if (dis(gen) > 1 / 60.0f)
    {
        return;
    }
    Enemy *enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy->position.y = float(-enemy->height);
    enemies.push_back(enemy);
}

void Scenemain::updateEnemies(float deltaTime)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        if (enemy->position.y > game.getWindowHeight())
        {
            delete enemy;
            it = enemies.erase(it);
            // SDL_Log("Enemy removed");
        }
        else
        {
            if ((currentTime - enemy->lastShootTime > enemy->coolDown) && !isDead)
            {
                shootEnemy(enemy);
                enemy->lastShootTime = currentTime;
            }
            if (enemy->currentHealth <= 0)
            {
                enemyExplode(enemy);
                it = enemies.erase(it);
            }

            else
            {
                it++;
            }
        }
    }
}

void Scenemain::updatePlayer(float)
{
    if (isDead)
    {
        return;
    }
    if (player.currentHealth <= 0)
    {
        auto currentTime = SDL_GetTicks();
        isDead = true;
        auto explosion = new Explosion(explosionTemplate);
        explosion->position.x = player.position.x + player.width / 2 - explosion->width / 2;
        explosion->position.y = player.position.y + player.height / 2 - explosion->height / 2;
        explosion->startTime = currentTime;
        explosions.push_back(explosion);
        Mix_PlayChannel(-1, sounds["player_explosion"], 0);
        return;
    }
    for (auto enemy : enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height};
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height};
        if (SDL_HasIntersection(&enemyRect, &playerRect))
        {
            player.currentHealth -= 1;
            enemy->currentHealth = 0;
        }
    }
}

void Scenemain::renderEnemies()
{
    for (auto enemy : enemies)
    {
        SDL_Rect enemyRect =
            {
                static_cast<int>(enemy->position.x),
                static_cast<int>(enemy->position.y),
                enemy->width,
                enemy->height};
        SDL_RenderCopy(game.getRenderer(), enemy->texture, nullptr, &enemyRect);
    }
}

void Scenemain::shootEnemy(Enemy *enemy)
{
    auto projectile = new ProjectileEnemy(projectileEnemyTemplate);
    projectile->position.x = enemy->position.x + enemy->width / 2 - projectile->width / 2;
    projectile->position.y = enemy->position.y + enemy->height / 2 - projectile->height / 2;
    projectile->direction = getDirection(enemy);
    projectilesEnemy.push_back(projectile);
    Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
}

SDL_FPoint Scenemain::getDirection(Enemy *enemy)
{
    auto x = (player.position.x + player.width / 2) - (enemy->position.x + enemy->width / 2);
    auto y = (player.position.y + player.height / 2) - (enemy->position.y + enemy->height / 2);
    auto length = sqrt(x * x + y * y);
    x /= length;
    y /= length;

    return SDL_FPoint{x, y};
}

void Scenemain::updateEnemyProjectiles(float deltaTime)
{
    auto margin = 32;
    for (auto it = projectilesEnemy.begin(); it != projectilesEnemy.end();)
    {
        auto projectile = *it;
        projectile->position.y += projectile->speed * projectile->direction.y * deltaTime;
        projectile->position.x += projectile->speed * projectile->direction.x * deltaTime;
        if (projectile->position.y > game.getWindowHeight() + margin || projectile->position.y < -margin || projectile->position.x > game.getWindowWidth() + margin || projectile->position.x < -margin)
        {
            delete projectile;
            it = projectilesEnemy.erase(it);
        }
        else
        {
            SDL_Rect projectileRect = {
                static_cast<int>(projectile->position.x),
                static_cast<int>(projectile->position.y),
                projectile->width,
                projectile->height};
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height};
            if (SDL_HasIntersection(&projectileRect, &playerRect) && !isDead)
            {
                player.currentHealth -= projectile->damage;
                delete projectile;
                it = projectilesEnemy.erase(it);
                Mix_PlayChannel(-1, sounds["hit"], 0);
            }
            else
            {
                it++;
            }
        }
    }
}

void Scenemain::enemyExplode(Enemy *enemy)
{
    auto currentTime = SDL_GetTicks();
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);
    Mix_PlayChannel(-1, sounds["ennemy_explosion"], 0);
    if (dis(gen) < 0.5f)
    {
        dropItem(enemy);
    }
    score += 10;
    delete enemy;
}

void Scenemain::updateExplosions(float)
{
    auto currentTime = SDL_GetTicks();
    for (auto it = explosions.begin(); it != explosions.end();)
    {
        auto explosion = *it;
        explosion->currentFrame = explosion->FPS * (currentTime - explosion->startTime) / 1000;
        if (explosion->currentFrame >= explosion->totalFrame)
        {
            delete explosion;
            it = explosions.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Scenemain::renderExplosions()
{
    for (auto explosion : explosions)
    {
        SDL_Rect src = {explosion->currentFrame * explosion->width, 0, explosion->width, explosion->height};
        SDL_Rect dst = {
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            static_cast<int>(explosion->width),
            static_cast<int>(explosion->height)};
        SDL_RenderCopy(game.getRenderer(), explosion->texture, &src, &dst);
    }
}

void Scenemain::dropItem(Enemy *enemy)
{
    auto item = new Item(itemLifeTemplate);
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
    float angle = dis(gen) * 2 * M_PI;
    item->direction.x = cos(angle);
    item->direction.y = sin(angle);
    items.push_back(item);
}

void Scenemain::updateItems(float deltaTime)
{
    for (auto it = items.begin(); it != items.end();)
    {
        auto item = *it;
        // 更新位置
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        // 反弹
        if (item->position.x < 0 && item->bounceCount > 0)
        {
            item->direction.x = -item->direction.x;
            item->bounceCount--;
        }
        if (item->position.x + item->width > game.getWindowWidth() && item->bounceCount > 0)
        {
            item->direction.x = -item->direction.x;
            item->bounceCount--;
        }
        if (item->position.y < 0 && item->bounceCount > 0)
        {
            item->direction.y = -item->direction.y;
            item->bounceCount--;
        }
        if (item->position.y + item->height > game.getWindowHeight() && item->bounceCount > 0)
        {
            item->direction.y = -item->direction.y;
            item->bounceCount--;
        }

        // 超出屏幕删除
        if (item->position.x + item->width < 0 ||
            item->position.x > game.getWindowHeight() ||
            item->position.y + item->height < 0 ||
            item->position.y > game.getWindowHeight())
        {
            delete item;
            it = items.erase(it);
        }
        else
        {
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height,
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height,
            };
            if (SDL_HasIntersection(&itemRect, &playerRect) && isDead == false)
            {
                playerGetItem(item);
                delete item;
                it = items.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
}

void Scenemain::playerGetItem(Item *item)
{
    score += 5;
    if (item->type == ItemType::HEART)
    {
        player.currentHealth += 1;
        if (player.currentHealth > player.maxHealth)
        {
            player.currentHealth = player.maxHealth;
        }
    }
    Mix_PlayChannel(-1, sounds["get_thing"], 0);
}

void Scenemain::renderItem()
{
    for (auto item : items)
    {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height,
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
    }
}

void Scenemain::renderUI()
{
    // 血量
    int x = 10;
    int y = 10;
    int offset = 40;
    int size = 32;
    SDL_SetTextureColorMod(uiHealth, 0, 0, 0);
    for (int i = 0; i < player.maxHealth; i++)
    {
        SDL_Rect rect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }
    SDL_SetTextureColorMod(uiHealth, 255, 255, 255);
    for (int i = 0; i < player.currentHealth; i++)
    {
        SDL_Rect rect = {x + i * offset, y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &rect);
    }
    // 得分
    auto text = "SCORE: " + std::to_string(score);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(scorefont, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect rect = {game.getWindowWidth()-200, 10, surface->w, surface->h};
    SDL_RenderCopy(game.getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
