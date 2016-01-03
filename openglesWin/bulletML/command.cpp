#include "command.h"
#include "object.h"
#include "../game/Engine.h"

#include <math.h>
#define M_PI (3.1415926)

extern float playerX;
extern float playerY;

double dtor(double x) { return x*M_PI/180; }
double rtod(double x) { return x*180/M_PI; }

BulletCommand::BulletCommand(BulletMLParser* parser, Bullet* bullet)
    : BulletMLRunner(parser)
	, bullet_(bullet)
{
	bullet_->invisible=true;
}

BulletCommand::~BulletCommand(){
}

BulletCommand::BulletCommand(BulletMLState* state, Bullet* bullet)
    : BulletMLRunner(state), bullet_(bullet)
{
	bullet_->invisible=false;
}

double BulletCommand::getBulletDirection() {
    return rtod(bullet_->d);
}

double BulletCommand::getAimDirection() {
	return rtod(M_PI-atan2(playerX-bullet_->x, playerY-bullet_->y));
}

double BulletCommand::getBulletSpeed() {
    return bullet_->s;
}

double BulletCommand::getDefaultSpeed() {
    return 1.0;
}

double BulletCommand::getRank() {
    return 0.5;
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
	if(bullet_->type!=PLAYER)
		addObj(new Shot(bullet_->x, bullet_->y, dtor(direction), speed,bullet_->type));
	else
		addPlayerObj(new Shot(bullet_->x, bullet_->y, dtor(direction), speed,bullet_->type));
}

void BulletCommand::createBullet(BulletMLState* state,
                                 double direction, double speed)
{
	if(bullet_->type!=PLAYER)
		addObj(new Bullet(state, bullet_->x, bullet_->y, dtor(direction), speed,bullet_->type));
	else
		addPlayerObj(new Bullet(state, bullet_->x, bullet_->y, dtor(direction), speed,bullet_->type));
}

int BulletCommand::getTurn() {
    return turn;
}

void BulletCommand::doVanish() {
	//puts("vanish");
    bullet_->die();
}

void BulletCommand::doChangeDirection(double direction) {
    bullet_->d = dtor(direction);
}

void BulletCommand::doChangeSpeed(double speed) {
    bullet_->s = speed;
}

void BulletCommand::doAccelX(double speedx) {
    double sx = getBulletSpeedX();
    double sy = getBulletSpeedY();
    sx = speedx;
    bullet_->d = atan2(sy, sx);
    bullet_->s = sqrt(sx*sx+sy*sy);
}

void BulletCommand::doAccelY(double speedy) {
    double sx = getBulletSpeedX();
    double sy = getBulletSpeedY();
    sy = speedy;
    bullet_->d = atan2(sy, sx);
    bullet_->s = sqrt(sx*sx+sy*sy);
}

double BulletCommand::getBulletSpeedX() {
    return bullet_->s * sin(bullet_->d);
}

double BulletCommand::getBulletSpeedY() {
    return -bullet_->s * cos(bullet_->d);
}

