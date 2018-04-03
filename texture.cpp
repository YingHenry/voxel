#include <string>
#include <SDL_rotozoom.h>
#include <GL/glu.h>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

using namespace std;

SDL_Surface* LoadBMP(string chemin, int zoom)
{
    const char* chaineChar = &chemin[0];

    SDL_Surface* surface = SDL_LoadBMP(chaineChar);

    if (surface == NULL)
    {
        return NULL;
    }

    // transparence
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, SDL_MapRGB(surface->format, 0, 255, 0));

    // zoom
    surface = rotozoomSurface(surface, 0, zoom, 0);

    return surface;
}

SDL_Surface * flipSurface(SDL_Surface * surface)
{
    int current_line,pitch;
    SDL_Surface * fliped_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                   surface->w,surface->h,
                                   surface->format->BitsPerPixel,
                                   surface->format->Rmask,
                                   surface->format->Gmask,
                                   surface->format->Bmask,
                                   surface->format->Amask);



    SDL_LockSurface(surface);
    SDL_LockSurface(fliped_surface);

    pitch = surface->pitch;

    for (current_line = 0; current_line < surface->h; current_line ++)
    {
        memcpy(&((unsigned char* )fliped_surface->pixels)[current_line*pitch],
               &((unsigned char* )surface->pixels)[(surface->h - 1 - current_line) * pitch],
               pitch);
    }

    SDL_UnlockSurface(fliped_surface);
    SDL_UnlockSurface(surface);
    return fliped_surface;
}

GLuint SdlToOpenGl(SDL_Surface *picture_surface)
{
    GLuint glID;

    SDL_Surface *gl_surface = NULL;
    SDL_Surface * gl_fliped_surface = NULL;
    Uint32 rmask, gmask, bmask, amask;

    if (picture_surface == NULL)
        return 0;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else

    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_PixelFormat format = *(picture_surface->format);
    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;
    format.Rmask = rmask;
    format.Gmask = gmask;
    format.Bmask = bmask;
    format.Amask = amask;

    gl_surface = SDL_ConvertSurface(picture_surface,&format,SDL_SWSURFACE);

    gl_fliped_surface = flipSurface(gl_surface);

    glGenTextures(1, &glID);

    glBindTexture(GL_TEXTURE_2D, glID);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, gl_fliped_surface->w, gl_fliped_surface->h, 0, GL_RGBA,GL_UNSIGNED_BYTE, gl_fliped_surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // filtrage des textures proches
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // filtrage des textures éloignées

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_FreeSurface(gl_surface);
    SDL_FreeSurface(gl_fliped_surface);

    return glID;
}


