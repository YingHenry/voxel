#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glu.h>
#include "Mouse.h"
#include "Player.h"
#include "World.h"
#include "simplexNoise.h"

using namespace std;

World::World(int x, int y, int z) :
    m_x(x),
    m_y(y),
    m_z(z),
    m_blocks(new int[x*y*z]),
    m_chunkSize(16),
    m_chunkX(x/16),
    m_chunkY(y/16),
    m_chunkZ(z/16),
    m_chunkCount(m_chunkX * m_chunkY * m_chunkZ),
    m_chunks(new Chunk[m_chunkCount])
{
    for(int i = 0; i < 512; i++)
    {
        m_permutations[i] = rand() % 255 + 0;
    }
}

World::~World()
{
    delete [] m_blocks;
    delete [] m_chunks;
}

int* World::getBlocks() const
{
    return m_blocks;
}

void World::setBlock(int index, int type)
{
    m_blocks[index] = type;
}

Chunk* World::getChunks() const
{
    return m_chunks;
}

void World::setChunk(struct Chunk morceau, int index)
{
    m_chunks[index] = morceau;
}

void World::generate()
{
    int x,y,z;
    float valeur;
    float amplitude;
    float incrementation;

    // montagne
    for(z = 16; z < 31; z++)
    {
        for(y = 0; y < m_y; y++)
        for(x = 0; x < m_x; x++)
        {
            valeur = raw_noise_3d(m_permutations, (float)x/256.0, (float)y/256.0,(float)z/256.0);
            valeur += raw_noise_3d(m_permutations, (float)x/128.0, (float)y/128.0,(float)z/128.0)/2;
            valeur += raw_noise_3d(m_permutations, (float)x/64.0, (float)y/64.0,(float)z/64.0)/4;
            valeur += raw_noise_3d(m_permutations, (float)x/32.0, (float)y/32.0,(float)z/32.0)/8;

            if(valeur > 0)// && m_blocks[z * m_x * m_y + y * m_x + x] == 1)
            {
                m_blocks[z * m_x * m_y + y * m_x + x] = 1;
            }
        }
    }

    // on met le sol
    for(z = 0; z < 16; z++)
    {
        for(y = 0; y < m_y; y++)
        for(x = 0; x < m_x; x++)
        {
            m_blocks[z * m_x * m_y + y * m_x + x] = 1;
        }
    }

    // grottes normales
    for(z = 16; z < 24; z++)
    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        //valeur = raw_noise_3d(m_permutations, (float)x/64.0, (float)y/64.0,(float)z/64.0);
        valeur = raw_noise_3d(m_permutations, (float)x/32.0, (float)y/32.0,(float)z/32.0)/2;
        valeur += raw_noise_3d(m_permutations, (float)x/16.0, (float)y/16.0,(float)z/16.0)/4;
        valeur += raw_noise_3d(m_permutations, (float)x/8.0, (float)y/8.0,(float)z/8.0)/8;

        if(valeur > 0.25f && m_blocks[z * m_x * m_y + y * m_x + x] == 1)
        {
            m_blocks[z * m_x * m_y + y * m_x + x] = 0;
        }
    }

    // ajout de la pierre
    for(z = 0; z < 31; z++)
    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        valeur = raw_noise_3d(m_permutations, (float)x/32.0, (float)y/32.0,(float)z/32.0);
        valeur += raw_noise_3d(m_permutations, (float)x/16.0, (float)y/16.0,(float)z/16.0);

        if(valeur > 0.5f && m_blocks[z * m_x * m_y + y * m_x + x] == 1)
        {
            m_blocks[z * m_x * m_y + y * m_x + x] = 3;
        }
    }

    // sol tout en bas
    // on remplit les trous
    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        if(m_blocks[0 * m_x * m_y + y * m_x + x] == 0)
            m_blocks[0 * m_x * m_y + y * m_x + x] = 1;

        if(m_blocks[1 * m_x * m_y + y * m_x + x] == 0)
            m_blocks[1 * m_x * m_y + y * m_x + x] = 1;
    }

    amplitude = 12;
    incrementation = 32 - amplitude;

    // on arrondit le haut
    // heightmap 2D
    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        valeur = raw_noise_2d(m_permutations, (float)x/128.0, (float)y/128.0);
        valeur += raw_noise_2d(m_permutations, (float)x/64.0, (float)y/64.0)/2.0;
        valeur += raw_noise_2d(m_permutations, (float)x/32.0, (float)y/32.0)/4.0;
        valeur += raw_noise_2d(m_permutations, (float)x/16.0, (float)y/16.0)/8.0;

        valeur = (valeur+1)/2.0; // pour avoir un résultat entre 0 et 1

        valeur *= amplitude;
        valeur += incrementation;


        for(z = incrementation; z < 32; z++)
        {
            // pour chaque bloc qu'on trouve
            if(m_blocks[z * m_x * m_y + y * m_x + x] > 0)
            {
                // si la hauteur de ce bloc est supérieure à valeur
                // on supprime le bloc
                if(z >= valeur)
                {
                    m_blocks[z * m_x * m_y + y * m_x + x] = 0;
                }
            }
        }
    }

    // sol
    amplitude = 4;

    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        valeur = raw_noise_2d(m_permutations, (float)x/128.0, (float)y/128.0);
        valeur += raw_noise_2d(m_permutations, (float)x/64.0, (float)y/64.0)/2;
        valeur += raw_noise_2d(m_permutations, (float)x/32.0, (float)y/32.0)/4;
        //valeur += raw_noise_2d(m_permutations, (float)x/16.0, (float)y/16.0)/8;

        valeur = (valeur+1)/2.0; // pour avoir un résultat entre 0 et 1
        valeur *= amplitude;

        // une fois qu'on a la hauteur pour x et y données on ajoute des blocs type 1 de 0 jusqu'à la hauteur
        for(z = 15; z < 15+valeur; z++)
        {
            m_blocks[z * m_x * m_y + y * m_x + x] = 1;
        }
    }

    // neige
    for(y = 0; y < m_y; y++)
    for(x = 0; x < m_x; x++)
    {
        for(z = m_z - 1; z >= 0; z--)
        {
            // dès qu'on tombe sur un bloc non nul
            if(m_blocks[z * m_x * m_y + y * m_x + x] != 0)
            {
                // on ajoute de la neige
                m_blocks[(z+1) * m_x * m_y + y * m_x + x] = 2;
                break;
            }
        }
    }

    // ajout d'arbre
    // on ajoute les arbres avant les herbes
    // pour éviter des arbres sur des herbes

    int plantePlacee = 0;
    int planteTotal = m_x * m_y / 400;
    int iteration = 0;

    while(plantePlacee < planteTotal)
    {
        // on choisi un emplacement au hasard
        x = rand() % (m_x - 16); // [0, m_x - 17]
        y = rand() % (m_y - 16);
        x += 8; // [8, m_x - 9]
        y += 8;

        for(z = m_z - 10; z >= 15; z--)
        {
            // si le cube est de la terre ou de la neige
            // ou un arbre
            if(m_blocks[z * m_x * m_y + y * m_x +  x] == 1
            || m_blocks[z * m_x * m_y + y * m_x +  x] == 2)
            {
                addTree(x, y, z);
                plantePlacee++;

                break;
            }
        }

        // sécurité
        iteration++;
        if(iteration > m_x * m_y)
            break;
    }
}

