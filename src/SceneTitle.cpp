#include"SceneTitle.h"
#include<string>
#include"Game.h"
#include"Scenemain.h"
void SceneTitle::init(){
    //载入bgm
    bgm=Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if(bgm==NULL){
       SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Mix_LoadMUS failed: %s",Mix_GetError());

    }
    Mix_PlayMusic(bgm,-1);

}
void SceneTitle::update(float detatime){
    time+=detatime;
    if(time>1.0f){
        time-=1.0f;
    }
}

void SceneTitle::render(){
    //标题
    std::string title="飞机大战";
   game.renderTextCenter(title,0.4,true);
    //开始游戏
    if(time<0.5f){
    std::string start="按J进入游戏";
    game.renderTextCenter(start,0.8,false); 
    }
}
void SceneTitle::clean(){
    
}
void SceneTitle::handleEvent(SDL_Event *event){
    if(event->type==SDL_KEYDOWN){
        if(event->key.keysym.scancode==SDL_SCANCODE_J){
            auto sceneMain=new Scenemain();
           //sceneMain->init();
            game.changeScene(sceneMain);
        }
    }
}
