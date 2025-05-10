#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>
#include <SDL_mixer.h>
#include <map>
#include <SDL_ttf.h>
class Game;
class Scenemain : public Scene
{
public:
    ~Scenemain();
    void init() override;
    void clean() override;
    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event *event) override;

private:
    Player player;
    Mix_Music *bgm;
    SDL_Texture *uiHealth;
    TTF_Font *scorefont;
    int score = 0;
    float timeEnd=0.0f;

    bool isDead = false;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    // 创建每个物体的模板
    ProjectilePlayer projectilePlayerTemplate;
    Enemy enemyTemplate;
    ProjectileEnemy projectileEnemyTemplate;
    Explosion explosionTemplate;
    Item itemLifeTemplate;
    // 创建每个物体容器
    std::list<ProjectilePlayer *> projectilesPlayer; //
    ProjectilePlayer enemyProjectileTemplate;
    std::list<Enemy *> enemies; //
    // std::list<ProjectilePlayer*> enemyProjectiles;
    std::list<ProjectileEnemy *> projectilesEnemy; //
    std::list<Explosion *> explosions;
    std::list<Item *> items;
    std::map<std::string, Mix_Chunk *> sounds; // 存音效
    // 渲染
    void renderItem();
    void renderUI();
    void renderPlayerProjectiles();
    void renderEnemyProjectiles();
    void renderExplosions();
    void renderEnemies();

    // 更新
    void updateEnemyProjectiles(float deltaTime);
    void updatePlayerProjectiles(float deltaTime);
    void updateEnemies(float deltaTime);
    void updatePlayer(float deltaTime);
    void updateExplosions(float deltaTime);
    void updateItems(float deltaTime);
    void keyboardControl(float deltaTime);
    void spawEnemy();
    void changeSceneDelayed(float deltatime,float delay);
    // 其他
    void playerGetItem(Item *item);
    void shootPlayer();
    void shootEnemy(Enemy *enemy);
    SDL_FPoint getDirection(Enemy *enemy);
    void enemyExplode(Enemy *enemy);
    void dropItem(Enemy *enemy);
    
};

#endif