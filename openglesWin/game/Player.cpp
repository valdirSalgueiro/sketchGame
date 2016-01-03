#include "Player.h"
#include "gmUtil.h"
#include "Bullet_.h"

#ifdef WIN32
#include "../sound.h"
#else
#include "jniUtil.h"
#endif

#include "../bulletML/object.h"
#include "bulletmlparser.h"
#include "bulletmlparser-tinyxml.h"

#include <chipmunk/chipmunk_private.h>

#define PLAYER_VELOCITY 500.0
#define PLAYER_GROUND_ACCEL_TIME 0.1
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY/PLAYER_GROUND_ACCEL_TIME)

#define PLAYER_AIR_ACCEL_TIME 0.25
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY/PLAYER_AIR_ACCEL_TIME)

#define JUMP_HEIGHT 100.0
#define JUMP_BOOST_HEIGHT 110.0
#define FALL_VELOCITY 225.0
#define GRAVITY 500.0

float playerX=400.0f;
float playerY=240.0f;

int explosion=0;

extern int direction;

int legsAnimationLeft=0;
int legsAnimationRight=0;

int shootAnimation=0;
int bulletAnimation=0;

int framesLeft=0;
int framesRight=0;

bool isShooting;

extern bool touch;

#ifdef WIN32
//sound* tiro;
#else
int tiro;
#endif

int jetAnimation=2;

int powerUp;

extern glImage sprBullet[4];

extern void addPlayerObj(Object* object);

cpBody *playerBody;
cpShape* playerShape;

const unsigned int sprite_texcoords[] = {
	0,0,1,1, // 0       
};

extern cpSpace *space;

static cpFloat remainingBoost = 0;
static cpBool grounded = cpFalse;
static cpBool lastJumpState = cpFalse;
static cpBool jumping = cpFalse;
int jumpState=0;


static void
SelectPlayerGroundNormal(cpBody *body, cpArbiter *arb, cpVect *groundNormal){
	cpVect n = cpArbiterGetNormal(arb);
	
	if(n.y > groundNormal->y){
		(*groundNormal) = n;
	}
}

static void
playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	float xAxis=0;
	if(direction & LEFT){
		xAxis = -1;
	}
	if(direction & RIGHT){
		xAxis = 1;
	}

	if(direction & UP){
		jumpState = 1;
	}else{
		jumpState = 0;
	}
	// Grab the grounding normal from last frame
	cpVect groundNormal = cpvzero;
	cpBodyEachArbiter(playerBody, (cpBodyArbiterIteratorFunc)SelectPlayerGroundNormal, &groundNormal);
	
	grounded = (groundNormal.y > 0.0);
	if(groundNormal.y < 0.0f) 
		remainingBoost = 0.0f;
	
	// Do a normal-ish update
	cpBool boost = (jumpState && remainingBoost > 0.0f);
	cpVect g = (boost ? cpvzero : gravity);
	cpBodyUpdateVelocity(body, g, damping, dt);
	
	// Target horizontal speed for air/ground control
	cpFloat target_vx = PLAYER_VELOCITY*xAxis;
	
	// Update the surface velocity and friction
	// Note that the "feet" move in the opposite direction of the player.
	cpVect surface_v = cpv(-target_vx, 0.0);
	playerShape->surfaceV = surface_v;
	playerShape->u = (grounded ? PLAYER_GROUND_ACCEL/GRAVITY : 0.0);
	
	// Apply air control if not grounded
	if(!grounded){
		// Smoothly accelerate the velocity
		playerBody->v.x = cpflerpconst(playerBody->v.x, target_vx, PLAYER_AIR_ACCEL*dt);
	}
	
	body->v.y = cpfclamp(body->v.y, -FALL_VELOCITY, INFINITY);
}

