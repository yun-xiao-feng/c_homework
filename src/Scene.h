#ifndef SCENE_H
#define SCENE_H
#include <SDL.h>

class Game;
class Scene {
    public:
        Scene();
        ~Scene()=default;
       virtual void init()=0;
       virtual void update(float deltaTime)=0;
       virtual void render()=0;
       virtual void clean()=0;
       virtual void handleEvent(SDL_Event *event)=0;
       protected:
       Game& game;
    
};



#endif
