#include <SDL.h>
#include "Mouse.h"
#include "Player.h"
#include <iostream>

using namespace std;

Mouse::Mouse() :
    m_sensitivity(2.0f),
    m_x(SDL_GetVideoSurface()->w / 2.0f),
    m_y(SDL_GetVideoSurface()->h / 2.0f),
    m_leftClick(false),
    m_rightClick(false),
    m_wheelUp(false),
    m_wheelDown(false),
    m_delta(0.0f),
    m_theta(0.0f),
    m_windowHeight(SDL_GetVideoSurface()->h)
{
    SDL_WM_GrabInput(SDL_GRAB_ON); // ON : la souris reste bloquée dans la fenêtre de jeu
    SDL_ShowCursor(SDL_DISABLE); // SDL_ENABLE : montre le curseur de la souris
    SDL_WarpMouse(m_x, m_y); // lieu d'apparition de la souris
}

Mouse::~Mouse()
{

}

void Mouse::update(World &world, Player &player)
{
    while(SDL_PollEvent(&m_event))
    {
        switch(m_event.type)
        {
            case SDL_QUIT:
            {
                break;
            }

            case SDL_MOUSEMOTION:
                m_delta -= m_event.motion.yrel * m_sensitivity * 0.1;

                if (m_delta > 89)
                {
                    m_delta = 89;
                }

                if (m_delta < -89)
                {
                    m_delta = -89;
                }

                m_theta -= m_event.motion.xrel * m_sensitivity * 0.1;

                m_x = m_event.motion.x;
                m_y = m_windowHeight - m_event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                // m_leftClick vaut true si on a pas appuyé sur le bouton gauche la frame précédente sinon elle vaut false pour éviter les répétitions
                if(m_event.button.type == SDL_MOUSEBUTTONDOWN && m_event.button.button == SDL_BUTTON_LEFT && m_leftClick == false)
                {
                    m_leftClick = true;
                }
                else
                {
                    m_leftClick = false;
                }

                if(m_event.button.type == SDL_MOUSEBUTTONDOWN && m_event.button.button == SDL_BUTTON_RIGHT && m_rightClick == false)
                {
                    m_rightClick = true;
                }
                else
                {
                    m_rightClick = false;
                }

                if (m_event.button.button == SDL_BUTTON_WHEELUP && !m_wheelUp)
                {
                    m_wheelUp = true;
                }
                else
                {
                    m_wheelUp = false;
                }

                if (m_event.button.button == SDL_BUTTON_WHEELDOWN && !m_wheelDown)
                {
                    m_wheelDown = true;
                }
                else
                {
                    m_wheelDown = false;
                }

                break;
        }
    }
}

float Mouse::getDelta() const
{
    return m_delta;
}

float Mouse::getTheta() const
{
    return m_theta;
}

float Mouse::getLeftClick() const
{
    return m_leftClick;
}

void Mouse::setLeftClick(bool state)
{
    m_leftClick = state;
}

float Mouse::getRightClick() const
{
    return m_rightClick;
}

void Mouse::setRightClick(bool state)
{
    m_rightClick = state;
}
