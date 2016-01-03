#include "Tammy.h"
#include "gmUtil.h"


const unsigned int Tammy_texcoords[] = {
0,0,325,511, // 0       shape567.png
327,0,109,37, // 1      573.png
327,39,109,36, // 2     572.png
327,77,109,36, // 3     570.png
438,0,45,33, // 4       571.png
438,0,45,33, // 5       576.png
438,0,45,33, // 6       580.png
438,0,45,33, // 7       583.png
438,0,45,33, // 8       584.png
438,0,45,33, // 9       588.png
438,0,45,33, // 10      591.png
438,0,45,33, // 11      594.png
438,39,45,33, // 12     569.png
438,39,45,33, // 13     575.png
438,39,45,33, // 14     577.png
438,39,45,33, // 15     579.png
438,39,45,33, // 16     582.png
438,39,45,33, // 17     585.png
438,39,45,33, // 18     587.png
438,39,45,33, // 19     590.png
438,39,45,33, // 20     593.png
438,39,45,33, // 21     595.png
438,77,45,33, // 22     568.png
438,77,45,33, // 23     574.png
438,77,45,33, // 24     578.png
438,77,45,33, // 25     581.png
438,77,45,33, // 26     586.png
438,77,45,33, // 27     589.png
438,77,45,33, // 28     592.png
438,77,45,33, // 29     596.png
};
const unsigned int Tammy_sequence[] ={22,12,4,23,13,5,14,24,15,6,25,16,7,8,17,26,18,9,27,19,10,28,20,11,21,29};


Tammy::Tammy(){
	numframes=sizeof(Tammy_sequence)/sizeof(const unsigned int);

	loadTextures();

	loops=0;

	Animation* animation=new Animation();
	animation->currentFrame=0;
	animations.push_back(animation);	

	pos=Vector2D<float>(1600-sprTammy[0].width/2 - 20,960-sprTammy[0].height/2);

}

Tammy::~Tammy(){
	std::vector<Animation*>::iterator iterAnim = animations.begin();
	while (iterAnim != animations.end())
	{
		delete * iterAnim;  
		iterAnim = animations.erase(iterAnim);
	}
}

void Tammy::loadTextures(){
	glLoadSpriteset("tammy.png",sprTammy,512,512,30,Tammy_texcoords,0);
}


bool Tammy::update(float time) {
	std::vector<Animation*>::iterator iter = animations.begin();
	while (iter != animations.end())
	{
		if(loops%12==0){
			(*iter)->currentFrame++;
		}

		if((*iter)->currentFrame>=numframes){
			(*iter)->currentFrame=0;
		}
		iter++;
	}

	loops++;
	return true;
}


bool Tammy::render(float time){

	glSprite(pos.x+20,pos.y+20,GL2D_CENTER,&sprTammy[0],false,0,1,1,0,0,0,0.5f);
	glSprite(pos.x,pos.y,GL2D_CENTER,&sprTammy[0]);
	std::vector<Animation*>::iterator iter = animations.begin();
	while (iter != animations.end())
	{
		glSprite(pos.x-23,pos.y-33,GL2D_CENTER,&sprTammy[Tammy_sequence[(*iter)->currentFrame]]);
		iter++;
	}
	return true;
}


void Tammy::init(){
}

void Tammy::cleanUp(){
}