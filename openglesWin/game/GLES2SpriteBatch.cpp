#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <GLES2/gl2.h>
#include "GLES2SpriteBatch.h"

#include <stdio.h>
#include <string.h>

#define STRINGIFY(A) #A

/*
Shaders for the spritebatch rendering with a color multiply
*/
const char* strSpriteBatchFragmentShaderColorMultiplty =
		STRINGIFY(
	uniform sampler2D sampler2d;
	varying lowp vec4 vertexColor;
	varying lowp vec2 texCoord;

	void main (void)
	{
	    gl_FragColor = texture2D(sampler2d, texCoord)*vertexColor;
	});

const char* strSpriteBatchVertexShaderColorMultiply =
	STRINGIFY(
	attribute highp vec3 vertex;
	uniform mediump mat4 projMatrix;
	uniform mediump mat4 im[16];        // input
	uniform mediump vec4 colorAdvancedInput;       
	varying mediump vec2 texCoord;
	varying mediump vec4 vertexColor;
	varying mediump vec4 colorAdvanced;

	void main(void)
	{
	mediump mat4 thisim = im[ int(vertex.z) ];
	highp vec2 transVertex = vec2( thisim[2][0], thisim[2][1] ) + mat2( thisim[0][0], thisim[0][1], thisim[1][0], thisim[1][1]) * (vertex.xy-vec2(thisim[2][2]-0.5, thisim[2][3]-0.5));
	gl_Position = vec4(transVertex,1,1) * projMatrix;
	vertexColor = vec4( thisim[3][0], thisim[3][1], thisim[3][2], thisim[3][3] );
	texCoord = (vertex.xy + vec2(0.5, 0.5)) * vec2(thisim[1][2], thisim[1][3]) + vec2(thisim[0][2], thisim[0][3]);
	colorAdvanced = colorAdvancedInput;
	});

/*
Shaders for the spritebatch rendering with a color multiply for alpha
*/
const char* strSpriteBatchFragmentShaderAlpha =
	STRINGIFY(
	uniform sampler2D sampler2d;
	varying lowp vec4 vertexColor;
	varying lowp vec2 texCoord;

	void main (void)
	{
	    gl_FragColor = texture2D(sampler2d, texCoord).w*vertexColor;
	});


const char* strSpriteBatchFragmentShaderColorNormal =
	STRINGIFY(
	uniform sampler2D sampler2d;
	uniform sampler2D sampler2dNormal;
	uniform mediump vec4 colorAdvanced;
	uniform mediump int hasTint;

	uniform mediump vec3 LightPos;        //light position, normalized
	//uniform vec4 LightColor;      //light RGBA -- alpha is intensity
	//uniform vec4 AmbientColor;    //ambient RGBA -- alpha is intensity 
	//uniform vec3 Falloff;         //attenuation coefficients

	varying lowp vec4 vertexColor;
	varying lowp vec2 texCoord;

	void main (void)
	{
		//lowp vec3 LightPos = vec3(0.5,0.5,0.075);       
		lowp vec4 LightColor = vec4(1, 0.8, 0.6, 1);      
		lowp vec4 AmbientColor= vec4(0.9, 0.9, 0.9, 1);  
		lowp vec3 Falloff= vec3(0.1,0.1,10);  
		//lowp vec3 Falloff= vec3(0.4,3,20);    
		lowp vec4 DiffuseColor;

		lowp vec3 NormalMap = texture2D(sampler2dNormal, texCoord).rgb;

		if(hasTint==1){
			DiffuseColor.rgba = texture2D(sampler2d, texCoord) + colorAdvanced;
		}else{
			DiffuseColor = texture2D(sampler2d, texCoord);
		}

		//The delta position of light
		lowp vec3 LightDir = vec3(LightPos.xy - (gl_FragCoord.xy / vec2(800.0,480.0)), LightPos.z);
	
		//Correct for aspect ratio
		LightDir.x *= 800.0 / 480.0;
	
		//Determine distance (used for attenuation) BEFORE we normalize our LightDir
		lowp float D = length(LightDir);
	
		//normalize our vectors
		lowp vec3 N = normalize(NormalMap * 2.0 - 1.0);
		lowp vec3 L = normalize(LightDir);
	
		//Pre-multiply light color with intensity
		//Then perform "N dot L" to determine our diffuse term
		lowp vec3 Diffuse = (LightColor.rgb * LightColor.a) * max(dot(N, L), 0.0);

		//pre-multiply ambient color with intensity
		lowp vec3 Ambient = AmbientColor.rgb * AmbientColor.a;
	
		//calculate attenuation
		lowp float Attenuation = 1.0 / ( Falloff.x + (Falloff.y*D) + (Falloff.z*D*D) );
	
		//the calculation which brings it all together
		lowp vec3 Intensity = Ambient + Diffuse * Attenuation;
		lowp vec3 FinalColor = DiffuseColor.rgb * Intensity;
		gl_FragColor = vec4(DiffuseColor.rgb * Intensity, DiffuseColor.a);
	}

	);

