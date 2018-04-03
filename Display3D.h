#ifndef DISPLAY3D_H_INCLUDED
#define DISPLAY3D_H_INCLUDED

#include "Player.h"
#include "World.h"

class Player;
class World;

class Display3D
{
    public:

    Display3D();
    ~Display3D();
    void init();
    void renderSkybox(Player &player);
    void camera(Player &player);
    void fog();
    void renderWorld(World &world, Player &player);

    private:

    const float m_FOV;
    const float m_width;
    const float m_height;
    GLuint m_textureSky;
    GLuint m_textureBlockXY;
    GLuint m_textureBlockZP;
    GLuint m_textureBlockZN;
};

#endif // DISPLAY3D_H_INCLUDED
