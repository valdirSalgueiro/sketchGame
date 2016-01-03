#include "gl2d.h"
#ifdef WIN32
#include "png.h"
#else
#include "jniUtil.h"
#endif



#include "Vector2D.h"
#include "SpriteBatch.h"

int gWidth;
int gHeight;

#ifdef WIN32
 #define TEXTURE_LOAD_ERROR 0
 GLuint loadTexture(const std::string filename, int &width, int &height)
 {
   //header for testing if it is a png
   png_byte header[8];

   //open file as binary
   std::string final="../res/";
   final=final.append(filename);
   FILE *fp = fopen(final.c_str(), "rb");
   if (!fp) {
     return TEXTURE_LOAD_ERROR;
   }

   //read the header
   fread(header, 1, 8, fp);

   //test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   //create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
       NULL, NULL);
   if (!png_ptr) {
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //png error stuff, not sure libpng man suggests this.
   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }

   //init png reading
   png_init_io(png_ptr, fp);

   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);

   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);

   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;

   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);

   //update width and height based on png info
   width = twidth;
   height = theight;

   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);

   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }

   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;

   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);

   //Now generate the OpenGL texture object
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,  GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);


   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);

   return texture;
 }
#endif

SpriteBatch *spriteBatch;

void setSize(const int width, const int height )
{	
	gWidth=width;
	gHeight=height;
	//glViewport( 0, 0, width, height );
	//glOrthof( 0, width, height, 0, -1, 1 );
	spriteBatch->setTargetSize(width,height);
}

void startBatch( const int width, const int height )
{
	gWidth=width;
	gHeight=height;
	//spriteBatch = new GLES11SpriteBatch(width, height);
	spriteBatch = new GLES2SpriteBatch(width, height);
}

void glClearScreen()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void glSprite( int x, int y, int flipmode, const glImage *spr,bool rotate,const float angle, const float scale1, const float scale2,float r, float g, float b, float a, float rb, float gb, float bb, float ab)
{
	spriteBatchDraw(x,y,flipmode,spr,rotate,angle,scale1,scale2,r,g,b,a,rb,gb,bb,ab);	
}

GLuint glLoadSpriteset( const char *filename,
                        glImage *sprite,
                        int width,
                        int height,
                        const int numframes,
                        const unsigned int *texcoords,
                        const  GLuint filtermode, const int frame
                      )
{
	return glLoadSpriteset(filename, "",
                        sprite,
                        width,
                        height,
                        numframes,
                        texcoords,
                        filtermode, frame
                      );
}


GLuint glLoadSpriteset( const char *filename, const char *filenameNormal,
                        glImage *sprite,
                        int width,
                        int height,
                        const int numframes,
                        const unsigned int *texcoords,
                        const  GLuint filtermode, const int frame
                      )
{

    int i;
    GLuint textureID;
	GLuint textureNormalID;

#ifdef WIN32	
	textureID=loadTexture(filename,width,height);
	bool hasNormal=false;
	if(strcmp(filenameNormal,"")){
		hasNormal=true;
		textureNormalID=loadTexture(filenameNormal,width,height);
	}
#else
	textureID=JNIUtil::jni_load_texture(filename);
#endif

	// init sprites texture coords and texture ID
    for ( i = frame; i < numframes+frame; i++)
    {
        int j = (i-frame) * 4; // texcoords array is u_off, wid, hei

		sprite[i].textureID = textureID;
		sprite[i].textureNormalID=hasNormal?textureNormalID:-1;
        sprite[i].width = texcoords[j+2];
        sprite[i].height = texcoords[j+3];
        sprite[i].u_off = texcoords[j] / (float)width;				// set x-coord
        sprite[i].v_off = texcoords[j+1] / (float)height;			// y-coord
        sprite[i].u_width = texcoords[j+2] / (float)width;			// set x-coord
        sprite[i].v_height = texcoords[j+3] / (float)height;		// y-coord
    }

    return textureID;
}