const char* strSpriteBatchFragmentShaderColorAdvanced =
	STRINGIFY(
	uniform sampler2D sampler2d;
	uniform mediump vec4 colorAdvanced;

	varying lowp vec4 vertexColor;
	varying lowp vec2 texCoord;

	void main (void)
	{
	lowp vec4 colorFinal = texture2D(sampler2d, texCoord) + colorAdvanced;

	gl_FragColor = colorFinal;
	});


const char* simpleVs=
	STRINGIFY(
	attribute highp vec2 vertex;
	void main(void) {
	gl_Position = vec4(vertex, 0, 1);
    });

const char* projVs=
	STRINGIFY(
	attribute highp vec3 vertex;
	uniform mediump mat4 projMatrix;

	void main(void)
	{
		highp vec2 transVertex;
		transVertex.x= vertex.x;
		transVertex.y= 480.0 - vertex.y;
		gl_Position = vec4(transVertex.xy,1,1) * projMatrix;
	});

const char* bloomFs = 
	STRINGIFY(
	uniform highp float blurX;
	uniform highp float blurY;
	uniform sampler2D sampler2d;
	varying mediump vec4 vertexColor;
	varying mediump vec2 texCoord;
	void main() {
		highp vec4 sum = vec4(0.0);
		highp vec2 dim = vec2(1024.0, 512.0);
		highp vec2 blurSize = vec2(blurX, blurY) / dim;
		highp vec2 p = gl_FragCoord.xy / dim;
	sum += texture2D(sampler2d, p - 4.0 * blurSize) * 0.05;
	sum += texture2D(sampler2d, p - 3.0 * blurSize) * 0.09;
	sum += texture2D(sampler2d, p - 2.0 * blurSize) * 0.12;
	sum += texture2D(sampler2d, p - 1.0 * blurSize) * 0.15;
	sum += texture2D(sampler2d, p                 ) * 0.16;
	sum += texture2D(sampler2d, p + 1.0 * blurSize) * 0.15;
	sum += texture2D(sampler2d, p + 2.0 * blurSize) * 0.12;
	sum += texture2D(sampler2d, p + 3.0 * blurSize) * 0.09;
	sum += texture2D(sampler2d, p + 4.0 * blurSize) * 0.05;
	gl_FragColor = sum;
	});

const char* blendFs=
	STRINGIFY(
	precision highp float;
	uniform sampler2D sampler0;
	uniform sampler2D sampler1;
	void main() {
	  float exposure = 1.5;
	  vec2 dim = vec2(1024.0, 512.0);
	  vec2 p = gl_FragCoord.xy / dim;
	  vec4 blur = texture2D(sampler0, p);
	  vec4 original = texture2D(sampler1, p);
	  vec4 color = mix(original, blur, 0.5);
	  //"p -= 0.5;"
	  //"float vignette = 1. - dot(p, p);"
	  //"color = color * vignette * vignette * vignette * vignette;"
	  color *= exposure;
	  gl_FragColor = pow(color, vec4(0.55));
	});

