#pragma once
#include <irrlicht.h>
#include <irrPhysx.h>
#include "Game.h"  

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace IrrPhysx;

enum ENUM_GAME_STATES {
    EGS_MENU,
    EGS_GAME,
    EGS_ANNIHILATED
};

class StateManager
{
public:
    ISceneManager* smgr;
    IPhysxManager* physxManager;
    video::IVideoDriver* driver;
    gui::IGUIEnvironment* guienv;
    Game* game;
    
    s32 currentState;
    core::dimension2du resolution;
    
    //ISceneNode* roomnode;
    IMesh* room;

    gui::IGUIStaticText* levelText;
    gui::IGUIStaticText* buildText;
    gui::IGUIStaticText* textSpawns;    
    gui::IGUIStaticText* textHealth;
    gui::IGUIStaticText* textCooldown;
    gui::IGUIStaticText* textPosition;
    gui::IGUIStaticText* textPrimitives;
    gui::IGUIStaticText* textTime;
    gui::IGUIStaticText* textFPS;
    gui::IGUIStaticText* textNotifications;

    StateManager(IrrlichtDevice* device, IPhysxManager* physxManager, Game* game);
    ~StateManager(void);
    
    void LoadState(s32 state);
    void Update(s32 time);
    void NodeInit(IMeshSceneNode* node, vector3df scale);
};
