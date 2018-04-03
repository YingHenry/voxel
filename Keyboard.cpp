#include <SDL.h>
#include "Keyboard.h"
#include <iostream>

using namespace std;

Keyboard::Keyboard()
{
    SDL_EnableKeyRepeat(10, 10);
    SDL_EnableUNICODE(1);
    m_keysStates = SDL_GetKeyState(NULL);
    m_keys[0] = 122; // haut (z)
    //m_keys[0] = 119; // haut (w)
    m_keys[1] = 115; // bas (s)
    m_keys[2] = 113; // gauche (q)
    //m_keys[2] = 97; // gauche (a)
    m_keys[3] = 100; // droite (d)
    m_keys[4] = 32; // sauter (espace)
    m_keys[5] = 304; // s'accroupir (left shift)
}

Keyboard::~Keyboard()
{

}

Uint8* Keyboard::getKeysStates()
{
    return m_keysStates;
}

int* Keyboard::getKeys()
{
    return m_keys;
}