const char* simpleFs=
	STRINGIFY(
uniform sampler2D sampler2d;
void main(void) {
	gl_FragColor = texture2D(sampler2d, gl_FragCoord.xy/vec2(1024.0, 512.0));
});

const char* colorFs=
	STRINGIFY(

uniform sampler2D sampler2d;
uniform mediump vec3 LightPos;        //light position, normalized

void main(void) {

		lowp vec4 LightColor = vec4(1, 1, 1, 1);      
		lowp vec4 AmbientColor= vec4(1, 1, 1, 1);  
		lowp vec4 DiffuseColor;

		DiffuseColor = vec4(1,0,0,1);

		//The delta position of light
		lowp vec3 LightDir = vec3(LightPos.xy - (gl_FragCoord.xy / vec2(800.0,480.0)), LightPos.z);
	
		//Correct for aspect ratio
		LightDir.x *= 800.0 / 480.0;
	
		//Determine distance (used for attenuation) BEFORE we normalize our LightDir
		lowp float D = length(LightDir);
	
		//normalize our vectors
		lowp vec3 N = vec3(0,0,1);
		lowp vec3 L = normalize(LightDir);
		lowp float NdotL = dot (N, L);
	
		//Pre-multiply light color with intensity
		//Then perform "N dot L" to determine our diffuse term
		lowp vec3 Diffuse = (LightColor.rgb * LightColor.a) * texture2D(sampler2d,vec2(NdotL,0.0)).rgb;

		//pre-multiply ambient color with intensity
		lowp vec3 Ambient = AmbientColor.rgb * AmbientColor.a;
	
		//calculate attenuation
		//lowp float Attenuation = 1.0 / D*D;
		lowp float Attenuation = (1.0 + 0.1*D + 0.01*D*D);
		
	
		//the calculation which brings it all together
		lowp vec3 Intensity = Diffuse * Attenuation;
		lowp vec3 FinalColor = DiffuseColor.rgb * Intensity;
		gl_FragColor = vec4(FinalColor,1);
});

FrameBuffer bloomXFramebuffer;
FrameBuffer bloomYFramebuffer;
FrameBuffer backBuffer;

extern float playerX;
extern float playerY;

extern float elapsedTime;

extern int toonRamp;

extern float level[];

//extern float shakeAmount;
//extern bool shakeScreen;


