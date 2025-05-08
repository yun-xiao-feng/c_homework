#ifndef GAME_H
#define GAME_H
#include"Scene.h"
#include"SDL.h"
#include"object.h"
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<string>

class Game
{
public:
   static Game& getInstance(){
       static Game instance;
       return instance;
   };
    ~Game();
    void init();
    void run();
    void clean();
void changeScene(Scene* scene);

void handleEvents(SDL_Event *event);
void update(float deltaTime);
void render();
void backgroudUpdate(float deltaTime);
void renderBackground();
//渲染文字
void renderTextCenter(std::string text,float posy, bool istitle);
SDL_Window* getWindow(){return window;};
SDL_Renderer* getRenderer(){return renderer;};
int getWindowWidth(){return windowWidth;};
int getWindowHeight(){return windowHeight;};
private:
Game();
//删除拷贝 赋值 构造函数
Game(const Game&) = delete;
Game& operator=(const Game&) = delete;

TTF_Font* titleFont;
TTF_Font* textFont;

    bool isRunning = true;
    Scene* currentScene=nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;
int FPS=60;
Uint32 frameTime;
float deltaTime;

Background nearStars;
Background farStars;
};

#endif