#include <SDL.h>
#include <cmath>
#include <iostream>
#include "Player.h"
#include "World.h"

using namespace std;

Player::Player() :
    m_cameraX(128.0f),
    m_cameraY(128.0f),
    m_cameraZ(64.0f),
    m_z(32.0f),
    m_speed(0.1f),
    m_gravityEnabled(true),
    m_fall(0.0f),
    m_crouch(false),
    m_eye(1.5f),
    m_height(m_eye + 0.2f),
    m_halfWidth(0.4f),
    m_onTheGround(false),
    m_jump(false),
    m_alreadyJumped(false)
{

}

Player::~Player()
{

}

void Player::updateVectors(Mouse &mouse)
{
    m_targetX = 4 * cos(mouse.getTheta() * M_PI / 180.0f) * cos(mouse.getDelta() * M_PI / 180.0f);
    m_targetY = 4 * sin(mouse.getTheta() * M_PI / 180.0f) * cos(mouse.getDelta() * M_PI / 180.0f);
    m_targetZ = 4 * sin(mouse.getDelta() * M_PI / 180.0f);

    m_forwardX = m_targetX / 4.0f * m_speed;
    m_forwardY = m_targetY / 4.0f * m_speed;
    m_forwardZ = m_targetZ / 4.0f * m_speed;

    // Mouvements latéraux
    // pour faire un pas chassé à gauche ou à droite
    // on fait le produit vectoriel du vecteur cible et du vecteur vertical qui va donner un vecteur orthogonal
    m_sideX = -1.0f * m_targetY;
    m_sideY = 1.0f * m_targetX;
    m_sideZ = 0.0f;

    // on normalise
    float length  = sqrt(m_sideX * m_sideX + m_sideY * m_sideY);
    //cout << length << endl;
    m_sideX = m_sideX / length * m_speed;
    m_sideY = m_sideY / length * m_speed;

}

void Player::updatePosition(World &world, Keyboard &keyboard)
{
    // on recopie la position du joueur pour les tests de collisions
    m_cameraXTest = m_cameraX;
    m_cameraYTest = m_cameraY;
    m_cameraZTest = m_z;

    // gravité
    if(m_gravityEnabled)
    {
        // à chaque frame le déplacement en Z est plus important
        m_fall -= 0.015f;

        // vitesse de chute limitée
        // on ne va pas en-dessous de -0.5 pour éviter les bugs de collision
        if (m_fall < -0.5f)
        {
            m_fall = -0.5f;
        }

        m_cameraZTest += m_fall;
    }

    // déplacements
    Uint8* keysStates = keyboard.getKeysStates();
    int* keys = keyboard.getKeys();

    if (keysStates[keys[0]])
    {
        m_cameraXTest += m_forwardX;
        m_cameraYTest += m_forwardY;

        if (!m_gravityEnabled) {
            m_cameraZTest += m_forwardZ;
        }
    }

    if (keysStates[keys[1]])
    {
        m_cameraXTest -= m_forwardX;
        m_cameraYTest -= m_forwardY;

        if (!m_gravityEnabled) {
            m_cameraZTest -= m_forwardZ;
        }
    }

    if (keysStates[keys[2]])
    {
        m_cameraXTest += m_sideX;
        m_cameraYTest += m_sideY;
    }

    if (keysStates[keys[3]])
    {
        m_cameraXTest -= m_sideX;
        m_cameraYTest -= m_sideY;
    }

    // sauter
    if (keysStates[keys[4]]) {
        m_jump = true;
    }
    else {
        m_jump = false;
    }

    if (m_onTheGround)
    {
        if (m_jump)
        {
            if(!m_alreadyJumped)
            {
                m_fall = 0.18f;
                m_alreadyJumped = true;
            }
        }
        else
        {
            m_alreadyJumped = false;
        }
    }

    m_cameraZTest += m_fall;

    // s'accroupir
    if (keysStates[keys[5]]) {
        m_crouch = true;
    }
    else {
        m_crouch = false;
    }

    // Limites de déplacement

    // x-
    if (m_cameraXTest < 1) {
        m_cameraXTest = 1;
    }

    // x+
    if (m_cameraXTest > world.getX() - 1) {
        m_cameraXTest = world.getX() - 1;
    }

    // y-
    if (m_cameraYTest < 1) {
        m_cameraYTest = 1;
    }

    // y+
    if (m_cameraYTest > world.getY() - 1) {
        m_cameraYTest = world.getY() - 1;
    }

    // z-
    if (m_cameraZTest < 1) {
        m_cameraZTest = 1;
    }

    // z+
    if (m_cameraZTest > world.getZ() - 1) {
        m_cameraZTest = world.getZ() - 1;
    }
}

