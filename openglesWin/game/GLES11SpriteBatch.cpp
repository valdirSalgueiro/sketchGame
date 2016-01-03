#include <math.h>
#include <stdio.h>
#include <string.h>
#include "GLES11SpriteBatch.h"

//#include <android/log.h>

int flushSprites_=0;

GLES11SpriteBatch::GLES11SpriteBatch( int tw, int th ) : SpriteBatch(tw,th)
{

	for (int f=0; f<COSINE_TABLE_SIZE; f++)
		cosineTable[f] = cosf( (float)f / (float)COSINE_TABLE_SIZE * 3.14159f * 2.0f )*0.5f;

	batchCounter = 0;
	vbo_vertices = 0;
	vbo = 0;
	ibo = 0;

	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [1]\n");
	init();
	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [2]\n");
}


GLES11SpriteBatch::~GLES11SpriteBatch()
{
	destroy();
}



bool GLES11SpriteBatch::init()
{
	glGenBuffers(1, &vbo);
	vbo_vertices = new GLfloat[ BATCH_SIZE * 4 * 9];        // 4 items / quad, 9 items / vertex = x,y,z,u,v, r,g,b,a


	glGenBuffers(1, &ibo);
	// fill the indexbuffer
	GLushort *tempIndices = new GLushort[ BATCH_SIZE * 6];

	for (int f=0; f<BATCH_SIZE; f++) {
		tempIndices[f*6+0] = f*4+0;
		tempIndices[f*6+1] = f*4+1;
		tempIndices[f*6+2] = f*4+2;

		tempIndices[f*6+3] = f*4+0;
		tempIndices[f*6+4] = f*4+2;
		tempIndices[f*6+5] = f*4+3;
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		BATCH_SIZE * 6 * sizeof(GLushort),
		tempIndices, GL_STATIC_DRAW);


	delete [] tempIndices;

	return true;

}


void GLES11SpriteBatch::destroy()
{
	if (vbo_vertices) delete [] vbo_vertices;
	if (vbo) glDeleteBuffers(1, &vbo ); vbo = 0;
	if (ibo) glDeleteBuffers(1, &ibo ); ibo = 0;

}


void GLES11SpriteBatch::begin( BlendMode bmode, TransformMode dTransform, float *customProjectionMatrix )
{
	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [3]\n");
	batchCounter = 0;

	glDisable( GL_COLOR_LOGIC_OP );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
	glEnable( GL_TEXTURE_2D );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );

	glDisable( GL_ALPHA_TEST );

	glEnable( GL_BLEND );
	//if (bmode==eALPHA)
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//else
	//	glBlendFunc( GL_SRC_ALPHA, GL_ONE);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if (dTransform == eCUSTOMPROJECTION && customProjectionMatrix ) {
		glLoadMatrixf( customProjectionMatrix );
	} else {
		// build a new projection matrix accoring screen's width and height
		GLfloat mproj[16] = {
			2.0f/(float)targetWidth, 0, 0, 0.0f,
			0, 2.0f/(float)targetHeight, 0, 0.0f,
			0, 0, 1, 0,
			-1, -1, 0, 1,
		};

		glLoadMatrixf( mproj );
	}


	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBindBuffer(GL_ARRAY_BUFFER, vbo );

	flushSprites_=0;
}

void GLES11SpriteBatch::end()
{

	flushSprites();
	// unbind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

/*	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisable( GL_TEXTURE_2D );*/
}


void GLES11SpriteBatch::draw( SpriteDrawInfo *sdi, int spriteCount )
{
	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [13]\n");
	SpriteDrawInfo *i = sdi;
	while (spriteCount>0) {
		if (i->textureHandle != currentTexture ) {
			// Texture have been changed, flush currently collected renderingdata
			flushSprites();
			// Change our current texture and activate it.
			currentTexture = i->textureHandle;

			glBindTexture( GL_TEXTURE_2D, currentTexture );
			// NOTE, the texturesize cannot be known in GLES2 -> sourcespace forced to abstract unitspace
			//glGetTexParameteriv(GL_TEXTURE_2D,  GL_TEXTURE_WIDTH, &currentTextureWidth);
			//glGetTexParameteriv(GL_TEXTURE_2D,  GL_TEXTURE_HEIGHT, &currentTextureHeight);
		}


		memcpy( batchCollection + batchCounter, i, sizeof( SpriteDrawInfo ) );
		batchCounter++;
		if (batchCounter>=BATCH_SIZE) flushSprites();
		i++;
		spriteCount--;
	}
	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [20]\n");
}