GLES2SpriteBatch::GLES2SpriteBatch(int w, int h ) : SpriteBatch(w,h)
{
	for (int f=0; f<COSINE_TABLE_SIZE; f++)
		cosineTable[f] = cosf( (float)f / (float)COSINE_TABLE_SIZE * 3.14159f * 2.0f ); 

	batchCounter = 0; 

	bloomXFramebuffer = createFramebuffer();    
	bloomYFramebuffer = createFramebuffer();
	backBuffer = createFramebuffer();


	// Initialize shaders
	GLint retval;

	for (int f=0; f<9; f++) { 
		if(f==1) 
			continue; 

		fragmentShader[f] = glCreateShader(GL_FRAGMENT_SHADER);

		switch(f){
		case 0:
			glShaderSource(fragmentShader[f], 1, (const char**)&strSpriteBatchFragmentShaderColorMultiplty, NULL);
			break;
		case 2:
			glShaderSource(fragmentShader[f], 1, (const char**)&strSpriteBatchFragmentShaderAlpha, NULL);
			break;
		case 3:
			glShaderSource(fragmentShader[f], 1, (const char**)&strSpriteBatchFragmentShaderColorAdvanced, NULL);
			break;
		case 4:
			glShaderSource(fragmentShader[f], 1, (const char**)&bloomFs, NULL);
			break;
		case 5:
			glShaderSource(fragmentShader[f], 1, (const char**)&simpleFs, NULL);
			break;
		case 6:
			glShaderSource(fragmentShader[f], 1, (const char**)&blendFs, NULL);
			break;
		case 7:
			glShaderSource(fragmentShader[f], 1, (const char**)&strSpriteBatchFragmentShaderColorNormal, NULL);
			break;
		case 8:
			glShaderSource(fragmentShader[f], 1, (const char**)&colorFs, NULL);
			break;
		}


		glCompileShader( fragmentShader[f] );
		glGetShaderiv(fragmentShader[f], GL_COMPILE_STATUS, &retval);
		if (!retval)
		{
			// failed
			printf("failed to compile fshader: %d", f);
		} else {}

		vertexShader[f] = glCreateShader( GL_VERTEX_SHADER );

		switch(f){
		case 0:
			glShaderSource( vertexShader[f], 1,(const char**)&strSpriteBatchVertexShaderColorMultiply, NULL );
			break;
		case 2:
			glShaderSource( vertexShader[f], 1,(const char**)&strSpriteBatchVertexShaderColorMultiply, NULL );
			break;
		case 3:
			glShaderSource( vertexShader[f], 1,(const char**)&strSpriteBatchVertexShaderColorMultiply, NULL );
			break;
		case 4:
			glShaderSource( vertexShader[f], 1,(const char**)&simpleVs, NULL );
			break;
		case 5:
			glShaderSource( vertexShader[f], 1,(const char**)&simpleVs, NULL );
			break;
		case 6:
			glShaderSource( vertexShader[f], 1,(const char**)&simpleVs, NULL );
			break;
		case 7:
			glShaderSource( vertexShader[f], 1,(const char**)&strSpriteBatchVertexShaderColorMultiply, NULL );
			break;
					case 8:
			glShaderSource( vertexShader[f], 1,(const char**)&projVs, NULL );
			break;
		}


		glCompileShader( vertexShader[f] );
		glGetShaderiv( vertexShader[f], GL_COMPILE_STATUS, &retval );
		if (!retval) {
			// failed
			printf("failed to compile vshader: %d", f);
		} else {}

		program[f] = glCreateProgram();
		glAttachShader(program[f], fragmentShader[f] );
		glAttachShader(program[f], vertexShader[f] );
		// Bind the custom vertex attributes
		glBindAttribLocation(program[f], 0, "vertex");

		glLinkProgram( program[f] );

		// Check if linking succeeded in the same way we checked for compilation success
		glGetProgramiv(program[f], GL_LINK_STATUS, &retval);
		if (!retval) {
			// failed
			printf("failed to link program");
			GLint size = 0;
			glGetProgramiv(program[f], GL_INFO_LOG_LENGTH, &size);
			GLsizei realSize = size; // including the null terminator
			char * infoprintf = new char[size];
			GLsizei obtainedprintfSize = 0; // not including the null terminator
			glGetProgramInfoLog(program[f], realSize, &obtainedprintfSize, infoprintf);
			if (obtainedprintfSize + 1 != realSize) {
				printf("mismatch in sizes %d vs %d", realSize, obtainedprintfSize);
			}
			printf(infoprintf);
			delete [] infoprintf;
		} else {
			printf("Finished building program %d", f );
		}


		samplerLocation[f] = glGetUniformLocation(program[f], "sampler2d");
		inputMatrixLocation[f] = glGetUniformLocation(program[f], "im");
		projmLocation[f] = glGetUniformLocation(program[f], "projMatrix");
		colorLocation[f] = glGetUniformLocation(program[f], "colorAdvanced");
	}


	glUseProgram( program[0] );
	glGenBuffers(1, &vbo);

	GLfloat vertices[] = {-0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f,    // triangle 1
		-0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f, -0.5f,0.5f,0.0f};   // triangle 2

	GLfloat *tempVertices = new GLfloat[ 3 * 6 * BATCH_SIZE];
	for (int f=0; f<BATCH_SIZE; f++) {
		memcpy( tempVertices+f*3*6, vertices, 3*6*sizeof(GLfloat ));
		for (int g=0; g<6; g++) 
			tempVertices[ f*3*6+2+g*3 ] = f;            // mark the index for each triangle
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo );
	glBufferData(GL_ARRAY_BUFFER, BATCH_SIZE *6*sizeof(GLfloat)*3, tempVertices, GL_STATIC_DRAW );

	glGenBuffers(1, &buffer);
	GLfloat quad[] = {-1,-1, 1,-1, -1,1, -1, 1,  1,-1, 1, 1}; 
	glBindBuffer(GL_ARRAY_BUFFER, buffer );
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat)*3, quad, GL_STATIC_DRAW );

	glGenBuffers(1, &levelBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, levelBuffer );
	glBufferData(GL_ARRAY_BUFFER, 27*sizeof(GLfloat)*2, level, GL_STATIC_DRAW );

	delete [] tempVertices;

	glClearColor(0,0,0,0);

}