Player::Player(){
	std::string bulletPattern1("<bulletml><action label=\"top\"><fire><direction type=\"absolute\">90</direction><bullet><speed>3</speed></bullet></fire></action></bulletml>");
	std::string bulletPattern2("<bulletml><action label=\"top\"><fire><direction type=\"relative\">90</direction><bullet><speed>6</speed></bullet></fire><fire><direction type=\"relative\">90</direction><bulletRef label=\"curveLeft\" /></fire><fire><direction type=\"relative\">90</direction><bulletRef label=\"curveRight\" /></fire></action><bullet label=\"curveLeft\"><speed>6</speed><action><changeDirection><direction type=\"relative\">45</direction><term>5</term></changeDirection></action></bullet><bullet label=\"curveRight\"><speed>6</speed><action><changeDirection><direction type=\"relative\">-45</direction><term>5</term></changeDirection></action></bullet></bulletml>");
	std::string bulletPattern3("<bulletml><action label=\"top\"><fire><direction type=\"relative\">135</direction><bulletRef label=\"curveRight\" /></fire><fire><direction type=\"absolute\">90</direction><bullet><speed>4</speed></bullet></fire><fire><direction type=\"relative\">45</direction><bulletRef label=\"curveLeft\" /></fire></action><bullet label=\"curveLeft\"><speed>4</speed><action><wait>8</wait><changeDirection><direction type=\"relative\">90</direction><term>15</term></changeDirection></action></bullet><bullet label=\"curveRight\"><speed>4</speed><action><wait>8</wait><changeDirection><direction type=\"relative\">-90</direction><term>15</term></changeDirection></action></bullet></bulletml>");


	powerUp=0;
	isShooting=false;

	loadTextures();

#ifdef WIN32
	//	tiro=new sound("../res/tiro.wav",25,100,50);
#else
	tiro=JNIUtil::jni_sound_load("tiro.ogg");
#endif
	shot=&sprBullet[3];


#ifdef WIN32
	bps[0] = new BulletMLParserTinyXML("../res/xml/player_1.xml");
	bps[1] = new BulletMLParserTinyXML("../res/xml/player_2.xml");
	bps[2] = new BulletMLParserTinyXML("../res/xml/player_3.xml");
#else
	bps[0] = new BulletMLParserTinyXML(bulletPattern1);
	bps[1] = new BulletMLParserTinyXML(bulletPattern2);
	bps[2] = new BulletMLParserTinyXML(bulletPattern3);
#endif
	bps[0]->build();
	bps[1]->build();
	bps[2]->build();

	playerBody = cpSpaceAddBody(space, cpBodyNew(1.0f, INFINITY));
	playerBody->p = cpv(10, 100);
	playerBody->velocity_func = playerUpdateVelocity;

	playerShape = cpSpaceAddShape(space, cpBoxShapeNew(playerBody, 5, 7, 10));
	playerShape->e = 0.0f; 
	playerShape->u = 0.0f;
	playerShape->type = 1;
}

Player::~Player(){
	delete bps[0];
	delete bps[1];
	delete bps[2];
}

void Player::loadTextures(){
	glLoadTileSet("box.png",spr,512,512,1,1);

}

void Player::init(){

}

bool Player::update(float time, int direction) {
	this->direction = direction;
	loops++;

	if(direction & UP){
		jumpState = 1;
	}else{
		jumpState = 0;
	}

	if(jumpState && !lastJumpState && grounded){
		cpFloat jump_v = cpfsqrt(2.0*JUMP_HEIGHT*GRAVITY);
		playerBody->v = cpvadd(playerBody->v, cpv(0.0, -jump_v));
		
		remainingBoost = JUMP_BOOST_HEIGHT/jump_v;
	}
	
	remainingBoost -= 1.0f/60.0f;
	lastJumpState = jumpState;

	return true;
}


bool Player::render(float time){
	cpVect pos= cpBodyGetPosition(playerBody);
	cpVect vel = cpBodyGetVelocity(playerBody);
	playerX=pos.x;
	playerY=pos.y;


	printf("%4.2f %4.2f\n", playerX, playerY);//512->10*3
	glSprite(playerX,playerY,GL2D_CENTER,&spr[0],false,0,0.01953125*3,0.01953125*3);
	return true;
}

void Player::cleanUp(){
}

bool Player::collides(Vector2D<float> pos){
	return false;
}