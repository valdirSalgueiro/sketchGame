#include "Engine.h"
#include "gl2d.h"
#include "gmUtil.h"
#include "Aabb.h"
#include "../sat.h"


#include <chipmunk/chipmunk_private.h>

#include <time.h>
//#include "../VideoGLSurfaceView.h"

#ifdef WIN32
//#include "../sound.h"
#else
#include "jniUtil.h"
#endif

#include "font/FTBitmapChar.h"


#include "Bullet_.h"

#include <vector>
#include "Animation.h"
#define ARRAY_SIZE(X)  		(sizeof(X)/sizeof(X[0]))

#include "Tween.h"
#include "Vector2D.h"


const char* szLetters = " !\"#&'()*,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\_аимясз";

bool touch;

int turn=0;

#include "../bulletML/object.h"
std::vector<Object*> objs;
std::vector<Object*> playerObjs;

int frames;
int direction;

glImage sprTiro[3];
glImage sprBullet[4];
glImage* point;

float elapsedTime=0.0;

int toonRamp=0;

polygon levelPolygon;

void addObj(Object* object) {
	objs.push_back(object);
}

void addPlayerObj(Object* object) {
	playerObjs.push_back(object);
}


std::vector<Animation*> animations;	


#ifndef WIN32
#include "android\log.h"
#endif	

#include "fonts\ahb.h"
#include "fonts\digit.h"

#ifdef USE_FONT
FontAtlas* m_pFontAtlas;
#endif

extern float playerX;
extern float playerY;

extern float level[];

vec levelVertices[100];

glImage intro[1];

cpFloat timeStep = 1.0/60.0;
cpSpace *space;

void Engine::init(int width_, int height_){
	if(!hasInit){
#ifndef WIN32
		__android_log_print(ANDROID_LOG_INFO, "SGS", "Engine INIT");
#endif	

		width=width_;
		height=height_;
		srand(time(NULL));

		//deinit();

		reset();

		loadTextures();

		controlBasePos.x=100;
		controlBasePos.y=380;
		controlKnobPos=controlBasePos;



		gameState=PLAYING;
		//gameState=LOGO2;
		

		levelVertices[100];
		cpVect hexagon[100];
		for(int i=0;i<54;i+=2){
			levelVertices[i/2]=v(level[i],level[i+1]);
			cpVect vector={level[i], level[i+1]};
			hexagon[i/2]=vector;

		}

		levelPolygon=new_polygon(24,levelVertices);

		//polygon a = Polygon(4, v(0,0),v(0,3),v(3,3),v(3,0)), b = Polygon(4, v(4,4),v(4,6),v(6,6),v(6,4));
		//printf("%d\n", sat(a,b)); // false

		//a = Polygon(4, v(0,0),v(0,5),v(5,4),v(3,0));  b = Polygon(4, v(4,4),v(4,6),v(6,6),v(6,4));
		//printf("%d\n", sat(a,b)); // true

		// cpVect is a 2D vector and cpv() is a shortcut for initializing them.
        cpVect gravity = cpv(0, 500.0);
        
        // Create an empty space.
        space = cpSpaceNew();
		space->iterations=10;
        cpSpaceSetGravity(space, gravity);
        
		cpShape *shape;
        shape = cpSpaceAddShape(space, cpSegmentShapeNew(space->staticBody, cpv(0,480), cpv(800,480), 0.0f));
		shape->e = 1.0f; shape->u = 1.0f;

		shape = cpSpaceAddShape(space, cpPolyShapeNew(space->staticBody, 27, hexagon, cpTransformIdentity, 1));
		//shape->e = 1.0f; shape->u = 1.0f;


		player=new Player();
	

		if(gameState==LOGO1){
#ifdef WIN32
			//		native_init("../res/SGSMovie.avi");
#else
			videoPath="/sdcard/SGSMovie.avi";
			//native_init(videoPath);
#endif
		}
		hasInit=true;
	}else{
		#ifndef WIN32
		__android_log_print(ANDROID_LOG_INFO, "SGS", "Engine REINIT");
#endif
		
		loadTextures();
	}
}

