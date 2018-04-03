#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <SDL.h>
#include <GL/glu.h>

class Mouse;
class Player;

struct Position
{
    float x;
    float y;
    float z;
};

struct Chunk
{
    float *vertex[6];
    float *coordonneTexture[6];
    GLubyte *couleur[6];
    int nombreFace[6];
};

class World
{
    public:

    World(int x, int y, int z);
    ~World();
    int* getBlocks() const;
    void setBlock(int index, int type);
    struct Chunk* getChunks() const;
    void setChunk(struct Chunk Chunk, int index);
    void generate();
    void addTree(int X, int Y, int Z);
    void load();
    struct Chunk updateChunk(int chunkIndex);
    bool isBlock(int type);
    void blockColor(int &blockType, int &r, int &g, int &b, int face, int x, int y, int z);
    void blockColorWithShadow(int &nombreCube, int r, int g, int b, int &ombreR, int &ombreG, int &ombreB);
    void colorLimit(int &red, int &green, int &blue);
    void updateBlock(int x, int y, int z);
    void addBlock(Mouse &mouse, Player &player);
    void removeBlock(Mouse &mouse, Player &player);
    float distance3D(float x, float y, float z, float X, float Y, float Z);
    struct Position getTargetPosition(float cameraX, float cameraY, float cameraZ, int mode);
    void save();
    int getX() const;
    int getY() const;
    int getZ() const;
    int getChunkSize() const;
    int getChunkX() const;
    int getChunkY() const;
    int getChunkZ() const;

    private:

    const int m_x;
    const int m_y;
    const int m_z;
    int* m_blocks;
    const int m_chunkSize;
    const int m_chunkX;
    const int m_chunkY;
    const int m_chunkZ;
    const int m_chunkCount;
    Chunk* m_chunks;
    int m_blockAddingX;
    int m_blockAddingY;
    int m_blockAddingZ;
    int m_blockRemovalX;
    int m_blockRemovalY;
    int m_blockRemovalZ;
    int m_permutations[512];
};

#endif // WORLD_H_INCLUDED
