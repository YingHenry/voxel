#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <SDL.h>
#include "World.h"

class Keyboard
{
    public:

    Keyboard();
    ~Keyboard();
    Uint8* getKeysStates() const;
    const int* getKeys() const;

    private:

    Uint8 *m_keysStates;
    int m_keys[6];
};

#endif // KEYBOARD_H_INCLUDED