FrameBuffer GLES2SpriteBatch::createFramebuffer() {
	GLuint buffer = 0;
	glGenFramebuffers(1, &buffer);

    //bind framebuffer to texture
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    GLuint texture = createTexture();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("fodeu");

	glBindFramebuffer(GL_FRAMEBUFFER,0);


	FrameBuffer framebuffer;
	framebuffer.buffer=buffer;
	framebuffer.texture=texture;
    return framebuffer;
}

GLuint GLES2SpriteBatch::createTexture(){
	GLuint texture;
	glGenTextures(1, &texture);

	//set properties for the texture
	glBindTexture(GL_TEXTURE_2D, texture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targetWidth, targetHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, 1024, 512, 0,  GL_RGBA, GL_UNSIGNED_BYTE, 0);

   glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

GLES2SpriteBatch::~GLES2SpriteBatch()
{
	for (int f=0; f<4; f++) {
		if (program[f]) glDeleteProgram(program[f]);
		if (fragmentShader[f]) glDeleteShader(fragmentShader[f]);
		if (vertexShader[f]) glDeleteShader(vertexShader[f]);
	}


	if (vbo) glDeleteBuffers(1, &vbo );
	vbo = 0;
	for (int f=0; f<4; f++) {
		program[f] = 0;
		fragmentShader[f] = 0;
		vertexShader[f] = 0;
	}
}


void GLES2SpriteBatch::flushSprites()
{

	if (batchCounter<1) return;         // nothing to fill

	// Fill the inputTempMatrices
	float *m = inputMatrixTemp;
	float *c = colorTemp;
	SpriteDrawInfo *sdi = batchCollection;
	GLint textureNormal;
	int effect;

	float angletointmul = (float)COSINE_TABLE_SIZE/3.14159f/2.0f;

	for (int f=0; f<batchCounter; f++)
	{
		textureNormal=sdi->textureNormalHandle;
		effect=sdi->effect;

		// targetposition
		m[8+0] = sdi->posx;
		m[8+1] = sdi->posy;
		// origin
		m[8+2] = sdi->originX;
		m[8+3] = sdi->originY;

		// Destination orientation
		if (sdi->manualTransformActive)
		{
			m[0] = sdi->manualTransformMatrix[0][0];
			m[1] = sdi->manualTransformMatrix[0][1];
			m[4] = sdi->manualTransformMatrix[1][0];
			m[5] = sdi->manualTransformMatrix[1][1];
		}
		else
		{
			int iangle = ((int)( sdi->angle * angletointmul ) & COSINE_TABLE_AND);
			m[0] = cosineTable[iangle] * sdi->scaleX;
			m[1] = cosineTable[(iangle+COSINE_TABLE_SIZE/4)&COSINE_TABLE_AND] * sdi->scaleX;
			m[4+0] = cosineTable[(iangle+COSINE_TABLE_SIZE/4)&COSINE_TABLE_AND] * sdi->scaleY;
			m[4+1] = -cosineTable[iangle] * sdi->scaleY;
		}


		// Source rectangle
		m[2] = sdi->sourcex;
		m[3] = sdi->sourcey;
		m[4+2] = sdi->sourceWidth;
		m[4+3] = sdi->sourceHeight;

		m[12+0] = sdi->r;
		m[12+1] = sdi->g;
		m[12+2] = sdi->b;
		m[12+3] = sdi->a;

		c[0] = sdi->rb;
		c[1] = sdi->gb;
		c[2] = sdi->bb;
		c[3] = sdi->ab;

		m+=16;
		c+=4;
		sdi++;
	}

	if(effect & NORMAL){
		GLint tintLocation = glGetUniformLocation(program[7], "hasTint");
		GLint lightLocation = glGetUniformLocation(program[7], "LightPos");
		glUniform3f(lightLocation, playerX/1600.0, 1.0 - playerY/960.0, 0.075);

		if(effect & COLOR_ADVANCED){
			glUniform1i(tintLocation, 1);
		}else{
			glUniform1i(tintLocation, 0);
		}
		GLint samplerLocation1 = glGetUniformLocation(program[7], "sampler2dNormal");
		glUniform1i(samplerLocation1, 1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureNormal);
	}

	glUniformMatrix4fv(inputMatrixLocation[currentProgram], batchCounter,GL_FALSE, inputMatrixTemp );
	glUniform4fv(colorLocation[currentProgram],batchCounter,colorTemp);

	glDrawArrays(GL_TRIANGLES, 0, 6*batchCounter);

	glActiveTexture(GL_TEXTURE0);
	// reset batch
	batchCounter = 0;
}

void GLES2SpriteBatch::draw ( SpriteDrawInfo *sdi, int spriteCount )
{
	SpriteDrawInfo *i = sdi;
	while (spriteCount>0) {

		int selectedProgram = 0;

		if(sdi->effect & FONT){
			selectedProgram = 2;
		}
		else if(sdi->effect & NORMAL){
			selectedProgram = 7;
		}
		else if(sdi->effect & COLOR_ADVANCED){
			selectedProgram = 3;
		}

		setActiveProgram( selectedProgram );


		if (i->textureHandle != currentTexture ) {
			// Texture have been changed, flush currently collected renderingdata
			flushSprites();
			// Change our current texture and activate it.
			currentTexture = i->textureHandle;
			glBindTexture( GL_TEXTURE_2D, currentTexture );
		}


		memcpy( batchCollection + batchCounter, i, sizeof( SpriteDrawInfo ) );
		batchCounter++;
		if (batchCounter>=BATCH_SIZE) 
			flushSprites();

		if(sdi->effect & COLOR_ADVANCED)
			flushSprites();

		i++;
		spriteCount--;
	}
}

void GLES2SpriteBatch::setActiveProgram( int programIndex )
{
	if(currentProgram!=programIndex){
		flushSprites();
		glUseProgram( program[programIndex] );
		glUniform1i(samplerLocation[programIndex], 0);
		glUniformMatrix4fv(projmLocation[programIndex], 1, GL_FALSE, projectionMatrix );
		currentProgram = programIndex;
	}
}

void GLES2SpriteBatch::begin( int bmode, TransformMode dTransform, float *customProjectionMatrix)
{

	if(batchCounter!=0)
	{
		printf("Warning: SpriteBatch begin() called before end()");
	}

	currentDestinationTransform = dTransform;


	if (customProjectionMatrix) {
		// set projection
		memcpy( projectionMatrix, customProjectionMatrix, 16 * sizeof( GLfloat ));
	} else {

		// Build a new projection matrix accoring screen's width and height
		GLfloat mproj[16] = {
			2.0f/(float)targetWidth, 0, 0, -1.0f,
			0, 2.0f/(float)targetHeight, 0, -1.0f,
			0, 0, 1, 0,
			0,0, 0, 1,
		};
		
		//if(shakeScreen){

		//	mproj[3] += cos( ( elapsedTime / 0.1 ) * 2 * 3.14159 ) * shakeAmount;
		//}

		memcpy( projectionMatrix, mproj, 16 * sizeof( GLfloat ));
	}

	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glEnableVertexAttribArray(0);
	glDisable( GL_CULL_FACE );

	glBindBuffer(GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GL_FLOAT),0);

	currentTexture = 0;
	currentProgram = -1;


} 

