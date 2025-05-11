#include "Game.h"
#include "Scene.h"
#include"SceneTitle.h"
#include "Scenemain.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include<fstream>

Game::Game() {}

void Game::saveData()
{
std::ofstream file("assets/save.dat");
if(!file.is_open()){
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to open file");
    return;
}
for(const auto& pair : leaderBoard){
    file << pair.first << " " << pair.second << std::endl;
}
}

void Game::loadData()
{
    std::ifstream file("assets/save.dat");
    if(!file.is_open()){
        SDL_Log("Unable to open file");
        return;
    }
    leaderBoard.clear();
    int score;
    std::string name;
    while(file >> score >> name){
        leaderBoard.insert({score, name});
    }
}

void Game::insertLeaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});
    if(leaderBoard.size() > 8){
        leaderBoard.erase(--leaderBoard.end());
    }
}

Game::~Game()
{
    saveData();
    clean();
}

void Game::init()
{
    frameTime = 1000 / FPS;
    // SDL 初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建窗口
    window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Render could not be created! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    //全屏
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);

    // 初始SDL_Image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Image could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Mixer could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Mixer could not open audio device! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 设置音效数量
    Mix_AllocateChannels(32);
    // 设置音量
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 8);
    //初始化SDL_ttf
    if (TTF_Init() == -1)   {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 设置背景卷轴
    nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
    if( nearStars.texture == NULL) {    
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
    nearStars.height/=2;
    nearStars.width/=2;
    farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
    if( farStars.texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_Error: %s\n", SDL_GetError());  
        isRunning = false;
    }
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.speed = 20;

    //载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (titleFont == NULL || textFont == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont: %s\n", TTF_GetError());
        isRunning = false;
    }

    loadData();


    currentScene = new SceneTitle();
    currentScene->init();
}

void Game::run()
{

    while (isRunning)
    {
        auto frameStart = SDL_GetTicks();
        SDL_Event event;
        handleEvents(&event);
        update(deltaTime);
        render();
        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if (diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        }

        else
        {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::clean()
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    if (nearStars.texture != nullptr)
    {
        SDL_DestroyTexture(nearStars.texture);
    }
    if (farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture);
    }
    if (textFont != nullptr)
    {
        TTF_CloseFont(textFont);

    }
    if (titleFont != nullptr){
        TTF_CloseFont(titleFont);
    }
    // 清理SDL_Image
    IMG_Quit();
    // 清理SDL_Mixer
    Mix_CloseAudio();
    Mix_Quit();
    // 清理SDL_TTF
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::changeScene(Scene *scene)
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
        currentScene = nullptr;
    }
    currentScene = scene;
    currentScene->init();
}

void Game::handleEvents(SDL_Event *event)
{
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isRunning = false;
        }
        if( event->type == SDL_KEYDOWN){
            if (event->key.keysym.scancode == SDL_SCANCODE_F){
                isFullScreen = !isFullScreen;
                if( isFullScreen){
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
            }
                else{
                    SDL_SetWindowFullscreen(window, 0);
                }
        }
        }
        currentScene->handleEvent(event);
    }
}

void Game::update(float deltaTime)
{
    backgroudUpdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
    // 清空
    SDL_RenderClear(renderer);
    // 渲染星空
    renderBackground();

    currentScene->render();
    // 更新
    SDL_RenderPresent(renderer);
}

void Game::backgroudUpdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if (nearStars.offset >= 0)
    {
        nearStars.offset -= nearStars.height;
    }
    farStars.offset += farStars.speed * deltaTime;
    if (farStars.offset >= 0)
    {
        farStars.offset -= farStars.height;
    }
}

void Game::renderBackground()
{
    // 远星渲染
    int posY1 = static_cast<int>(farStars.offset);
    for (posY1; posY1 < getWindowHeight(); posY1 += farStars.height)
    {
        for (int posX = 0; posX < getWindowWidth(); posX += farStars.width)
        {
            SDL_Rect dstRect = {posX, posY1, farStars.width, farStars.height};
            SDL_RenderCopy(renderer, farStars.texture, nullptr, &dstRect);
        }
    }
    // 近星渲染
    int posY2 = static_cast<int>(nearStars.offset);
    for (posY2; posY2 < getWindowHeight(); posY2 += nearStars.height)
    {
        for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width)
        {

            SDL_Rect dstRect = {posX, posY2, nearStars.width, nearStars.height};
            SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
        }
    }
}

SDL_Point Game::renderTextCenter(std::string text, float posy, bool istitle)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface;
    if(istitle){
    surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }
    else{
    surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
   
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect Rect = {getWindowWidth() / 2 - surface->w / 2,static_cast<int> (posy*(getWindowHeight()-surface->h)), surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &Rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
return {Rect.x+Rect.w, Rect.y};
}

void Game::renderTextPoser(std::string text, int posx, int posy, bool istitle,bool isLeft)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface;
    if(istitle){
    surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }
    else{
    surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect Rect;
    if(isLeft){
    Rect = {static_cast<int> (posx),static_cast<int> (posy), surface->w, surface->h};
    }
    else{
    Rect = {getWindowWidth() - surface->w - static_cast<int> (posx),static_cast<int> (posy), surface->w, surface->h};
    }
    SDL_RenderCopy(renderer, texture, nullptr, &Rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
