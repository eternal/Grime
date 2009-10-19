#include "SpawnManager.h"

SpawnManager::SpawnManager( ISceneManager* smgr, ISoundEngine* soundEngine, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects, core::array<Block*>* blockArray, Player* player, core::array<video::ITexture*>* explosionTextures)
{
    this->smgr = smgr;
    this->physxManager = physxManager;
    this->enemyObjects = enemyObjects;
    this->blockArray = blockArray;
    this->timeBetweenSpawns = timeBetweenSpawns;
    this->soundEngine = soundEngine;
    this->explosionTextures = explosionTextures;
    this->player = player;
    currentTimer = 0;
    spawnTimer = 0;
    waveTimer = 0;
    minuteTimer = 0;
    cooldownTimer = 0;
    phase = 0;
    timeBetweenSpawns = 2500;
    spawnsActive = false;
    onCooldown = false;
    
    spawnPosition[BASIN_HIGH] = vector3df(1401.0f, 680.1f, 0.0f);
    spawnPosition[MICROWAVE_HIGH] = vector3df(580.0f, 680.0f, -1125.0f);
    spawnPosition[MICROWAVE_LOWER] = vector3df(1627, 30.0f, -890.0f);
    spawnPosition[STOVE_LOWER] = vector3df(-805.0f, 30.0f, -1400.0f);
    spawnPosition[CUPBOARD_LOWER] = vector3df(-789.5f,30.0f,1080.842f);
    spawnPosition[FRIDGE_LOWER] = vector3df(164.0f, 30.0f, 1439.0f);
    
    for (u32 i =0; i < NUMBER_OF_POSITIONS; i++)
    {
        spawnLocationCooldown[i] = 0;
    }
    
    //cockroachMesh = smgr->getMesh("media/cockroack_rigged_a02.x"); COMMENTED OUT UNTIL LOWPOLY MESH ARRIVES
    cockroachMesh = smgr->getMesh("media/roachlowpoly.x");
    spiderMesh = smgr->getMesh("media/spiderlowpoly.x");
    ratMesh = smgr->getMesh("media/ratlowpoly.x");
    beetleMesh = smgr->getMesh("media/buglowpoly.x");
}
SpawnManager::~SpawnManager(void)
{
}
void SpawnManager::Update( s32 time )
{
    for (u32 i = 0; i < NUMBER_OF_POSITIONS; i++)
    {
        if (spawnLocationCooldown[i] >= 0)
        {
            spawnLocationCooldown[i] -= time;
        }
    }
    if (!onCooldown)
    {
        if (enemyObjects->size() <= 50)
        {
            waveTimer += time;
            if (!spawnsActive)
            {   
                //setup phase
                spawnsActive = true;
                onCooldown = true;
            }
            if (spawnsActive)
            {
                if (timeBetweenSpawns >= 800)
                {
                    currentTimer += time;
                    if (currentTimer >= 280) 
                    {
                        timeBetweenSpawns--;
                        currentTimer = 0;
                    }
                }
                if (phase == 3)
                {
                    FinalWave();
                    phase++;
                }
                spawnTimer += time;
                minuteTimer += time;
                if (spawnTimer >= timeBetweenSpawns)
                {
                    SpawnWave();
                    SpawnWaveSpiders();
                    spawnTimer = 0;
                }

                if (minuteTimer >= 60000)
                {
                    SpawnWaveBeetles();
                    minuteTimer = 0;
                }
                //3 min mark
                if (waveTimer >= 180000 && phase == 0)
                {
                    onCooldown = true;
                    timeBetweenSpawns = 2000;
                    player->weaponAmmunition[WEAPON_BLOCKGUN] += 5;
                    phase++;
                }
                //6min
                if (waveTimer >= 360000 && phase == 1)
                {
                    onCooldown = true;
                    timeBetweenSpawns = 1500;
                    player->weaponAmmunition[WEAPON_BLOCKGUN] += 5;
                    phase++;
                    
                }
                //9 min rat phase
                if (waveTimer >= 540000 && phase == 2)
                {
                    onCooldown = true;
                    player->weaponAmmunition[WEAPON_BLOCKGUN] += 5;
                    timeBetweenSpawns = 1000;
                    phase++;
                }                
            }
        }
    }   
    else
    {
        cooldownTimer += time;
        //std::cout << cooldownTimer << std::endl;
        if (cooldownTimer >= 60000)
        {
            onCooldown = false;
            std::cout << phase << std::endl;
            cooldownTimer = 0;
        }
    }
}
void SpawnManager::SpawnWaveSpiders()
{
    s32 randomNum = getRandom(15);
    if (randomNum == 1)
    {
        SpawnSpider(RandomPoint());
    }
}
void SpawnManager::SpawnWaveBeetles() 
{
    SpawnBeetle(RandomPoint());
    SpawnBeetle(RandomPoint());
    SpawnBeetle(RandomPoint());
}
void SpawnManager::SpawnWave()
{
    //fairly self explanatory, 1/4 chance to get 1 Cockroach at a random point, 1/4 chance to get 2 Cockroaches
    // 1/4 chance to get 3 Cockroaches, 1/4 chances for no spawn
    int rand1 = getRandom(4);
    if (rand1 == 1)
    {
        SpawnCockroach(RandomPoint());
    }
    else if (rand1 == 2)
    {
        SpawnCockroach(RandomPoint());
        SpawnCockroach(RandomPoint());
        
    }
    else if (rand1 == 3)
    {
        SpawnCockroach(RandomPoint());
        SpawnCockroach(RandomPoint());
        SpawnCockroach(RandomPoint());
    }
}

