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
    IrrlichtDevice* device;
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
    gui::IGUIStaticText* textAmmo;
    gui::IGUIStaticText* textPosition;
    gui::IGUIStaticText* textPrimitives;
    gui::IGUIStaticText* textTime;
    gui::IGUIStaticText* textFPS;
    gui::IGUIStaticText* textNotifications;
    
    gui::IGUIStaticText* menuNew;
    gui::IGUIStaticText* menuExit;
    
    gui::IGUIImage* imgBoxCooldowns;
    gui::IGUIImage* imgBoxHealth;
    
    gui::IGUIImage* crosshair;
    
    gui::IGUIImage* damageOverlay;
    gui::IGUIImage* webOverlay;
    gui::IGUIImage* deadOverlay;

    gui::IGUIImage* imgHealth;
    gui::IGUIImage* imgBlock;          
    gui::IGUIImage* imgKnockback;
    gui::IGUIImage* imgRPG;
    gui::IGUIImage* imgAmmo;

    StateManager(IrrlichtDevice* device, IPhysxManager* physxManager, Game* game);
    ~StateManager(void);
    
    void LoadState(s32 state);
    void Update(s32 time);
    void NodeInit(IMeshSceneNode* node, vector3df scale);
};
