#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <string>
#include <SDL.h>
#include <GL/glu.h>

SDL_Surface* LoadBMP(std::string chemin, int zoom);
GLuint SdlToOpenGl(SDL_Surface *picture_surface);
SDL_Surface* flipSurface(SDL_Surface * surface);

#endif // TEXTURE_H_INCLUDED