void GLES2SpriteBatch::startBloom(){
	flushSprites();
	glBindFramebuffer(GL_FRAMEBUFFER,backBuffer.buffer);
    glClear(GL_COLOR_BUFFER_BIT);
} 

void GLES2SpriteBatch::endBloom(){
	flushSprites();

	bloom();


	glBindBuffer(GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GL_FLOAT),0);

	currentTexture = 0;
	currentProgram = -1;
} 

void GLES2SpriteBatch::end()
{
	// Draw everything that is collected.
	flushSprites();

	currentProgram=-1;
}

void GLES2SpriteBatch::drawLevel(){
	flushSprites();

	glUseProgram(program[8]);
	glBindTexture( GL_TEXTURE_2D, toonRamp );

	GLint samplerLocation = glGetUniformLocation(program[8], "sampler2D");
	glUniform1i(samplerLocation, 0);


	GLint lightLocation = glGetUniformLocation(program[8], "LightPos");
	glUniform3f(lightLocation, playerX/800.0,  1-playerY/480.0, 0.075);
	glBindBuffer(GL_ARRAY_BUFFER, levelBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glUniformMatrix4fv(projmLocation[8], 1, GL_FALSE, projectionMatrix );

	glDrawArrays(GL_TRIANGLES, 0, 27);

	glBindBuffer(GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GL_FLOAT),0);

	currentTexture = 0;
	currentProgram = -1;
}

