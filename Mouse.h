#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include <SDL.h>

class Player;
class World;

class Mouse
{
    public:

    Mouse();
    ~Mouse();
    void update(World &world, Player &player);
    float getDelta() const;
    float getTheta() const;
    float getLeftClick() const;
    void setLeftClick(bool state);
    float getRightClick() const;
    void setRightClick(bool state);

    private:

    SDL_Event m_event;
    float m_sensitivity;
    float m_x;
    float m_y;
    bool m_leftClick;
    bool m_rightClick;
    bool m_wheelUp;
    bool m_wheelDown;
    float m_delta;
    float m_theta;
    float m_windowHeight;
};

#endif // MOUSE_H_INCLUDED