s32 SpawnManager::getRandom( s32 upper )
{
    return (rand() % upper) + 1;
}

void SpawnManager::SpawnCockroach( vector3df position )
{
    Enemy* enemy = new Cockroach(smgr, soundEngine, cockroachMesh, physxManager, enemyObjects, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::SpawnBeetle( vector3df position )
{
    Enemy* enemy = new Beetle(smgr, soundEngine, beetleMesh, physxManager, enemyObjects, blockArray, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::SpawnSpider( vector3df position )
{
    Enemy* enemy = new Spider(smgr, soundEngine, spiderMesh, physxManager, enemyObjects, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::SpawnRat( vector3df position )
{
    Enemy* enemy = new Rat(smgr, soundEngine, ratMesh, physxManager, enemyObjects, player, explosionTextures, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::DIRTYMESHFIX()
{
    enemyObjects->push_back( new Spider(smgr, soundEngine, spiderMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Cockroach(smgr, soundEngine, cockroachMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Beetle(smgr, soundEngine, beetleMesh, physxManager, enemyObjects, blockArray, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Rat(smgr, soundEngine, ratMesh, physxManager, enemyObjects, NULL, explosionTextures, vector3df(0.0f, -1050.0f, 0.0f)));
}

irr::core::vector3df SpawnManager::RandomPointLower()
{
    s32 randomNum = getRandom(NUMBER_OF_POSITIONS - 2) - 1;
    switch (randomNum)
    {
    case MICROWAVE_LOWER:
        {
            vector3df position = spawnPosition[MICROWAVE_LOWER];
            position.Z = (float)(-308 - getRandom(582));
            return position;
        }
        break;
    case STOVE_LOWER:
        {
            vector3df position = spawnPosition[STOVE_LOWER];
            position.X = (float)(-805 - getRandom(1150));
            return position;
        }
        break;
    case CUPBOARD_LOWER:
        return spawnPosition[CUPBOARD_LOWER];
        break;
    case FRIDGE_LOWER:
        {
            vector3df position = spawnPosition[FRIDGE_LOWER];
            position.X = (float)(164 + getRandom(931));
            return position;
        }
        break;
    default:
        return spawnPosition[CUPBOARD_LOWER];
        break; 
    }
}

irr::core::vector3df SpawnManager::RandomPoint()
{
    s32 randomNum = getRandom(NUMBER_OF_POSITIONS);
    randomNum--;
    if (spawnLocationCooldown[randomNum] <= 0)
    {
        switch (randomNum)
        {
        case BASIN_HIGH:
            return spawnPosition[BASIN_HIGH];
            break;
        case MICROWAVE_HIGH:
            return spawnPosition[MICROWAVE_HIGH];
            break;
        case MICROWAVE_LOWER:
            {
                vector3df position = spawnPosition[MICROWAVE_LOWER];
                position.Z = (float)(-308 - getRandom(582));
                return position;
            }
            break;
        case STOVE_LOWER:
            {
                vector3df position = spawnPosition[STOVE_LOWER];
                position.X = (float)(-805 - getRandom(1150));
                return position;
            }
            break;
        case CUPBOARD_LOWER:
            return spawnPosition[CUPBOARD_LOWER];
            break;
        case FRIDGE_LOWER:
            {
                vector3df position = spawnPosition[FRIDGE_LOWER];
                position.X = (float)(164 + getRandom(931));
                return position;
            }
            break; 
        default:
            return spawnPosition[MICROWAVE_HIGH];
            break;  
        }
        spawnLocationCooldown[randomNum] = 500;
    }
    else
    {
        return RandomPointLower();
    }
}
void SpawnManager::FinalWave() 
{
    //spawn rat
    this->SpawnRat(vector3df(1500.0f,100.0f,-800.0f));
}