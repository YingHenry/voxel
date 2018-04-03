#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "Keyboard.h"
#include "Mouse.h"

class World;

class Player
{
    public:

    Player();
    ~Player();
    void updateVectors(Mouse &mouse);
    void updatePosition(World &world, Keyboard &keyboard);
    void collisionWithWorld(World &world);
    bool collisionWithCube(float x, float y, float z, float X, float Y, float Z, float W, float H);
    float getCameraX() const;
    float getCameraY() const;
    float getCameraZ() const;
    float getZ() const;
    float getTargetX() const;
    float getTargetY() const;
    float getTargetZ() const;

    private:

    float m_cameraX;
    float m_cameraY;
    float m_cameraZ;
    float m_z;
    float m_targetX;
    float m_targetY;
    float m_targetZ;
    float m_speed;
    float m_forwardX;
    float m_forwardY;
    float m_forwardZ;
    float m_sideX;
    float m_sideY;
    float m_sideZ;
    float m_cameraXTest;
    float m_cameraYTest;
    float m_cameraZTest;
    bool m_gravityEnabled;
    float m_fall;
    float m_crouch;
    float m_eye;
    float m_height;
    float m_halfWidth;
    bool m_onTheGround;
    bool m_jump;
    bool m_alreadyJumped;
};

#endif // PLAYER_H_INCLUDED