void World::addTree(int X, int Y, int Z)
{
    // on ajoute des blocs
    int x, y, z;
    int i, j, k;

    // hauteur 8
    // chaque arbre contient 7 cubes de type 5 et 29 cubes de type 6
    // 29 cubes donnent en moyenne 29/3 = 10 cubes de bois
    // 29 cubes donnent en moyenne 29/6 = 5 fruits
    // un arbre donne entre 7 et 36 cubes, en moyenne 17 cubes
    int modeleArbre[] = {
                            0,0,0,0,0,
                            0,0,0,0,0,
                            0,0,5,0,0,
                            0,0,0,0,0,
                            0,0,0,0,0,

                            0,0,0,0,0,
                            0,0,0,0,0,
                            0,0,5,0,0,
                            0,0,0,0,0,
                            0,0,0,0,0,

                            0,0,0,0,0,
                            0,0,0,0,0,
                            0,0,5,0,0,
                            0,0,0,0,0,
                            0,0,0,0,0,

                            0,0,0,0,0,
                            0,0,6,0,0,
                            0,6,5,6,0,
                            0,0,6,0,0,
                            0,0,0,0,0,

                            0,0,6,0,0,
                            0,6,6,6,0,
                            6,6,5,6,6,
                            0,6,6,6,0,
                            0,0,6,0,0,

                            0,0,0,0,0,
                            0,6,6,6,0,
                            0,6,5,6,0,
                            0,6,6,6,0,
                            0,0,0,0,0,

                            0,0,0,0,0,
                            0,0,6,0,0,
                            0,6,5,6,0,
                            0,0,6,0,0,
                            0,0,0,0,0,

                            0,0,0,0,0,
                            0,0,0,0,0,
                            0,0,6,0,0,
                            0,0,0,0,0,
                            0,0,0,0,0,
                        };

    int bloc;

    for(k = 0; k < 8; k++)
    for(j = 0; j < 5; j++)
    for(i = 0; i < 5; i++)
    {
        // on enlève 2 pour centrer
        x = X + i - 2;
        y = Y + j - 2;
        z = Z + k;

        bloc = modeleArbre[k * 25 + j * 5 + i];

        if(bloc != 0) // on remplace seulement si c'est un cube différent de 0
        {
            m_blocks[z * m_y * m_x + y * m_x + x] =
            modeleArbre[k * 25 + j * 5 + i];
        }

    }
}