void GLES11SpriteBatch::flushSprites()
{
	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [21]\n");
	if (batchCounter<1) return;         // nothing to fill
	float angletointmul = (float)COSINE_TABLE_SIZE/3.14159f/2.0f;


	// Fill the inputTempMatrices
	float m[2][2];
	float posx, posy;
	GLfloat *v = vbo_vertices;
	SpriteDrawInfo *sdi = batchCollection;
	for (int f=0; f<batchCounter; f++)
	{


		int iangle = ((int)( sdi->angle * angletointmul ) & COSINE_TABLE_AND);
		m[0][0] = cosineTable[iangle] * sdi->scaleX;
		m[0][1] = cosineTable[(iangle+COSINE_TABLE_SIZE/4)&COSINE_TABLE_AND] * sdi->scaleX;
		m[1][0] = cosineTable[(iangle+COSINE_TABLE_SIZE/4)&COSINE_TABLE_AND] * sdi->scaleY;
		m[1][1] = -cosineTable[iangle] * sdi->scaleY;

		if(!sdi->manualTransformActive){
			posx = sdi->posx - (sdi->originX-0.5f)*m[0][0]*2.0f - (sdi->originY-0.5f)*m[1][0]*2.0f;
			posy = sdi->posy - (sdi->originX-0.5f)*m[0][1]*2.0f - (sdi->originY-0.5f)*m[1][1]*2.0f;
		}


		// fill the vertices
		if(!sdi->manualTransformActive){
			v[0] = posx -m[0][0] - m[1][0];
			v[1] = posy -m[0][1] - m[1][1];
		}else{
			v[0] =sdi->manualTransformMatrix[0][0];
			v[1] =sdi->manualTransformMatrix[0][1];
		}
		v[2] = 0.1f;
		v[3] = sdi->sourcex;
		v[4] = sdi->sourcey;
		memcpy( v+5, &sdi->r, sizeof( float ) * 4 );    // copy the color for this vertex.

		if(!sdi->manualTransformActive){
			v[9+0] = posx +m[0][0] - m[1][0];
			v[9+1] = posy +m[0][1] - m[1][1];
		}else{
			v[9] =sdi->manualTransformMatrix[1][0];
			v[10] =sdi->manualTransformMatrix[1][1];
		}
		v[9+2] = 0.1f;
		v[9+3] = sdi->sourcex + sdi->sourceWidth;
		v[9+4] = sdi->sourcey;
		memcpy( v+14, &sdi->r, sizeof( float ) * 4 );    // copy the color for this vertex.

		if(!sdi->manualTransformActive){
			v[18+0] = posx +m[0][0] + m[1][0];
			v[18+1] = posy +m[0][1] + m[1][1];
		}else{
			v[18] =sdi->manualTransformMatrix[2][0];
			v[19] =sdi->manualTransformMatrix[2][1];
		}
		v[18+2] = 0.1f;
		v[18+3] = sdi->sourcex + sdi->sourceWidth;
		v[18+4] = sdi->sourcey + sdi->sourceHeight;
		memcpy( v+23, &sdi->r, sizeof( float ) * 4 );    // copy the color for this vertex.

		if(!sdi->manualTransformActive){
			v[27+0] = posx -m[0][0] + m[1][0];
			v[27+1] = posy -m[0][1] + m[1][1];
		}else{
			v[27] =sdi->manualTransformMatrix[3][0];
			v[28] =sdi->manualTransformMatrix[3][1];
		}
		v[27+2] = 0.1f;
		v[27+3] = sdi->sourcex;
		v[27+4] = sdi->sourcey + sdi->sourceHeight;
		memcpy( v+32, &sdi->r, sizeof( float ) * 4 );    // copy the color for this vertex.

		v+=4*9;
		sdi++;
	}




	glBufferData(GL_ARRAY_BUFFER, batchCounter*4*9*sizeof(GLfloat), vbo_vertices, GL_DYNAMIC_DRAW );

	// do we need to do this every time
	glVertexPointer(3, GL_FLOAT, sizeof( GL_FLOAT)*9, 0);
	glTexCoordPointer( 2, GL_FLOAT, sizeof( GL_FLOAT)*9, (GLvoid*)(3 * sizeof( GLfloat) ) );
	glColorPointer( 4, GL_FLOAT, sizeof( GL_FLOAT)*9, (GLvoid*)(5 * sizeof( GLfloat) ) );

	glDrawElements( GL_TRIANGLES, 6*batchCounter, GL_UNSIGNED_SHORT, 0);
	batchCounter = 0;

	//__android_log_print(ANDROID_LOG_INFO, "adMOB","GLES11SpriteBatch [%d]\n",flushSprites_++);
}

