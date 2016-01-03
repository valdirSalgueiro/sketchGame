/**
 * Copyright (c) 2011 Nokia Corporation.
 *
 * CAUTION: This class is in very early experimental state.
 *
 */

#ifndef __SPRITEBATCH__
#define __SPRITEBATCH__

#include "Vector2D.h"
#include <memory.h>

enum EFFECT{
	FONT = 1 << 0,
	COLOR_ADVANCED = 1 << 1,
	NORMAL = 1 << 16,
	NONE = 1 << 20
};

struct SpriteDrawInfo
{
    SpriteDrawInfo() {
        sourcex = 0;
        sourcey = 0;
        sourceWidth = 1;
        sourceHeight = 1;
        r = 1;
        g = 1;
        b = 1;
        a = 1;
        posx = 0;
        posy = 0;
        originX = 0.5f;
        originY = 0.5f;
        angle = 0;

        scaleX = 1.0f;
        scaleY = 1.0f;

        manualTransformActive = false;
		effect=NONE;

    }
    // Setter helpers
    void setTargetPos( float xi, float yi ) {
        posx = xi;
        posy = yi;
    }

    void setOrigin( float ox=0.5f, float oy=0.5f ) {
        originX = ox;
        originY = oy;
    }

    void setColor( float ri, float gi, float bi, float ai ) {
        r = ri; g = gi; b = bi; a = ai;
    }

    void setColor( float *col ) {
        r = col[0]; g = col[1]; b = col[2]; a = col[3];
    }

    void setSourceRect( float x=0.0f, float y=0.0f, float w=1.0f, float h=1.0f) {
        sourcex = x;
        sourcey = y;
        sourceWidth = w;
        sourceHeight = h;
    }

    void setSourceRect( float *rec ) {
        sourcex = rec[0];
        sourcey = rec[1];
        sourceWidth = rec[2];
        sourceHeight = rec[3];
    }
    void setScale( float scale ) {
        scaleX = scale;
        scaleY = scale;
    }

    void setScale( float scaleXi, float scaleYi ) {
        scaleX = scaleXi;
        scaleY = scaleYi;
    }

    void manualTransform( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4 ) {
        manualTransformMatrix[0][0] = x1;
        manualTransformMatrix[0][1] = y1;
        manualTransformMatrix[1][0] = x2;
        manualTransformMatrix[1][1] = y2;
		manualTransformMatrix[2][0] = x3;
        manualTransformMatrix[2][1] = y3;
        manualTransformMatrix[3][0] = x4;
        manualTransformMatrix[3][1] = y4;
        manualTransformActive = true;
    }
    void disableManualTransform() { manualTransformActive = false; }



        // Texture handle
    unsigned int textureHandle;

	unsigned int textureNormalHandle;

        // Target position
    float posx;
    float posy;

        // Source rectangle.
    float sourcex;
    float sourcey;
    float sourceWidth;
    float sourceHeight;


        // Blit color. NOTE, these must be in this order r,g,b,a must be next to eachother in structures memory
    float r,g,b,a;

	float rb,gb,bb,ab;

        // Source origin
    float originX, originY;

        // Scale
    bool manualTransformActive;
    float manualTransformMatrix[4][2];


    float scaleX, scaleY;
    float angle;
	int effect;


    // For now, TODO, thinkAbout.
    // Effects ignored.
    // Layer depth ignored.
};



class SpriteBatch
{
public:
    enum TransformMode { ePIXELSPACE, eCUSTOMPROJECTION };
    enum BlendMode { eALPHA, eADDITIVE };

    SpriteBatch(int tWidth, int tHeight) {
        setTargetSize(tWidth, tHeight);
    }

    virtual ~SpriteBatch() {}

    virtual void begin( int mode, TransformMode dTransform = ePIXELSPACE, float *customProjectionMatrix = 0) {}
    virtual void end() {}
	virtual void drawLevel() {};
    virtual void draw ( SpriteDrawInfo *sdi, int spriteCount=1 ) = 0;

	virtual void startBloom(){}
	virtual void endBloom(){}

    // Helper draws
    void draw( int texture, float x, float y, float w, float h, float angle = 0.0f );

    virtual void flushSprites() = 0;            // Force flush,..

    inline int getTargetWidth() { return targetWidth; }
    inline int getTargetHeight() { return targetHeight; }
    inline void setTargetSize( int w, int h ) {
        targetWidth = w;
        targetHeight = h;
    }

protected:
    int targetWidth;
    int targetHeight;
    
public:    
    static SpriteBatch* createInstance(int width, int height, ...);


};

//#include "GLES11SpriteBatch.h"
#include "GLES2SpriteBatch.h"


#endif
