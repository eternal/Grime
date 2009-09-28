#pragma once
#include <irrlicht.h>
#include "Game.h"
#include <ctime>

class EventReceiver :
    public IEventReceiver
{
public:
    EventReceiver(Game* game, IrrlichtDevice* device);
    EventReceiver();
    ~EventReceiver(void);
    
    Game* game;
    IrrlichtDevice* device;
    
    bool OnEvent(const SEvent& event);
    void Setup(Game* game, IrrlichtDevice* device);
    
};
