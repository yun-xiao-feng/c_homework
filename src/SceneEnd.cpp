#include "SceneEnd.h"
#include "Game.h"
#include <string>
#include <cstring>
#include"Scenemain.h"
void SceneEnd::init()
{
    //bgm
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if(!bgm){
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "SDL", "Failed to load music: %s", SDL_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    if (!SDL_IsTextInputActive())
    {
        SDL_StartTextInput();
        if (!SDL_IsTextInputActive())
        {
            SDL_LogError(SDL_LOG_PRIORITY_ERROR, "SDL", "Failed to start text input: %s", SDL_GetError());
        }
    }
}
void SceneEnd::update(float delatTime)
{
    blinktime-= delatTime;
    if (blinktime < 0)
    {
        blinktime += 1.0f;
    }
}
void SceneEnd::render()
{
    if (isTyping)
    {
        renderPhase1();
    }
    else
    {
        renderPhase2();
    }
}
void SceneEnd::clean()
{
    if(bgm!= nullptr){
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void SceneEnd::handleEvent(SDL_Event *event)
{
    if (isTyping)
    {
        if (event->type == SDL_TEXTINPUT)
        {
            name += event->text.text;
        }
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                isTyping = false;
                SDL_StopTextInput();
                if(name==""){
                    name="Player";
                }
                game.insertLeaderBoard(game.getFinalscore(),name);
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE){
                name.pop_back();
            }
        }
        
    }
    else{
        if(event->type == SDL_KEYDOWN){
            if(event->key.keysym.scancode == SDL_SCANCODE_J){
            auto secenemain= new Scenemain();
            game.changeScene(secenemain);
            
            }
        }
    }
}

void SceneEnd::renderPhase1()
{
    auto score = game.getFinalscore();
    std::string scoreText = "your score is : " + std::to_string(score);

    std::string gameOver = "游戏结束";
    std::string instructionText = "请输入你的名字，按回车键确认:";
    game.renderTextCenter(scoreText, 0.1, false);
    game.renderTextCenter(gameOver, 0.4, true);
    game.renderTextCenter(instructionText, 0.6, false);
    if(name!=""){
        SDL_Point p=game.renderTextCenter(name, 0.8, false);
        if(blinktime<0.5f){
        game.renderTextPoser("_",p.x, p.y,false);
    }
}
    else{
        if(blinktime<0.5f){
        game.renderTextCenter("_", 0.8, false);
    }
}
}

void SceneEnd::renderPhase2()
{
    int i=1;
    auto posy=0.2f*game.getWindowHeight();

    game.renderTextCenter("Scoreboard", 0.05f, true);
    for(auto item:game.getLeaderBoard()){
    std::string name=std::to_string(i)+"."+item.second;
    std::string score=std::to_string(item.first);
    game.renderTextPoser(name, 100,posy, false);
    game.renderTextPoser(score, 100,posy, false,false);
    posy+=45;
    i++;
    }
    if(blinktime<0.5f){
    game.renderTextCenter("按J重新进入游戏", 0.85, false); 
    }   
}
