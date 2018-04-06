#include <iostream>
#include <ctime>
#include "texture.h"
#include "Display2D.h"
#include "Display3D.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Player.h"
#include "World.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Console
    freopen( "CON", "w", stdout ); // redirige stdout vers la console au lieu du fichier bin/Debug/stdout.txt
    freopen( "CON", "w", stderr );

    // SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    atexit(SDL_Quit);

    // Fenêtre
    putenv("SDL_VIDEO_WINDOW_POS=10, 30"); // pour placer la fenêtre en (x,y), "center" pour centrer la fenêtre
    SDL_WM_SetCaption("Voxel", NULL); // titre et icône de la fenêtre
    SDL_SetVideoMode(1024, 768, 32, SDL_OPENGL); // résolution de démarrage

    Keyboard keyboard;
    Mouse mouse;
    Player player;

    World world(256, 256, 64);
    world.load();

    Display3D display3D;
    Display2D display2D;

    int start = 0;
    int delta = 0;
    bool run = true;

    while(run) {
        start = SDL_GetTicks();

        if (keyboard.getKeysStates()[SDLK_ESCAPE] == 1) {
            //world.save();
            run = false;
        }

        player.updateVectors(mouse);
        player.updatePosition(world, keyboard);
        player.collisionWithWorld(world);

        display3D.init();
        display3D.renderSkybox(player);
        display3D.camera(player);
        display3D.fog();
        display3D.renderWorld(world, player);

        mouse.update(world, player); // maj de la souris après le rendu 3D pour le bloc cible
        world.addBlock(mouse, player);
        world.removeBlock(mouse, player);

        display2D.init();
        display2D.crosshair();

        glFlush();
        SDL_GL_SwapBuffers();

        // ne pas mettre plus de 16, sinon un balayage vertical apparaît
        delta = SDL_GetTicks() - start;

        if (delta < 16) {
            SDL_Delay(16 - delta);
        }
    }

    return 0;
}