Engine::~Engine(){ 
	#ifndef WIN32
	__android_log_print(ANDROID_LOG_INFO, "SGS", "Engine deinit");
#endif	
	//deinit();
}

void Engine::loadTextures(){
#ifdef USE_FONT
	m_pFontAtlas = new FontAtlas();
	m_pFontAtlas->AddFont(ahb_ttf, ahb_ttf_size, 44, szLetters);
	m_pFontAtlas->AddFont(ahb_ttf, ahb_ttf_size, 36, szLetters);
	m_pFontAtlas->AddFont(ahb_ttf, ahb_ttf_size, 20, szLetters);
	m_pFontAtlas->AddFont(digit_ttf, digit_ttf_size, 58, szLetters);
	m_pFontAtlas->CreateAtlas();
#endif

	const unsigned int texcoords[] = {
	0,0,800,480
	};

	glLoadSpriteset("intro.png","",intro,2048,2048,1,texcoords,0);

	int tempWidth;
	int tempheight;
	toonRamp=loadTexture("toonRamp.png", tempWidth, tempheight);
}

void Engine::reset(){
	player=NULL;
	frames=0;

	touch=false;
}

void Engine::deinit(){
#ifndef WIN32
	__android_log_print(ANDROID_LOG_INFO, "SGS", "Engine DESTROY");
#endif	
	hasInit=false;
	//	destroy_();
#ifdef USE_FONT
	delete m_pFontAtlas;
#endif

	delete player;

	std::vector<Object*>::iterator iterObj = objs.begin();
	while (iterObj != objs.end())
	{
		delete* iterObj;  
		iterObj = objs.erase(iterObj);
	}

	iterObj = playerObjs.begin();
	while (iterObj != playerObjs.end())
	{
		delete* iterObj;  
		iterObj = playerObjs.erase(iterObj);
	}

	std::vector<Animation*>::iterator iterAnim = animations.begin();
	while (iterAnim != animations.end())
	{
		delete * iterAnim;  
		iterAnim = animations.erase(iterAnim);
	}

}

inline int
isLeft( vec P0, vec P1, vec P2 )
{
    return ( (P1.x - P0.x) * (P2.y - P0.y)
            - (P2.x -  P0.x) * (P1.y - P0.y) );
}


int
wn_PnPoly( vec P, vec* V, int n )
{
    int    wn = 0;    // the  winding number counter

    // loop through all edges of the polygon
    for (int i=0; i<n; i++) {   // edge from V[i] to  V[i+1]
        if (V[i].y <= P.y) {          // start y <= P.y
            if (V[i+1].y  > P.y)      // an upward crossing
                 if (isLeft( V[i], V[i+1], P) > 0)  // P left of  edge
                     ++wn;            // have  a valid up intersect
        }
        else {                        // start y > P.y (no test needed)
            if (V[i+1].y  <= P.y)     // a downward crossing
                 if (isLeft( V[i], V[i+1], P) < 0)  // P right of  edge
                     --wn;            // have  a valid down intersect
        }
    }
    return wn;
}

void Engine::update(float time){
	elapsedTime+=time;

	 cpSpaceStep(space, timeStep);

	int collision=sat(levelPolygon,Polygon(4, v(playerX-15,playerY-15),v(playerX-15,playerY+15),v(playerX+15,playerY+15),v(playerX+15,playerY-15)));
	if(collision){
		printf("%d\n", collision);
	}

	//printf("%d\n", sat(levelPolygon,Polygon(4, v(4,4),v(4,6),v(6,6),v(6,4)) ) );
	//printf("%d\n", wn_PnPoly(v(playerX,playerY),levelVertices,24) );
	
	

	player->update(time, direction);

	frames++;	
	turn++;
}

void Engine::render(float time){

	spriteBatchBegin();
	glSprite(400,240, GL2D_NO_SCALE | GL2D_CENTER,intro);
	drawLevel();
	player->render(time);
	spriteBatchEnd();

	//touch=false; 
}


void Engine::playing(float time){ 

} 

void Engine::handleInput(int type,int direction_){
	direction=direction_;
}