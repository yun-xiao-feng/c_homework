#include"SceneTitle.h"
#include<string>
#include"Game.h"

void SceneTitle::init(){
    //载入bgm
    bgm=Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if(bgm==NULL){
       SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Mix_LoadMUS failed: %s",Mix_GetError());

    }
    Mix_PlayMusic(bgm,-1);

}
void SceneTitle::update(float detatime){
    
}

void SceneTitle::render(){
    //标题
    std::string title="飞机大战";
   game.renderTextCenter(title,0.4,true);
    //开始游戏
    std::string start="按J进入游戏";
    game.renderTextCenter(start,0.8,false); 

}
void SceneTitle::clean(){
    
}
void SceneTitle::handleEvent(SDL_Event *event){
    
}
