#ifndef GLES11SPRITEBATCH_H
#define GLES11SPRITEBATCH_H

#include "SpriteBatch.h"

#include <GLES/gl.h>


#define COSINE_TABLE_SIZE 1024
#define COSINE_TABLE_AND 1023
#define BATCH_SIZE 32


class GLES11SpriteBatch : public SpriteBatch
{
public:
    GLES11SpriteBatch( int screenWidth, int screenHeight );
    virtual ~GLES11SpriteBatch();

    void begin( BlendMode bmode = eALPHA, TransformMode dTransform = ePIXELSPACE, float *customProjectionMatrix = 0);
    void end();
    void draw ( SpriteDrawInfo *sdi, int spriteCount=1 );

    void flushSprites();



protected:
        // faster cos/sin via table
    float cosineTable[COSINE_TABLE_SIZE];

        // For internal initialization / destruction
    bool init();
    void destroy();

        // The vertexbufferobject holding information of the batches
    GLuint vbo;
    GLfloat *vbo_vertices;

        // The static indexbuffer defining the triangles of the vbo
    GLuint ibo;


        // Collectino of sprites to be rendered
    SpriteDrawInfo batchCollection[ BATCH_SIZE ];
    int batchCounter;

        // Currently active texture
    GLuint currentTexture;
    int currentTextureWidth;
    int currentTextureHeight;

        // Objects for GLES2 rendering
    TransformMode currentDestinationTransform;
    int currentViewportWidth;
    int currentViewportHeight;


};

#endif