void GLES2SpriteBatch::bloom(){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

	//bloomx
	glBindFramebuffer(GL_FRAMEBUFFER,  bloomXFramebuffer.buffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program[4]);

	GLint blurXLocation = glGetUniformLocation(program[4], "blurX");
	GLint blurYLocation = glGetUniformLocation(program[4], "blurY");

	glUniform1i(samplerLocation[4], 0);
	glUniform1f(blurXLocation, 1.0);
    glUniform1f(blurYLocation, 0.0);
	glBindTexture(GL_TEXTURE_2D, backBuffer.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//bloomy
	glBindFramebuffer(GL_FRAMEBUFFER,  bloomYFramebuffer.buffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1i(samplerLocation[4], 0);
	glUniform1f(blurXLocation, 0.0);
    glUniform1f(blurYLocation, 1.0);
	glBindTexture(GL_TEXTURE_2D, bloomXFramebuffer.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);


	//blend
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(program[6]);
	GLint samplerLocation0 = glGetUniformLocation(program[6], "sampler0");
    GLint samplerLocation1 = glGetUniformLocation(program[6], "sampler1");
	glUniform1i(samplerLocation0, 0);
    glUniform1i(samplerLocation1, 1);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bloomYFramebuffer.texture);

    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, backBuffer.texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

	glActiveTexture(GL_TEXTURE0);

	//debug
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glUseProgram(program[5]);
	//GLint samplerLocation0 = glGetUniformLocation(program[5], "sampler2d");
	//glUniform1i(samplerLocation0, 0);
 //   glBindTexture(GL_TEXTURE_2D, backBuffer.texture);
 //   glDrawArrays(GL_TRIANGLES, 0, 6);
}