void World::load()
{
    ifstream fichier ("save/world", ios::out | ios::binary);

    if (fichier.good())
    {
        fichier.read((char*)m_blocks, sizeof(int) * m_x * m_y * m_z);
    }
    else
    {
        generate();
    }

    for(int i = 0; i < m_chunkCount; i++)
    {
        m_chunks[i] = updateChunk(i);
    }
}

struct Chunk World::updateChunk(int chunkIndex)
{
    int i,j;

    int nombreChunkX = 16;
    int nombreChunkY = 16;
    int tailleChunk = 16;

    struct Chunk morceau;


    for(j = 0; j < 6; j++)
    {
        morceau.vertex[j] = new float[1];
        morceau.coordonneTexture[j] = new float[1];
        morceau.couleur[j] = new GLubyte[1];
        morceau.nombreFace[j] = 0;
    }

    int limitX = 256;
    int limitY = 256;
    int limitZ = 64;

    // ---------------------------------------------------

    int x,y,z;

    // on détermine quel morceau il faut mettre à jour
    //int numeroChunk = 0;
    int xMin, xMax, yMin, yMax, zMin, zMax;

    z = chunkIndex / (nombreChunkX*nombreChunkY);
    y = (chunkIndex % (nombreChunkX*nombreChunkY)) / nombreChunkX;
    x = (chunkIndex % (nombreChunkX*nombreChunkY)) % nombreChunkX;

    xMin = x*tailleChunk;
    xMax = (x+1)*tailleChunk;

    yMin = y*tailleChunk;
    yMax = (y+1)*tailleChunk;

    zMin = z*tailleChunk;
    zMax = (z+1)*tailleChunk;


    // Calcul préalable de la taille des tableaux
    // entre la taille maximale et le nombre de coordonnées rentrées dans le tableau
    // on peut constater un facteur x50

