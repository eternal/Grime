#pragma once
#include <irrlicht.h>
#include "Game.h"
#include "StateManager.h"
#include <ctime>

class EventReceiver :
    public IEventReceiver
{
public:
    EventReceiver(Game* game, IrrlichtDevice* device);
    EventReceiver();
    ~EventReceiver(void);
    
    Game* game;
    StateManager* stateManager;
    IrrlichtDevice* device;
    
    bool leftPressed;
    bool rightPressed;
    
    bool OnEvent(const SEvent& event);
    void Setup(Game* game, IrrlichtDevice* device);
    void Update(s32 time);

};
