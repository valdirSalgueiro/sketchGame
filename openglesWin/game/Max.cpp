#include "Max.h"
#include "gmUtil.h"



const unsigned int Max_texcoords[] = {
0,0,467,475, // 0       max-helmet0033.png
0,0,467,475, // 1       max-helmet0034.png
469,0,467,475, // 2     max-helmet0030.png
469,0,467,475, // 3     max-helmet0031.png
469,0,467,475, // 4     max-helmet0032.png
0,477,467,475, // 5     max-helmet0013.png
0,477,467,475, // 6     max-helmet0014.png
0,477,467,475, // 7     max-helmet0015.png
0,477,467,475, // 8     max-helmet0016.png
0,477,467,475, // 9     max-helmet0017.png
0,477,467,475, // 10    max-helmet0018.png
0,477,467,475, // 11    max-helmet0019.png
0,477,467,475, // 12    max-helmet0020.png
469,477,467,475, // 13  max-helmet0010.png
469,477,467,475, // 14  max-helmet0011.png
469,477,467,475, // 15  max-helmet0012.png
469,477,467,475, // 16  max-helmet0021.png
469,477,467,475, // 17  max-helmet0022.png
469,477,467,475, // 18  max-helmet0023.png
0,954,467,475, // 19    max-helmet0007.png
0,954,467,475, // 20    max-helmet0008.png
0,954,467,475, // 21    max-helmet0009.png
0,954,467,475, // 22    max-helmet0024.png
0,954,467,475, // 23    max-helmet0025.png
0,954,467,475, // 24    max-helmet0026.png
469,954,467,475, // 25  max-helmet0004.png
469,954,467,475, // 26  max-helmet0005.png
469,954,467,475, // 27  max-helmet0006.png
469,954,467,475, // 28  max-helmet0027.png
469,954,467,475, // 29  max-helmet0028.png
469,954,467,475, // 30  max-helmet0029.png
0,1431,467,475, // 31   max-helmet0001.png
0,1431,467,475, // 32   max-helmet0002.png
0,1431,467,475, // 33   max-helmet0003.png
};
const unsigned int Max_sequence[] ={31,32,33,25,26,27,19,20,21,13,14,15,5,6,7,8,9,10,11,12,16,17,18,22,23,24,28,29,30,2,3,4,0,1};



Max::Max(){
	numframes=sizeof(Max_sequence)/sizeof(const unsigned int);

	loadTextures();

	loops=0;

	Animation* animation=new Animation();
	animation->currentFrame=0;
	animations.push_back(animation);	

	pos=Vector2D<float>(20,960-sprMax[0].height);

}

void Max::loadTextures(){
	glLoadSpriteset("max.png",sprMax,1024,2048,34,Max_texcoords,0);
}


bool Max::update(float time) {
	loops++;
	std::vector<Animation*>::iterator iter = animations.begin();
	while (iter != animations.end())
	{
		if(loops%4==0){
			(*iter)->currentFrame++;
		}

		if((*iter)->currentFrame>=numframes){
			(*iter)->currentFrame=0;
		}
		iter++;
	}
	return true;
}

Max::~Max(){
	std::vector<Animation*>::iterator iterAnim = animations.begin();
	while (iterAnim != animations.end())
	{
		delete * iterAnim;  
		iterAnim = animations.erase(iterAnim);
	}
}

bool Max::render(float time){
	std::vector<Animation*>::iterator iter = animations.begin();
	glSprite(pos.x+20,pos.y+20,0,&sprMax[0],false,0,1,1,0,0,0,0.5f);
	while (iter != animations.end())
	{
		glSprite(pos.x,pos.y,0,&sprMax[Max_sequence[(*iter)->currentFrame]]);
		iter++;
	}
	return true;
}


void Max::init(){
}

void Max::cleanUp(){
}