    morceau.nombreFace[0] = 0;
    morceau.nombreFace[1] = 0;
    morceau.nombreFace[2] = 0;
    morceau.nombreFace[3] = 0;
    morceau.nombreFace[4] = 0;
    morceau.nombreFace[5] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x]) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x+1]) == 0) // si le bloc en x+1 n'existe pas
                {
                    morceau.nombreFace[0]++;
                }

                if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x-1]) == 0) // si le bloc en x-1 n'existe pas
                {
                    morceau.nombreFace[1]++;
                }

                if(isBlock(m_blocks[z * limitX * limitY + (y+1) * limitX + x]) == 0) // si le bloc en y+1 n'existe pas
                {
                    morceau.nombreFace[2]++;
                }

                if(isBlock(m_blocks[z * limitX * limitY + (y-1) * limitX + x]) == 0) // si le bloc en y-1 n'existe pas
                {
                    morceau.nombreFace[3]++;
                }

                if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) == 0) // si le bloc en z+1 n'existe pas
                {
                    morceau.nombreFace[4]++;
                }

                if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) == 0) // si le bloc en z-1 n'existe pas
                {
                    morceau.nombreFace[5]++;
                }
            }
        }
    }

    for(j = 0; j < 6; j++)
    {
        delete [] morceau.vertex[j];
        morceau.vertex[j] = new float[morceau.nombreFace[j]*4*3];

        delete [] morceau.coordonneTexture[j];
        morceau.coordonneTexture[j] = new float[morceau.nombreFace[j]*4*2];

        delete [] morceau.couleur[j];
        morceau.couleur[j] = new GLubyte[morceau.nombreFace[j]*4*3];
    }

    int indice = 0;
    int indiceCoordonneeTexture = 0;
    int indiceCouleur = 0;

    int typeBloc;
    int r,g,b;

    int ombreR;
    int ombreG;
    int ombreB;

    int nombreCube;

    morceau.nombreFace[0] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        // si ce n'est pas vide ET si ce n'est pas de la végétation
        if(isBlock(typeBloc) == true)
        {
            // on s'assure de ne pas sortir des limites
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x+1]) == 0) // si le bloc en x+1 n'existe pas
                {
                    morceau.nombreFace[0]++;

                    // vertex 1
                    morceau.vertex[0][indice+0] = x+1;
                    morceau.vertex[0][indice+1] = y;
                    morceau.vertex[0][indice+2] = z+1;

                    // vertex 2
                    morceau.vertex[0][indice+3] = x+1;
                    morceau.vertex[0][indice+4] = y;
                    morceau.vertex[0][indice+5] = z;

                    // vertex 3
                    morceau.vertex[0][indice+6] = x+1;
                    morceau.vertex[0][indice+7] = y+1;
                    morceau.vertex[0][indice+8] = z;

                    // vertex 4
                    morceau.vertex[0][indice+9] = x+1;
                    morceau.vertex[0][indice+10] = y+1;
                    morceau.vertex[0][indice+11] = z+1;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+0] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+2] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+3] = j*0.0625 + 0.001;

                    // vertex 3
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+4] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+6] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[0][indiceCoordonneeTexture+7] = (j+1)*0.0625 - 0.001;
                    indiceCoordonneeTexture += 8;

                    // Mode sans texture
                    // chaque vertex a une couleur définie par 3

                    blockColor(typeBloc, r,g,b,0,x,y,z);

                    // Z
                    // | 1 4
                    // | 2 3
                    // X - - Y

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[0][indiceCouleur+0] = ombreR;
                    morceau.couleur[0][indiceCouleur+1] = ombreG;
                    morceau.couleur[0][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[0][indiceCouleur+3] = ombreR;
                    morceau.couleur[0][indiceCouleur+4] = ombreG;
                    morceau.couleur[0][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[0][indiceCouleur+6] = ombreR;
                    morceau.couleur[0][indiceCouleur+7] = ombreG;
                    morceau.couleur[0][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-0) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[0][indiceCouleur+9] = ombreR;
                    morceau.couleur[0][indiceCouleur+10] = ombreG;
                    morceau.couleur[0][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;

                }
            }
        }
    }

    indice = 0;
    indiceCoordonneeTexture = 0;
    indiceCouleur = 0;

    morceau.nombreFace[1] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        if(isBlock(typeBloc) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x-1]) == 0) // si le bloc en x-1 n'existe pas
                {
                    morceau.nombreFace[1]++;

                    // vertex 1
                    morceau.vertex[1][indice+0] = x;
                    morceau.vertex[1][indice+1] = y+1;
                    morceau.vertex[1][indice+2] = z+1;

                    // vertex 2
                    morceau.vertex[1][indice+3] = x;
                    morceau.vertex[1][indice+4] = y+1;
                    morceau.vertex[1][indice+5] = z;

                    // vertex 3
                    morceau.vertex[1][indice+6] = x;
                    morceau.vertex[1][indice+7] = y;
                    morceau.vertex[1][indice+8] = z;

                    // vertex 4
                    morceau.vertex[1][indice+9] = x;
                    morceau.vertex[1][indice+10] = y;
                    morceau.vertex[1][indice+11] = z+1;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+6] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+7] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+0] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 3
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+2] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+3] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+4] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[1][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;
                    indiceCoordonneeTexture += 8;

                    blockColor(typeBloc, r,g,b,0,x,y,z);

                    // Z
                    // | 1 2
                    // | 4 3
                    // X - - Y

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[z * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[1][indiceCouleur+0] = ombreR;
                    morceau.couleur[1][indiceCouleur+1] = ombreG;
                    morceau.couleur[1][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[1][indiceCouleur+3] = ombreR;
                    morceau.couleur[1][indiceCouleur+4] = ombreG;
                    morceau.couleur[1][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[z * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[z * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[1][indiceCouleur+6] = ombreR;
                    morceau.couleur[1][indiceCouleur+7] = ombreG;
                    morceau.couleur[1][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[1][indiceCouleur+9] = ombreR;
                    morceau.couleur[1][indiceCouleur+10] = ombreG;
                    morceau.couleur[1][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;
                }
            }
        }
    }


    indice = 0;
    indiceCoordonneeTexture = 0;
    indiceCouleur = 0;

    morceau.nombreFace[2] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        if(isBlock(typeBloc) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[z * limitX * limitY + (y+1) * limitX + x]) == 0) // si le bloc en y+1 n'existe pas
                {
                    morceau.nombreFace[2]++;

                    // vertex 1
                    morceau.vertex[2][indice+0] = x;
                    morceau.vertex[2][indice+1] = y+1;
                    morceau.vertex[2][indice+2] = z+1;

                    // vertex 2
                    morceau.vertex[2][indice+3] = x+1;
                    morceau.vertex[2][indice+4] = y+1;
                    morceau.vertex[2][indice+5] = z+1;

                    // vertex 3
                    morceau.vertex[2][indice+6] = x+1;
                    morceau.vertex[2][indice+7] = y+1;
                    morceau.vertex[2][indice+8] = z;

                    // vertex 4
                    morceau.vertex[2][indice+9] = x;
                    morceau.vertex[2][indice+10] = y+1;
                    morceau.vertex[2][indice+11] = z;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+0] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+2] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+3] = (j+1)*0.0625 - 0.001;

                    // vertex 3
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+4] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+6] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[2][indiceCoordonneeTexture+7] = j*0.0625 + 0.001;
                    indiceCoordonneeTexture += 8;

                    blockColor(typeBloc, r,g,b,1,x,y,z);

                    // Z
                    // | 1 2
                    // | 4 3
                    // Y - - X

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[2][indiceCouleur+0] = ombreR;
                    morceau.couleur[2][indiceCouleur+1] = ombreG;
                    morceau.couleur[2][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-0) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-0) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[2][indiceCouleur+3] = ombreR;
                    morceau.couleur[2][indiceCouleur+4] = ombreG;
                    morceau.couleur[2][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[2][indiceCouleur+6] = ombreR;
                    morceau.couleur[2][indiceCouleur+7] = ombreG;
                    morceau.couleur[2][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x+0]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[2][indiceCouleur+9] = ombreR;
                    morceau.couleur[2][indiceCouleur+10] = ombreG;
                    morceau.couleur[2][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;

                }
            }
        }
    }

    indice = 0;
    indiceCoordonneeTexture = 0;
    indiceCouleur = 0;

    morceau.nombreFace[3] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        if(isBlock(typeBloc) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[z * limitX * limitY + (y-1) * limitX + x]) == 0) // si le bloc en y-1 n'existe pas
                {
                    morceau.nombreFace[3]++;

                    // vertex 1
                    morceau.vertex[3][indice+0] = x;
                    morceau.vertex[3][indice+1] = y;
                    morceau.vertex[3][indice+2] = z+1;

                    // vertex 2
                    morceau.vertex[3][indice+3] = x;
                    morceau.vertex[3][indice+4] = y;
                    morceau.vertex[3][indice+5] = z;

                    // vertex 3
                    morceau.vertex[3][indice+6] = x+1;
                    morceau.vertex[3][indice+7] = y;
                    morceau.vertex[3][indice+8] = z;

                    // vertex 4
                    morceau.vertex[3][indice+9] = x+1;
                    morceau.vertex[3][indice+10] = y;
                    morceau.vertex[3][indice+11] = z+1;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+0] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+2] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+3] = j*0.0625 + 0.001;

                    // vertex 3
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+4] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+6] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[3][indiceCoordonneeTexture+7] = (j+1)*0.0625 - 0.001;
                    indiceCoordonneeTexture += 8;


                    blockColor(typeBloc, r,g,b,1,x,y,z);

                    // Z
                    // | 1 4
                    // | 2 3
                    // Y - - X

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[3][indiceCouleur+0] = ombreR;
                    morceau.couleur[3][indiceCouleur+1] = ombreG;
                    morceau.couleur[3][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[3][indiceCouleur+3] = ombreR;
                    morceau.couleur[3][indiceCouleur+4] = ombreG;
                    morceau.couleur[3][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[3][indiceCouleur+6] = ombreR;
                    morceau.couleur[3][indiceCouleur+7] = ombreG;
                    morceau.couleur[3][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+0) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[3][indiceCouleur+9] = ombreR;
                    morceau.couleur[3][indiceCouleur+10] = ombreG;
                    morceau.couleur[3][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;

                }
            }
        }
    }


    indice = 0;
    indiceCoordonneeTexture = 0;
    indiceCouleur = 0;

    morceau.nombreFace[4] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        if(isBlock(typeBloc) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) == 0) // si le bloc en z+1 n'existe pas
                {
                    morceau.nombreFace[4]++;

                    // vertex 1
                    morceau.vertex[4][indice+0] = x;
                    morceau.vertex[4][indice+1] = y+1;
                    morceau.vertex[4][indice+2] = z+1;

                    // vertex 2
                    morceau.vertex[4][indice+3] = x;
                    morceau.vertex[4][indice+4] = y;
                    morceau.vertex[4][indice+5] = z+1;

                    // vertex 3
                    morceau.vertex[4][indice+6] = x+1;
                    morceau.vertex[4][indice+7] = y;
                    morceau.vertex[4][indice+8] = z+1;

                    // vertex 4
                    morceau.vertex[4][indice+9] = x+1;
                    morceau.vertex[4][indice+10] = y+1;
                    morceau.vertex[4][indice+11] = z+1;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+0] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+2] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+3] = j*0.0625 + 0.001;

                    // vertex 3
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+4] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+6] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[4][indiceCoordonneeTexture+7] = (j+1)*0.0625 - 0.001;
                    indiceCoordonneeTexture += 8;

                    blockColor(typeBloc, r,g,b,2,x,y,z);

                    colorLimit(r, g, b);

                    // Y
                    // | 1 4
                    // | 2 3
                    // Z - - X

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[4][indiceCouleur+0] = ombreR;
                    morceau.couleur[4][indiceCouleur+1] = ombreG;
                    morceau.couleur[4][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[4][indiceCouleur+3] = ombreR;
                    morceau.couleur[4][indiceCouleur+4] = ombreG;
                    morceau.couleur[4][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[4][indiceCouleur+6] = ombreR;
                    morceau.couleur[4][indiceCouleur+7] = ombreG;
                    morceau.couleur[4][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z+1) * limitX * limitY + y * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[4][indiceCouleur+9] = ombreR;
                    morceau.couleur[4][indiceCouleur+10] = ombreG;
                    morceau.couleur[4][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;

                }
            }
        }
    }

    indice = 0;
    indiceCoordonneeTexture = 0;
    indiceCouleur = 0;

    morceau.nombreFace[5] = 0;

    for(z = zMin; z < zMax; z++)
    for(y = yMin; y < yMax; y++)
    for(x = xMin; x < xMax; x++)
    {
        typeBloc = m_blocks[z * limitX * limitY + y * limitX + x];
        i = typeBloc % 16;
        j = typeBloc / 16;

        if(isBlock(typeBloc) == true)
        {
            if(x + 1 < limitX && y + 1 < limitY && z + 1 < limitZ
            && x - 1 >= 0 && y - 1 >= 0 && z - 1 >= 0)
            {
                if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) == 0) // si le bloc en z-1 n'existe pas
                {
                    morceau.nombreFace[5]++;

                    // vertex 1
                    morceau.vertex[5][indice+0] = x;
                    morceau.vertex[5][indice+1] = y+1;
                    morceau.vertex[5][indice+2] = z;

                    // vertex 2
                    morceau.vertex[5][indice+3] = x+1;
                    morceau.vertex[5][indice+4] = y+1;
                    morceau.vertex[5][indice+5] = z;

                    // vertex 3
                    morceau.vertex[5][indice+6] = x+1;
                    morceau.vertex[5][indice+7] = y;
                    morceau.vertex[5][indice+8] = z;

                    // vertex 4
                    morceau.vertex[5][indice+9] = x;
                    morceau.vertex[5][indice+10] = y;
                    morceau.vertex[5][indice+11] = z;

                    indice += 12;

                    // vertex 1
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+0] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+1] = (j+1)*0.0625 - 0.001;

                    // vertex 2
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+2] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+3] = (j+1)*0.0625 - 0.001;

                    // vertex 3
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+4] = (i+1)*0.0625 - 0.001;
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+5] = j*0.0625 + 0.001;

                    // vertex 4
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+6] = i*0.0625 + 0.001;
                    morceau.coordonneTexture[5][indiceCoordonneeTexture+7] = j*0.0625 + 0.001;
                    indiceCoordonneeTexture += 8;

                    blockColor(typeBloc, r,g,b,2,x,y,z);

                    // Y
                    // | 1 2
                    // | 4 3
                    // Z - - X

                    // vertex 1
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[5][indiceCouleur+0] = ombreR;
                    morceau.couleur[5][indiceCouleur+1] = ombreG;
                    morceau.couleur[5][indiceCouleur+2] = ombreB;

                    // vertex 2
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y+1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[5][indiceCouleur+3] = ombreR;
                    morceau.couleur[5][indiceCouleur+4] = ombreG;
                    morceau.couleur[5][indiceCouleur+5] = ombreB;

                    // vertex 3
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x+1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x+1]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[5][indiceCouleur+6] = ombreR;
                    morceau.couleur[5][indiceCouleur+7] = ombreG;
                    morceau.couleur[5][indiceCouleur+8] = ombreB;

                    // vertex 4
                    nombreCube = 0;
                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + y * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x-1]) != 0)
                        nombreCube++;

                    if(isBlock(m_blocks[(z-1) * limitX * limitY + (y-1) * limitX + x]) != 0)
                        nombreCube++;

                    blockColorWithShadow(nombreCube, r, g, b, ombreR, ombreG, ombreB);

                    morceau.couleur[5][indiceCouleur+9] = ombreR;
                    morceau.couleur[5][indiceCouleur+10] = ombreG;
                    morceau.couleur[5][indiceCouleur+11] = ombreB;

                    indiceCouleur += 12;
                }
            }
        }
    }

    return morceau;
}

