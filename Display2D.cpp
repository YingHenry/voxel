#include <GL/glu.h>
#include "Display2D.h"
#include "texture.h"

Display2D::Display2D() :
    m_width(1024.0f),
    m_height(768.0f)
{
    m_textureTarget = SdlToOpenGl(LoadBMP("images/target.bmp", 1));
}

Display2D::~Display2D()
{

}

void Display2D::init()
{
    glDisable(GL_DEPTH_TEST); // IMPORTANT pour afficher correctement dans l'ordre avec la transparence
    glEnable(GL_TEXTURE_2D); // activation du texturing
    glEnable(GL_BLEND); // activation de la transparence totale ATTENTION désactive les shaders
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, m_width, 0.0f, m_height); // en gluOrtho2D 1 unité vaut 1 pixel
    glViewport(0.0f, 0.0f, m_width, m_height);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void Display2D::crosshair()
{
    float x, y, w, h;
    x = m_width / 2.0f - 17.0f;
    y = m_height / 2.0f - 17.0f;
    w = 34.0f;
    h = 34.0f;

    glColor3ub(0, 170, 0);
    glBindTexture(GL_TEXTURE_2D, m_textureTarget);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);      glVertex2f(x, y+h);
        glTexCoord2f(0.0f, 0.0f);      glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f);      glVertex2f(x+w, y);
        glTexCoord2f(1.0f, 1.0f);      glVertex2f(x+w, y+h);
    glEnd();
}
