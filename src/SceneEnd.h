#ifndef SCENE_END_H
#define SCENE_END_H
#include "Scene.h"
#include<string>

class SceneEnd : public Scene
{
public:
    void init();
    void update(float deltaTime);
    void render();
    void clean();
    void handleEvent(SDL_Event *event);
    private:
    bool isTyping=true;
    void renderPhase1();
    void renderPhase2();
    std::string name="";
    float blinktime=1.0f;
};

#endif