bool World::isBlock(int type)
{
    if(type != 0
    && type != 253
    && type != 254
    && type != 255)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void World::blockColor(int &blockType, int &r, int &g, int &b, int face, int x, int y, int z)
{
    // on varie légèrement la couleur selon les coordonnées x, y, z du cube
    x = x % 16;
    y = y % 16;
    z = z % 16;

    //int nombreAleatoire = game_data.tableauNombreAleatoire[z * 16 * 16 + y * 16 + x];
    //int nombreAleatoire = random[z * 16 * 16 + y * 16 + x];
    // les valeurs doivent être comprises entre 0 et 255
    // à 250 il n'y a presque plus de différence de teinte
    int R,G,B;

    //TerrainCubeCouleur(typeBloc, R, G, B);
    switch (blockType)
    {
        case 1:
            R = 125;
            G = 100;
            B = 80;
            break;

        case 2:
            R = 240;
            G = 245;
            B = 245;
            break;

        case 3:
            R = 120;
            G = 120;
            B = 120;
            break;

        case 4:
            R = 70;
            G = 70;
            B = 80;
            break;

        case 5:
            R = 88;
            G = 68;
            B = 38;
            break;

        case 6:
            R = 230;
            G = 240;
            B = 220;
            break;

        default:
            R = 225;
            G = 225;
            B = 225;
            break;
    }

    colorLimit(R, G, B);

    // teinte qui différencie les faces X, Y et Z
    if(face == 0) // en x+ et x-
    {
        R -= 15;
        G -= 15;
        B -= 15;
    }
    else if(face == 1) // en y+ et y-
    {
        R -= 25;
        G -= 25;
        B -= 25;
    }

    colorLimit(R, G, B);

    r = R;
    g = G;
    b = B;
}

void World::blockColorWithShadow(int &nombreCube, int r, int g, int b, int &ombreR, int &ombreG, int &ombreB)
{
    switch (nombreCube)
    {
        case 0:
            ombreR = r;
            ombreG = g;
            ombreB = b;
            break;

        case 1:
            ombreR = r - 30;
            ombreG = g - 30;
            ombreB = b - 30;
            break;

        case 2:
            ombreR = r - 60;
            ombreG = g - 60;
            ombreB = b - 60;
            break;

        case 3:
            ombreR = r - 70;
            ombreG = g - 70;
            ombreB = b - 70;
            break;
    }

    colorLimit(ombreR, ombreG, ombreB);
}

void World::colorLimit(int &red, int &green, int &blue)
{
    if (red < 0) red = 0;
    else if (red > 255) red = 255;

    if (green < 0) green = 0;
    else if (green > 255) green = 255;

    if (blue < 0) blue = 0;
    else if (blue > 255) blue = 255;
}

void World::updateBlock(int x, int y, int z)
{
    // on détermine dans quel morceau se trouve le bloc qu'on a changé
    int X,Y,Z;
    int index;
    X = x / m_chunkSize;
    Y = y / m_chunkSize;
    Z = z / m_chunkSize;

    index = Z * m_chunkX * m_chunkY + Y * m_chunkX + X;

    setChunk(updateChunk(index), index);

    // si on se trouve sur un bord on met à jour le morceau d'à côté
    if((x % m_chunkSize) == 15 && X+1 < m_chunkX)
    {
        index = Z * m_chunkX * m_chunkY + Y * m_chunkX + X+1;
        setChunk(updateChunk(index), index);
    }

    if((x % m_chunkSize) == 0 && X-1 >= 0)
    {
        index = Z * m_chunkX * m_chunkY + Y * m_chunkX + X-1;
        setChunk(updateChunk(index), index);
    }

    if((y % m_chunkSize) == 15 && Y+1 < m_chunkY)
    {
        index = Z * m_chunkX * m_chunkY + (Y+1) * m_chunkX + X;
        setChunk(updateChunk(index), index);
    }

    if((y % m_chunkSize) == 0 && Y-1 >= 0)
    {
        index = Z * m_chunkX * m_chunkY + (Y-1) * m_chunkX + X;
        setChunk(updateChunk(index), index);
    }

    if((z % m_chunkSize) == 15 && Z+1 < m_chunkZ)
    {
        index = (Z+1) * m_chunkX * m_chunkY + Y * m_chunkX + X;
        setChunk(updateChunk(index), index);
    }

    if((z % m_chunkSize) == 0 && Z-1 >= 0)
    {
        index = (Z-1) * m_chunkX * m_chunkY + Y * m_chunkX + X;
        setChunk(updateChunk(index), index);
    }
}

void World::addBlock(Mouse &mouse, Player &player)
{
    if (mouse.getLeftClick())
    {
        struct Position position = getTargetPosition(player.getCameraX(), player.getCameraY(), player.getCameraZ(), 1);

        m_blockAddingX = position.x;
        m_blockAddingY = position.y;
        m_blockAddingZ = position.z;

        if (m_blockAddingX < 1)
            m_blockAddingX = 1;
        else if (m_blockAddingX > m_x - 1)
            m_blockAddingX = m_x - 1;

        if (m_blockAddingY < 1)
            m_blockAddingY = 1;
        else if (m_blockAddingY > m_y - 1)
            m_blockAddingY = m_y - 1;

        if (m_blockAddingZ < 1)
            m_blockAddingZ = 1;
        else if (m_blockAddingZ > m_z - 1)
            m_blockAddingZ = m_z - 1;

        float distance = distance3D(player.getCameraX(), player.getCameraY(), player.getCameraZ(), m_blockAddingX + 0.5f, m_blockAddingY + 0.5f, m_blockAddingZ + 0.5f);

        if (distance > 1.5f && distance < 10.0f) {
            int index = m_blockAddingZ * m_y * m_x + m_blockAddingY * m_x + m_blockAddingX;
            setBlock(index, 1);
            updateBlock(m_blockAddingX, m_blockAddingY, m_blockAddingZ);
        }

        mouse.setLeftClick(false);
    }
}

void World::removeBlock(Mouse &mouse, Player &player)
{
    if (mouse.getRightClick())
    {
        struct Position position = getTargetPosition(player.getCameraX(), player.getCameraY(), player.getCameraZ(), 0);

        m_blockRemovalX = position.x;
        m_blockRemovalY = position.y;
        m_blockRemovalZ = position.z;

        if (m_blockRemovalX < 1)
            m_blockRemovalX = 1;
        else if (m_blockRemovalX > m_x - 1)
            m_blockRemovalX = m_x - 1;

        if (m_blockRemovalY < 1)
            m_blockRemovalY = 1;
        else if (m_blockRemovalY > m_y - 1)
            m_blockRemovalY = m_y - 1;

        if (m_blockRemovalZ < 1)
            m_blockRemovalZ = 1;
        else if (m_blockRemovalZ > m_z - 1)
            m_blockRemovalZ = m_z - 1;

        float distance = distance3D(player.getCameraX(), player.getCameraY(), player.getCameraZ(), m_blockRemovalX + 0.5f, m_blockRemovalY + 0.5f, m_blockRemovalZ + 0.5f);

        if (distance < 10.0f) {
            int index = m_blockRemovalZ * m_y * m_x + m_blockRemovalY * m_x + m_blockRemovalX;
            setBlock(index, 0);
            updateBlock(m_blockRemovalX, m_blockRemovalY, m_blockRemovalZ);
        }

        mouse.setRightClick(false);
    }
}

float World::distance3D(float x, float y, float z, float X, float Y, float Z)
{
    return sqrt( (X-x) * (X-x) + (Y-y) * (Y-y) + (Z-z) * (Z-z));
}

// on récupère les coordonnées 3D du point désigné par le centre de l'écran
// la texture qui s'applique sur le polygone n'a pas d'importance
struct Position World::getTargetPosition(float cameraX, float cameraY, float cameraZ, int mode)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    // la matrice modelview transforme les vertices des primitives OpenGL en eye coordinates
    // on récupère les 16 valeurs doubles
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );

    // la matrice projection transforme les eye coordinates en clip coordinates
    glGetDoublev( GL_PROJECTION_MATRIX, projection );

    // on récupère les informations de la fenêtre
    // x, y, largeur et hauteur
    // et on les stocke dans un tableau
    glGetIntegerv( GL_VIEWPORT, viewport );

    // on récupère les coordonnées de l'écran windows
    winX = SDL_GetVideoSurface()->w/2;
    winY = SDL_GetVideoSurface()->h/2;

    // on récupère la coordonnée Z
    glReadPixels( int(winX), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

    // on calcule les coordonnées OpenGL et on les stocke dans posX, posY et posZ
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    Position position;

    position.x = (int)posX;
    position.y = (int)posY;
    position.z = (int)posZ;

    // on extrait la partie décimale
    int partieDecimale;

    if(mode == 1) // ajout de bloc
    {
        partieDecimale = (posX - (int)posX) * 100;
        if(partieDecimale > 98)
        {
            if(cameraX < posX)
            {
                // on garde la partie trouquée donc pas de changement
            }
            else
            {
                position.x++;
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraX < posX)
            {
                position.x--;
            }
            else
            {
                // on garde la partie trouquée donc pas de changement
            }
        }

        partieDecimale = (posY - (int)posY) * 100;
        if(partieDecimale > 98)
        {
            if(cameraY < posY)
            {
                // on garde la partie trouquée donc pas de changement
            }
            else
            {
                position.y++;
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraY < posY)
            {
                position.y--;
            }
            else
            {
                // on garde la partie trouquée donc pas de changement
            }
        }

        partieDecimale = (posZ - (int)posZ) * 100;
        if(partieDecimale > 98)
        {
            if(cameraZ < posZ)
            {
                // on garde la partie trouquée donc pas de changement
            }
            else
            {
                position.z++;
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraZ < posZ)
            {
                position.z--;
            }
            else
            {
                // on garde la partie trouquée donc pas de changement
            }
        }
    }
    else // si on veut supprimer un bloc
    {
        partieDecimale = (posX - (int)posX) * 100;
        if(partieDecimale > 98)
        {
            if(cameraX < posX)
            {
                position.x++;
            }
            else
            {
                //
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraX < posX)
            {
                //
            }
            else
            {
                position.x--;
            }
        }

        partieDecimale = (posY - (int)posY) * 100;
        if(partieDecimale > 98)
        {
            if(cameraY < posY)
            {
                position.y++;
            }
            else
            {
                //
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraY < posY)
            {
                //
            }
            else
            {
                position.y--;
            }
        }

        partieDecimale = (posZ - (int)posZ) * 100;
        if(partieDecimale > 98)
        {
            if(cameraZ < posZ)
            {
                position.z++;
            }
            else
            {
                //
            }
        }

        if(partieDecimale < 2)
        {
            if(cameraZ < posZ)
            {
                //
            }
            else
            {
                position.z--;
            }
        }
    }

    return position;
}

void World::save()
{
    ofstream fichier ("save/world", ios::out | ios::binary);
    // ATTENTION pas de & !!!
    fichier.write((char*)m_blocks, sizeof(int) * m_x * m_y * m_z);
    fichier.close();
}

int World::getX() const
{
    return m_x;
}

int World::getY() const
{
    return m_y;
}

int World::getZ() const
{
    return m_z;
}

int World::getChunkSize() const
{
    return m_chunkSize;
}

int World::getChunkX() const
{
    return m_chunkX;
}

int World::getChunkY() const
{
    return m_chunkY;
}

int World::getChunkZ() const
{
    return m_chunkZ;
}
