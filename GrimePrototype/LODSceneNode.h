#pragma once
#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;

class CLODSceneNode : public scene::ISceneNode
{

    class Vertex;
    class Triangle;
    class Triangle {
    public:
        Vertex* vertex[3];
        Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
            vertex[0] = v1;
            vertex[1] = v2;
            vertex[2] = v3;
            for (u32 x = 0; x < 3; x++) {
                vertex[x]->face.push_back(this);
                for (u32 y = 0; y < 3; y++) {
                    bool add = true;
                    for(u32 i = 0; i < vertex[x]->neighbors.size(); i++) {
                        if (vertex[y]->id  == vertex[x]->neighbors[i]->id || y == x) {
                            add = false;			
                        }
                    }
                    if (add) {
                        vertex[x]->neighbors.push_back(vertex[y]);

                    }
                }
            }
        }
        bool HasVertex(Vertex* v) { return (v->position  == vertex[0]->position || v->position == vertex[1]->position  || v->position == vertex[2]->position ); };
        void ReplaceVertex(Vertex* target, Vertex* v) {
            for (int x = 0; x < 3; x++)
                if (vertex[x]->position == target->position ) {
                    vertex[x] = v;

                }
        }
        bool CheckAndRemove(int num, array<Triangle*> *list) {
            if (vertex[0]->position == vertex[1]->position || vertex[1]->position == vertex[2]->position || vertex[2]->position == vertex[0]->position) {
                list->erase(num);
                return true;
            }
        }
    };
    class Vertex {
    public:
        vector3df position;
        array<Vertex*> neighbors;
        array<Triangle*> face;
        int id;
    };



    IMeshBuffer* BuildMeshBufferFromTriangles(IMeshBuffer* oldmb, array<Triangle*> arr) {
        SMeshBuffer* newm = new SMeshBuffer();
        newm->Material = oldmb->getMaterial();
        u16* indices = new u16[arr.size()*3];
        for (u32 x = 0; x < arr.size(); x++) {
            for (u32 y = 0; y < 3; y++) {
                indices[x*3+y] = arr[x]->vertex[y]->id;
            }
        }
        newm->append(oldmb->getVertices(),oldmb->getVertexCount(),indices,arr.size()*3);
        return newm;
    }



    IMesh* DefaultMesh;
    IMesh** LODMesh;
    IMesh* CurrentMesh;
    IMeshBuffer* test;
    char CurrentLevel;
    char LevelCount;
    float LODBegin;
    float LODLast;
    bool LODOn;
    array<Vertex*> Verts[8];
    array<Triangle*> Tris[8];
public:

    CLODSceneNode(IMesh* mesh, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, int numOfCollapseOnLast, char numOfLevels = 4, float LODBeginDist = 10, float LODLastDist = 30, bool combineDuplicateVertices = true)
        : scene::ISceneNode(parent, mgr, id)
    {
        DefaultMesh = mesh;
        CurrentMesh = mesh;
        //CurrentMesh->getMeshBuffer(0)->getMaterial().Wireframe = true;
        LODBegin = LODBeginDist;
        LODLast = LODLastDist;
        LevelCount = numOfLevels;
        CurrentLevel = 0;
        LODMesh = new IMesh*[numOfLevels];
        LODOn = true;
        if (combineDuplicateVertices)
            DefaultMesh = SceneManager->getMeshManipulator()->createMeshWelded(DefaultMesh);


        for (u32 x = 0; x < DefaultMesh->getMeshBufferCount(); x++) {
            S3DVertex* verts = (S3DVertex*)DefaultMesh->getMeshBuffer(x)->getVertices();


            for (u32 y = 0; y < DefaultMesh->getMeshBuffer(x)->getVertexCount(); y++) {
                Vertex* vert = new Vertex();
                vert->position = verts[y].Pos;
                vert->id = y;
                Verts[x].push_back(vert);
            }

        }

        for (u32 x = 0; x < DefaultMesh->getMeshBufferCount(); x++) {
            u16* indices = DefaultMesh->getMeshBuffer(x)->getIndices();

            for (u32 y = 0; y < DefaultMesh->getMeshBuffer(x)->getIndexCount(); y += 3) {
                Triangle *tri = new Triangle(Verts[x][indices[y]],Verts[x][indices[y+1]],Verts[x][indices[y+2]]);

                Tris[x].push_back(tri);
            }

        }

        for (int x = 0; x < LevelCount; x++)
            LODMesh[x] = DefaultMesh;


        int collapsePerLevel = numOfCollapseOnLast/(LevelCount-1);
        int collapsePerBuffer = collapsePerLevel/DefaultMesh->getMeshBufferCount();


        for (s32 x = 1; x < LevelCount; x++) {
            SMesh* newLod = new SMesh();
            for (u32 y = 0; y < DefaultMesh->getMeshBufferCount(); y++) {
                for (s32 i = 0; i < collapsePerBuffer; i++) {
                    int vert = rand()%Verts[y].size();
                    int neighbor = rand()%Verts[y][vert]->neighbors.size();
                    for (u32 u = 0; u < Tris[y].size(); u++) {
                        if (Tris[y][u]->HasVertex(Verts[y][vert])) {
                            Tris[y][u]->ReplaceVertex(Verts[y][vert],Verts[y][vert]->neighbors[neighbor]);

                        }
                    }
                }
                SMeshBuffer* mn = new SMeshBuffer();
                for (u32 x = 0; x < Tris[0].size(); x++) {
                    if (Tris[0][x]->CheckAndRemove(x,&Tris[0])) {
                        x--;
                    }
                }
                mn = (SMeshBuffer*)BuildMeshBufferFromTriangles(DefaultMesh->getMeshBuffer(y),Tris[y]);
                newLod->addMeshBuffer(mn);
                newLod->recalculateBoundingBox();
            }
            LODMesh[x] = newLod;

        }







    }


    virtual void OnRegisterSceneNode()
    {
        if (IsVisible)
            SceneManager->registerNodeForRendering(this);

        ISceneNode::OnRegisterSceneNode();
    }


    virtual void render()
    {
        if (LODOn) {
            vector3df cameraPos = SceneManager->getActiveCamera()->getPosition();
            f32 dist = AbsoluteTransformation.getTranslation().getDistanceFrom(cameraPos);
            CurrentLevel = 0;
            float increment = (LODLast-LODBegin)/LevelCount;
            for (int x = 0; x < LevelCount; x++) {
                if (dist >= (LODBegin+increment*x)*(LODBegin+increment*x))
                    CurrentLevel = x;
            }
            CurrentMesh = LODMesh[CurrentLevel];

        }
        else
            CurrentMesh = DefaultMesh;
        IVideoDriver* driver = SceneManager->getVideoDriver();
        driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
        for (u32 x = 0; x < CurrentMesh->getMeshBufferCount(); x++) {
            driver->setMaterial(CurrentMesh->getMeshBuffer(x)->getMaterial());
            driver->drawMeshBuffer(CurrentMesh->getMeshBuffer(x));
        }
    }

    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return CurrentMesh->getBoundingBox();
    }

    void setLODOn(bool on) { LODOn = on; };
    bool getLODOn() { return LODOn; };

};