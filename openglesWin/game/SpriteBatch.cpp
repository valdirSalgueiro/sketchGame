/**
 * Copyright (c) 2011 Nokia Corporation.
 *
 * CAUTION: This class is in very early experimental state.
 *
 */


#include "SpriteBatch.h"



void SpriteBatch::draw( int texture, float x, float y, float w, float h, float angle )
{
    SpriteDrawInfo sdi;
    sdi.textureHandle = texture;
    sdi.setTargetPos( x, y );
    sdi.setScale( w, h );
    sdi.angle = angle;
    draw( &sdi );
}

#ifdef SPRITE_BATCH_CLASS
SpriteBatch* SpriteBatch::createInstance(int width, int height, ...)
	{
	return new SPRITE_BATCH_CLASS(width, height);
	}
#endif
