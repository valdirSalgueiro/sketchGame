#include "object.h"
#include "command.h"

#include <stdio.h>
#include <math.h>


#include "gl2d.h"
#define M_PI (3.1415926)

extern glImage sprTiro[3];
extern glImage sprBullet[3];

extern int frames;
extern int powerUp;


Object::Object(double x0, double y0, double d0, double s0, unsigned int c,ObjectType type_)
	: x(x0), y(y0), d(d0), s(s0), col(c),type(type_)
{
	anim=0;
}

Object::~Object(){
}

bool Object::move() {
	if(type!=PLAYER){
		if(frames%5==0)
			anim++;
		if(anim>2)
			anim=0;
	}

	x += s*10 * sin(d);
	y -= s*10 * cos(d);	
	//printf("%f %f\n",x,y);
	if(x<-100 || x>1600)
		return false;
	if(y<-100 || y>960)
		return false;
	return true;
}


bool Object::draw() {
	if(type==COMMON){
		glSprite(x,y,GL2D_CENTER,&sprTiro[anim],0,0,2,2);
	}
	else if(type==BOSS){
		glSprite(x,y,GL2D_CENTER|GL2D_COLOR_ADVANCED,&sprTiro[anim],0,0,2,2,100,100,100,100,-255.0f/255.0f,64/255.0f,64/255.0f,0);
	}
	else{
		if(powerUp==0){
			glSprite(x+130,y+22,GL2D_CENTER,&sprBullet[3],true,d-M_PI/2,2,2);			
		}
		else if(powerUp==1){
			glSprite(x+130,y+22,GL2D_CENTER|GL2D_COLOR_ADVANCED,&sprBullet[3],true,d-M_PI/2,2,2,100,100,100,100,0,-127.0f/255.0f,-179.0f/255.0f);			
		}else{
			glSprite(x+130,y+22,GL2D_CENTER|GL2D_COLOR_ADVANCED,&sprBullet[3],true,d-M_PI/2,2,2,100,100,100,100,-36.0f/255.0f,-189.0f/255.0f,-179.0f/255.0f);		
		}

	}

	return true;
}

Shot::Shot(double x, double y, double d, double s,ObjectType type)
	: Object(x, y, d, s, 0,type)
{}

Shot::~Shot(){

}


Bullet::Bullet(class BulletMLState* state,
	double x, double y, double d, double s,ObjectType type)
	: Object(x, y, d, s, 0,type)
	,command_(new BulletCommand(state, this)), alive_(true)
{}

Bullet::Bullet(class BulletMLParser* parser,
	double x, double y, double d, double s,ObjectType type)
	: Object(x, y, d, s, 0,type),
	command_(new BulletCommand(parser, this)), alive_(true)
{}

Bullet::~Bullet(){
	delete command_;
}

bool Bullet::move() {
	if (alive_) {
		command_->run();

		if(!Object::move())
			die();
	}
	return true;
}

bool Bullet::draw() {
	if (alive_) {
		//if(!invisible)
		Object::draw();
	}
	return !invisible && alive_;
}
