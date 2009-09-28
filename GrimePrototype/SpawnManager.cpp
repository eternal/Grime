#include "SpawnManager.h"

SpawnManager::SpawnManager( ISceneManager* smgr, ISoundEngine* soundEngine, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects, Player* player )
{
    this->smgr = smgr;
    this->physxManager = physxManager;
    this->enemyObjects = enemyObjects;
    this->timeBetweenSpawns = timeBetweenSpawns;
    this->soundEngine = soundEngine;
    this->player = player;
    currentTimer = 0;
    spawnTimer = 0;
    waveTimer = 0;
    minuteTimer = 0;
    timeBetweenSpawns = 2500;
    spawnsActive = false;
    
    positionOne = vector3df(-789.5f,30.0f,1080.842f);
    positionTwo = vector3df(-1955.101f,30.0f,-1432.715f);
    positionThree = vector3df(950.0f,720.1f,-1125.0f);
    
    //cockroachMesh = smgr->getMesh("media/cockroack_rigged_a02.x"); COMMENTED OUT UNTIL LOWPOLY MESH ARRIVES
    cockroachMesh = smgr->getMesh("media/spiderupdated.x");
    spiderMesh = smgr->getMesh("media/spiderupdated.x");
    ratMesh = smgr->getMesh("media/rat.x");
    beetleMesh = smgr->getMesh("media/beetle2.x");
}
SpawnManager::~SpawnManager(void)
{
}

void SpawnManager::Update( s32 time )
{
    currentTimer += time;
    waveTimer += time;
    if (currentTimer >= 100) {
        timeBetweenSpawns--;
        currentTimer = 0;
    }
    if (!spawnsActive)
    {
        if (waveTimer >= 30000)
        {
            spawnsActive = true;
        }
    }
    if (spawnsActive)
    {
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
    }
    
   
    
}
void SpawnManager::SpawnWaveSpiders()
{
    s32 randomNum = getRandom(30);
    if (randomNum == 1)
    {
        SpawnSpider(RandomPoint());
    }
}
void SpawnManager::SpawnWaveBeetles() 
{
    SpawnBeetle(positionOne);
    SpawnBeetle(positionTwo);
    SpawnBeetle(positionThree);
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
        int rand2 = getRandom(3);
        if (rand2 == 1)
        {
            SpawnCockroach(positionOne);
            SpawnCockroach(positionTwo);
        }
        if (rand2 == 2)
        {
            SpawnCockroach(positionOne);
            SpawnCockroach(positionThree);
        }
        if (rand2 == 3)
        {
            SpawnCockroach(positionThree);
            SpawnCockroach(positionTwo);
        }
    }
    else if (rand1 == 3)
    {
        SpawnCockroach(positionOne);
        SpawnCockroach(positionTwo);
        SpawnCockroach(positionThree);
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
    Enemy* enemy = new Beetle(smgr, soundEngine, beetleMesh, physxManager, enemyObjects, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::SpawnSpider( vector3df position )
{
    Enemy* enemy = new Spider(smgr, soundEngine, spiderMesh, physxManager, enemyObjects, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::SpawnRat( vector3df position )
{
    Enemy* enemy = new Rat(smgr, soundEngine, ratMesh, physxManager, enemyObjects, player, position);
    enemyObjects->push_back(enemy);
}

void SpawnManager::DIRTYMESHFIX()
{
    enemyObjects->push_back( new Enemy(smgr, soundEngine, spiderMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Enemy(smgr, soundEngine, cockroachMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Enemy(smgr, soundEngine, beetleMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
    enemyObjects->push_back( new Enemy(smgr, soundEngine, ratMesh, physxManager, enemyObjects, NULL, vector3df(0.0f, -1050.0f, 0.0f)));
}

irr::core::vector3df SpawnManager::RandomPoint()
{
    s32 randomNum = getRandom(3);
    if (randomNum == 1)
    {
        return positionOne;
    }
    else if (randomNum == 2)
    {
        return positionTwo;
    }
    else
    {
        return positionThree;
    }
}