void Player::collisionWithWorld(World &world)
{
    int* blocks = world.getBlocks();

    // on limite le calcul des collisions à une certaine zone il est inutile de calculer les collisions avec tous les cubes du jeu
    int x, y, z;
    float X,Y,Z,W,H;

    int xMin, xMax, yMin, yMax, zMin, zMax;
    int distance = 4;

    xMin = m_cameraX - distance;

    if(xMin < 1)
        xMin = 1;

    xMax = m_cameraX + distance;

    if(xMax >= world.getX())
        xMax = world.getX() - 1;

    yMin = m_cameraY - distance;

    if(yMin < 1)
        yMin = 1;

    yMax = m_cameraY + distance;

    if(yMax >= world.getY())
        yMax = world.getY() - 1;

    zMin = m_z - distance;

    if(zMin < 1)
        zMin = 1;

    zMax = m_z + distance;

    if(zMax >= world.getZ())
        zMax = world.getZ() - 1;

    bool collisionX = false;
    bool collisionY = false;
    bool collisionZ = false;

    // collision axe X
    X = m_cameraXTest;
    Y = m_cameraY;
    Z = m_z;
    W = m_halfWidth;
    H = m_height;

    for(z = zMin; z < zMax; z++)
    {
        for(y = yMin; y < yMax; y++)
        {
            for(x = xMin; x < xMax; x++)
            {
                if(world.isBlock(blocks[z * world.getX() * world.getY() + y * world.getX() + x]) == true)
                {
                    if(collisionWithCube(x, y, z, X, Y, Z, W, H) == true)
                    {
                        // gauche
                        if(m_cameraXTest - W <= x + 1.0f
                        && m_cameraX - W > x + 0.6f)
                        {
                            m_cameraX = x + 1.0f + W + 0.001f;
                            collisionX = true;
                            break;
                        }

                        // droite
                        if(m_cameraXTest + W >= x
                        && m_cameraX + W < x + 0.4f)
                        {
                            m_cameraX = x - W - 0.001f;
                            collisionX = true;
                            break;
                        }

                    }
                }
            }

            if(collisionX == true)
            {
                break;
            }
        }

        if(collisionX == true)
        {
            break;
        }
    }

    // collision axe Y
    X = m_cameraX;
    Y = m_cameraYTest;
    Z = m_z;
    W = m_halfWidth;
    H = m_height;

    for(z = zMin; z < zMax; z++)
    {
        for(y = yMin; y < yMax; y++)
        {
            for(x = xMin; x < xMax; x++)
            {
                if(world.isBlock(blocks[z * world.getX() * world.getY() + y * world.getX() + x]) == true)
                {
                    if(collisionWithCube(x, y, z, X, Y, Z, W, H) == true)
                    {
                        // bas
                        if(m_cameraYTest - W <= y + 1.0f
                        && m_cameraY - W > y + 0.6f)
                        {
                            m_cameraY = y + 1.0f + W + 0.001f;
                            collisionY = true;
                            break;
                        }

                        // haut
                        if(m_cameraYTest + W >= y
                        && m_cameraY + W < y + 0.4f)
                        {
                            m_cameraY = y - W - 0.001f;
                            collisionY = true;
                            break;
                        }
                    }
                }
            }

            if(collisionY == true)
            {
                break;
            }
        }

        if(collisionY == true)
        {
            break;
        }
    }

    // position des yeux
    if (m_crouch)
    {
        // on peut se baisser jusqu'à 0.5 au-desus des pieds
        if (m_eye > 0.5f)
        {
            m_eye -= 0.08f;
        }
        else
        {
            m_eye = 0.5f;
        }
    }

    // la caméra est placée au niveau des yeux du personnage
    // cameraZ = position des pieds du joueur + distance entre les yeux et les pieds
    float eyeTest = 0.0f;

    // si on est pas complètement debout
    if (!m_crouch && m_eye < 1.5f)
    {
        eyeTest = m_eye + 0.11f;

        if(eyeTest > 1.5f)
        {
            eyeTest = 1.5f;
        }

        bool collisionHaut = false;

        X = m_cameraX;
        Y = m_cameraY;
        Z = m_z;
        W = m_halfWidth;

        // on calcul la hauteur du joueur
        // eyeTest est la distance des pieds aux yeux
        // 0.3f est la distance entre les yeux et le haut de la tête
        H = eyeTest + 0.3f;

        for(z = zMin; z < zMax; z++)
        {
            for(y = yMin; y < yMax; y++)
            {
                for(x = xMin; x < xMax; x++)
                {
                    if(world.isBlock(blocks[z * world.getX() * world.getY() + y * world.getX() + x]) == true)
                    {
                        if(collisionWithCube(x, y, z, X, Y, Z, W, H) == true)
                        {
                            collisionHaut = true;
                            break;
                        }
                    }
                }

                if(collisionHaut == true)
                {
                    break;
                }
            }

            if(collisionHaut == true)
            {
                break;
            }
        }

        if(collisionHaut == false) // s'il n'y a rien qui gêne
        {
            m_eye = eyeTest;
        }
    }

    m_cameraZ = m_z + m_eye;

    // collision axe Z
    m_height = m_eye + 0.2f; // à 0.3 le joueur se retrouve dans le cube au-dessus quand il saute

    X = m_cameraX;
    Y = m_cameraY;

    Z = m_cameraZTest;
    W = m_halfWidth;
    H = m_height;

    collisionZ = false;
    m_onTheGround = false;

    for(z = zMin; z < zMax; z++)
    {
        for(y = yMin; y < yMax; y++)
        {
            for(x = xMin; x < xMax; x++)
            {
                if(world.isBlock(blocks[z * world.getX() * world.getY() + y * world.getX() + x]) == true)
                {
                    if(collisionWithCube(x, y, z, X, Y, Z, W, H) == true)
                    {
                        // si le test rentre dans le cube
                        // ET que le joueur est actuellement au-dessus
                        if(m_cameraZTest <= z + 1
                        && m_z > z + 1)
                        {
                            collisionZ = true;
                            m_onTheGround = true;
                            break;
                        }

                        // on doit faire ce test dans le cas où le joueur essaie de sauter
                        // si le test rentre dans le cube
                        // ET que le joueur est actuellement en-dessous
                        if(m_cameraZTest + m_height >= z
                        && m_z + m_height < z)
                        {
                            collisionZ = true;
                            break;
                        }

                    }
                }
            }

            if(collisionZ == true)
            {
                break;
            }
        }

        if(collisionZ == true)
        {
            break;
        }
    }

    if(collisionX == false)
    {
        m_cameraX = m_cameraXTest;
    }

    if(collisionY == false)
    {
        m_cameraY = m_cameraYTest;
    }

    if(collisionZ == false)
    {
        m_z = m_cameraZTest;
    }
    else
    {
        // lorsqu'on touche le sol on met à zéro la vitesse de chute
        m_fall = 0.0f;
    }
}

bool Player::collisionWithCube(float x, float y, float z, float X, float Y, float Z, float W, float H)
{
    // x,y,z sont les coordonnées du coin (0,0,0) d'un cube
    if(X - W > x + 1.0f
    || X + W < x

    || Y - W > y + 1.0f
    || Y + W < y

    || Z > z + 1.0f
    || Z + H < z)
    {
        return false;
    }
    else
    {
        return true;
    }
}

float Player::getCameraX() const
{
    return m_cameraX;
}

float Player::getCameraY() const
{
    return m_cameraY;
}

float Player::getCameraZ() const
{
    return m_cameraZ;
}

float Player::getZ() const
{
    return m_z;
}

float Player::getTargetX() const
{
    return m_targetX;
}

float Player::getTargetY() const
{
    return m_targetY;
}

float Player::getTargetZ() const
{
    return m_targetZ;
}