int glLoadTileSet(  const char *filename,
				   glImage              *sprite,
				   int                  tile_wid,
				   int                  tile_hei,
                   int                  bmp_wid,
				   int                  bmp_hei	  
                 )
{
	GLuint textureID;
#ifdef WIN32	
	textureID=loadTexture(filename,bmp_wid,bmp_hei);
#else
	textureID=JNIUtil::jni_load_texture(filename);
#endif

	int i=0;
	int x, y;
	
	int calcHeight=bmp_hei/tile_hei;
	int calcWidth=bmp_wid/tile_wid;

	for (y = 0; y < calcHeight; y++)
	{
		for (x = 0; x < calcWidth; x++) 
		{
			sprite[i].width 			= tile_wid;
			sprite[i].height 			= tile_hei;
			sprite[i].u_off				= (float)(x*tile_wid)/ (float)bmp_wid;
			sprite[i].v_off				= (float)(y*tile_hei)/ (float)bmp_hei; 
			sprite[i].u_width			= (float)(tile_wid)/ (float)bmp_wid;
			sprite[i].v_height			= (float)(tile_hei)/ (float)bmp_hei; 
			sprite[i].textureID 		= textureID;
			i++;
		}
	}
	return textureID;
}

void startBloom(){
	spriteBatch->startBloom();
}

void endBloom(){
	spriteBatch->endBloom();
}

void spriteBatchBegin(int mode){
	spriteBatch->begin(mode);
}

void spriteBatchEnd(){
	spriteBatch->end();
}

void spriteBatchDestroy(){
	delete spriteBatch;
}

void drawLevel(){
	spriteBatch->drawLevel();
}


void spriteBatchDraw(int x, int y, int flipmode, const glImage *spr,bool rotate,const float angle, const float scale1, const float scale2, float r, float g, float b, float a, float rb, float gb, float bb, float ab){
    SpriteDrawInfo sdi;
	sdi.textureHandle = spr->textureID;
	sdi.textureNormalHandle = spr->textureNormalID;
	float scaleX;
	float scaleY;

	scaleX=gWidth/800.0f;
	scaleY=gHeight/480.0f;

	if(flipmode & GL2D_SCALE_TO_SCREEN){
		//scaleX=spr->width/800.0f;
		//scaleY=spr->height/480.0f;
	}

	sdi.setTargetPos( x*scaleX, gHeight-(y*scaleY));


    sdi.angle = angle;
	sdi.setScale(spr->width*scale1*scaleX,spr->height*scale2*scaleY);
    sdi.setColor(r, g, b, a);
	sdi.rb=rb;
	sdi.gb=gb;
	sdi.bb=bb;
	sdi.ab=ab;

	float bottom=(flipmode & GL2D_BOTTOM)?1:0;
	float right=(flipmode & GL2D_RIGHT)?1:0;

	if(flipmode & GL2D_CENTER){
		sdi.setOrigin(0.5,0.5);
	}
	else if(flipmode & GL2D_CENTER_HORIZONTAL){
		sdi.setOrigin(0.5,bottom);
	}
	else if(flipmode & GL2D_CENTER_VERTICAL){
		sdi.setOrigin(right,0.5);
	}
	else{
		sdi.setOrigin(right,bottom);
	}

	if(flipmode & GL2D_FONT){
		sdi.effect|=FONT;
	}

	if(flipmode & GL2D_COLOR_ADVANCED){
		sdi.effect|=COLOR_ADVANCED;
	}

	if(flipmode & GL2D_NORMAL){
		sdi.effect|=GL2D_NORMAL;
	}

	float param1;
	float param2;
	float param3;
	float param4; 

#ifdef WIN32
	param1=spr->u_off;
	if(flipmode & GL2D_FONT)
		param2=spr->v_off;
	else
		param2=-spr->v_off;	
	param3=spr->u_width;
	if(flipmode & GL2D_FONT)
		param4=spr->v_height;
	else
		param4=-spr->v_height;
#else
	param1=spr->u_off;
	param2=spr->v_off;
	param3=spr->u_width;
	param4=spr->v_height; 
#endif

	if(flipmode & GL2D_FLIP_H){
		param1=spr->u_off+spr->u_width;
		param3=-spr->u_width;
	}
	if(flipmode & GL2D_FLIP_V){
		param2=param2+param4;
		param4=-param4;
	}

	if(!(flipmode & GL2D_NO_SRC)){
		sdi.setSourceRect(param1,param2,param3,param4);
	}

    spriteBatch->draw( &sdi );
}

void glFree(){
	if(spriteBatch!=NULL)
		delete spriteBatch;
}