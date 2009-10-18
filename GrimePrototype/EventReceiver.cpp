#include "EventReceiver.h"

EventReceiver::EventReceiver()
{
    this->game = NULL;
    this->device = NULL;
    leftPressed = false;
    rightPressed = false;
}

EventReceiver::EventReceiver(Game* game, IrrlichtDevice* device)
{
    this->game = game;
    this->device = device;
    leftPressed = false;
    rightPressed = false;
}

EventReceiver::~EventReceiver(void)
{
}

void EventReceiver::Update(s32 time)
{
    if (game)
    {
        if (stateManager->smgr)
        {
            switch (stateManager->currentState)
            {
                case EGS_GAME:
                {
                    if (leftPressed)
                    {
                        game->WeaponFire();
                    }
                    if (rightPressed)
                    {
                        game->WeaponFire(WEAPON_BLOCKGUN);
                    }
                }
             }
    }
}
}

bool EventReceiver::OnEvent(const SEvent& event) 
{    
    if (game)
    {
        if (stateManager->smgr)
        {
            switch (stateManager->currentState)
            {
            case EGS_GAME:
                {
                    if (event.EventType == EET_KEY_INPUT_EVENT) 
                    {
                        switch (event.KeyInput.Key) 
                        { 
                        case KEY_ADD:   
                            game->spawnManager->timeBetweenSpawns+= 100;
                            break;
                        case KEY_SUBTRACT:
                            game->spawnManager->timeBetweenSpawns-= 100;
                            break;
                        case KEY_SPACE:
                            //jump
                            game->player->Jump();
                            break;
                        }
                        //if the key is held down, return and ignore
                        if (event.KeyInput.PressedDown) return false;

                        switch (event.KeyInput.Key) 
                        {
                        case KEY_F9: 
                            {
                                //screenshot                  
                                core::stringc filename = "GrimePrototype";
                                filename += (unsigned)time(0);
                                filename += ".jpg";
                                this->device->getVideoDriver()->writeImageToFile(device->getVideoDriver()->createScreenShot(),filename,0);
                            }
                            break;
                        case KEY_KEY_G: 
                            {
                                Block* block = new Block(game->smgr, game->physxManager, &game->enemyObjects, &game->blockObjects);
                                game->blockObjects.push_back(block);
                            }
                            break;  
                        case KEY_KEY_H:
                            game->ConvertBlocks();
                            break;
                        case KEY_KEY_J: 
                            {
                                game->CleanupArrays();
                            }
                            break;
                        case KEY_KEY_P:
                            {
                                game->spawnManager->onCooldown = true;
                            }   
                            break;                      
                        case KEY_KEY_1: 
                            {
                                game->spawnManager->SpawnCockroach(vector3df(0.0f,100.0f,0.0f));
                            }
                            break;
                        case KEY_KEY_2: 
                            {
                                game->spawnManager->SpawnSpider(vector3df(0.0f,100.0f,0.0f));
                            }
                            break;
                        case KEY_KEY_3: 
                            {
                                game->spawnManager->SpawnBeetle(vector3df(0.0f,100.0f,0.0f));
                            }
                            break;
                        case KEY_KEY_4: 
                            {
                                game->spawnManager->SpawnRat(vector3df(1500.0f,100.0f,-800.0f));
                            }
                            break; 
                        case KEY_KEY_5:
                            {
                                game->spawnManager->SpawnCockroach(game->spawnManager->RandomPoint());
                            }
                        break;
                        case KEY_KEY_0:
                            game->spawnManager->waveTimer = 530000;
                            game->spawnManager->phase = 3;
                        break;
                        case KEY_KEY_I:
                            game->RestartLevel();
                            break;
                        case KEY_KEY_C:
                        {
                            game->ClearEnemies();
                        }
                        break;
                        case KEY_KEY_O:
                            stateManager->LoadState(EGS_MENU);
                            break;
                        case KEY_KEY_V:
                            // Toggle the debug data visibility
                            game->physxManager->setDebugDataVisible(!game->physxManager->isDebugDataVisible());
                            break;
                        case KEY_MULTIPLY:
                            game->spawnManager->timeBetweenSpawns = 100;
                            break;
                        case KEY_DIVIDE:
                            game->spawnManager->timeBetweenSpawns = 2500;
                            break;
                        case KEY_ESCAPE:
                            //close the device and exit
                            device->closeDevice();
                            break;
                        }
                    } 
                    else if (event.EventType == EET_MOUSE_INPUT_EVENT) 
                    {
                        if (event.MouseInput.isLeftPressed())
                        {
                            leftPressed = true;
                        }
                        else
                        {
                            leftPressed = false;
                        }
                        if (event.MouseInput.isRightPressed())
                        {
                            rightPressed = true;
                        }
                        else
                        {
                            rightPressed = false;
                        }
                        switch (event.MouseInput.Event) 
                        {
                        case EMIE_MOUSE_WHEEL: 
                            {
                                game->player->WeaponSelect(event.MouseInput.Wheel);
                            }
                            break;
                        case EMIE_LMOUSE_PRESSED_DOWN: 
                            {

                            }
                            break;
                        case EMIE_LMOUSE_LEFT_UP: 
                            {

                            }
                            break;             
                        case EMIE_RMOUSE_PRESSED_DOWN: 
                            {

                            }
                            break;
                        case EMIE_RMOUSE_LEFT_UP: 
                            {

                            }
                            break;
                        }          
                    } 
                    else if (event.EventType == EET_GUI_EVENT) 
                    {
                        //gui events
                        return false;
                    }
                    return false;
                }
            case EGS_MENU:
                {
                    if (event.EventType == EET_KEY_INPUT_EVENT) 
                    {
                        //if the key is held down, return and ignore
                        if (event.KeyInput.PressedDown) return false;

                        switch (event.KeyInput.Key) 
                        {
                        case KEY_KEY_O: 
                            {
                                this->stateManager->LoadState(EGS_GAME);
                            }
                        }

                    }
                }
            }
        }
    }
 return false;
}
    


void EventReceiver::Setup( Game* game, IrrlichtDevice* device )
{
    this->game = game;
    this->device = device;
}