#include <GL/glu.h>
#include "Display3D.h"
#include "texture.h"

Display3D::Display3D() :
    m_FOV(70.0f),
    m_width(1024.0f),
    m_height(768.0f)
{
    m_textureSky = SdlToOpenGl(LoadBMP("images/sky.bmp", 1));
    m_textureBlockXY = SdlToOpenGl(LoadBMP("images/blockXY.bmp", 1));
    m_textureBlockZP = SdlToOpenGl(LoadBMP("images/blockZP.bmp", 1));
    m_textureBlockZN = SdlToOpenGl(LoadBMP("images/blockZN.bmp", 1));
}

Display3D::~Display3D()
{

}

void Display3D::init()
{
    glEnable(GL_DEPTH_TEST); // activation du Z-buffer
    glEnable(GL_TEXTURE_2D); // activation du texturing
    glEnable(GL_BLEND); // activation de la transparence totale ATTENTION désactive les shaders
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // on définit le type de projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(m_FOV, m_width / m_height, 0.1f, 1000.0f);
    glViewport(0.0f, 0.0f, m_width, m_height);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

void Display3D::renderSkybox(Player &player)
{
    glPushMatrix();
    glLoadIdentity();

    gluLookAt(
        0.0f, 0.0f, 0.0f,
        player.getTargetX(), player.getTargetY(), player.getTargetZ(),
        0.0f, 0.0f, 1.0f
    );

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3ub(255, 255, 255);
    glBindTexture(GL_TEXTURE_2D, m_textureSky);

    GLUquadric* params = gluNewQuadric();
    gluQuadricDrawStyle(params,GLU_FILL);
    gluQuadricTexture(params,GL_TRUE);

    // p1 : rayon
    // p2 : tranches verticales
    // p3 : tranches horizontales
    gluSphere(params, 10,20,20);

    gluDeleteQuadric(params);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void Display3D::camera(Player &player)
{
    float targetX = player.getCameraX() + player.getTargetX();
    float targetY = player.getCameraY() + player.getTargetY();
    float targetZ = player.getCameraZ() + player.getTargetZ();

    gluLookAt(
        player.getCameraX(), player.getCameraY(), player.getCameraZ(),
        targetX, targetY, targetZ,
        0.0f, 0.0f, 1.0f
    );
}

void Display3D::fog()
{
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR); // on choisit le mode linéaire

    // couleur
    GLfloat fogColor[4]= {0.9f, 0.9f, 0.9f, 0.1f};
    glFogfv(GL_FOG_COLOR, fogColor);

    // distance
    glFogf(GL_FOG_START, 0);
    glFogf(GL_FOG_END, 250);
}

void Display3D::renderWorld(World &world, Player &player)
{
    int chunkCountX = world.getChunkX();
    int chunkCountY = world.getChunkY();
    int chunkCountZ = world.getChunkZ();
    int chunkSize = world.getChunkSize();
    struct Chunk *chunks = world.getChunks();

    int chunkIndex;
    float X,Y,Z;

    // le backface culling augmente les performances surtout pour les gros polygones
    // sur des carré de taille 1 il n'y a pas de changement
    glEnable(GL_CULL_FACE);
    // on cache la face arrière
    // la face avant est définie par des vertices dans le sens inverse des aiguilles d'une montre
    glCullFace(GL_BACK);

    // on parcourt la liste des morceaux du monde
    for (int z = 0; z < chunkCountZ; z++)
    for (int y = 0; y < chunkCountY; y++)
    for (int x = 0; x < chunkCountX; x++)
    {
        chunkIndex = z * chunkCountX * chunkCountY + y * chunkCountX + x;

        // on se déplace au centre du morceau
        X = x * chunkSize + chunkSize / 2.0f;
        Y = y * chunkSize + chunkSize / 2.0f;
        Z = z * chunkSize + chunkSize / 2.0f;

        if(chunks[chunkIndex].faceCounts[0] > 0 && player.getCameraX() >= x * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[0]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[0]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[0]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockXY);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[0] * 4);
        }

        if(chunks[chunkIndex].faceCounts[1] > 0 && player.getCameraX() <= (x+1) * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[1]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[1]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[1]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockXY);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[1] * 4);
        }

        if(chunks[chunkIndex].faceCounts[2] > 0 && player.getCameraY() >= y * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[2]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[2]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[2]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockXY);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[2] * 4);
        }

        if(chunks[chunkIndex].faceCounts[3] > 0 && player.getCameraY() <= (y+1) * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[3]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[3]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[3]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockXY);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[3] * 4);
        }

        if(chunks[chunkIndex].faceCounts[4] > 0 && player.getCameraZ() >= z * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[4]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[4]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[4]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockZP);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[4] * 4);
        }

        if(chunks[chunkIndex].faceCounts[5] > 0 && player.getCameraZ() <= (z+1) * chunkSize)
        {
            glVertexPointer(3, GL_FLOAT, 3 * sizeof(GLfloat), chunks[chunkIndex].vertices[5]);
            glColorPointer(3, GL_UNSIGNED_BYTE  , 3 * sizeof(GLubyte), chunks[chunkIndex].colors[5]);
            glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), chunks[chunkIndex].texturesCoordinates[5]);
            glBindTexture(GL_TEXTURE_2D, m_textureBlockZN);
            glDrawArrays(GL_QUADS, 0, chunks[chunkIndex].faceCounts[5] * 4);
        }
    }

    glDisable(GL_CULL_FACE);
}
