#include "Intro.h"
#include "gmUtil.h"
#include "Tween.h"

#define DEG2RAD(LOL) (LOL*3.1415/180.0)

#include "font/PreCompile.h"
#include "font/FontAtlas.h" 
#include "font/FTBitmapFont.h"
#include "font/FTBitmapChar.h"

#ifndef WIN32
	#include "jniUtil.h"
#endif
 
const unsigned int intro_texcoords[] = {
0,0,800,480, //0,intro.png
802,0,637,98, //1,introback.png
0,482,620,569, //2,flare.png
802,100,529,221, //3,sgs0130.png
802,100,529,221, //4,sgs0131.png
1441,0,529,221, //5,sgs0112.png
1441,0,529,221, //6,sgs0113.png
1441,223,529,221, //7,sgs0074.png
1441,223,529,221, //8,sgs0075.png
1441,223,529,221, //9,sgs0076.png
1441,223,529,221, //10,sgs0077.png
1441,223,529,221, //11,sgs0078.png
1441,223,529,221, //12,sgs0079.png
1441,223,529,221, //13,sgs0080.png
1441,223,529,221, //14,sgs0081.png
1441,223,529,221, //15,sgs0082.png
1441,223,529,221, //16,sgs0083.png
1441,223,529,221, //17,sgs0084.png
1441,223,529,221, //18,sgs0085.png
1441,223,529,221, //19,sgs0086.png
1441,223,529,221, //20,sgs0087.png
1441,223,529,221, //21,sgs0088.png
1441,223,529,221, //22,sgs0089.png
1441,223,529,221, //23,sgs0090.png
1441,223,529,221, //24,sgs0091.png
1441,223,529,221, //25,sgs0092.png
1441,223,529,221, //26,sgs0093.png
1441,223,529,221, //27,sgs0094.png
1441,223,529,221, //28,sgs0095.png
1441,223,529,221, //29,sgs0096.png
1441,223,529,221, //30,sgs0097.png
1441,223,529,221, //31,sgs0098.png
1441,223,529,221, //32,sgs0099.png
1441,223,529,221, //33,sgs0100.png
1441,223,529,221, //34,sgs0101.png
1441,223,529,221, //35,sgs0102.png
1441,223,529,221, //36,sgs0103.png
1441,223,529,221, //37,sgs0104.png
1441,223,529,221, //38,sgs0105.png
1441,223,529,221, //39,sgs0106.png
1441,223,529,221, //40,sgs0107.png
1441,223,529,221, //41,sgs0108.png
1441,223,529,221, //42,sgs0109.png
1441,223,529,221, //43,sgs0110.png
1441,223,529,221, //44,sgs0111.png
0,1053,529,221, //45,sgs0065.png
0,1053,529,221, //46,sgs0138.png
0,1276,529,221, //47,sgs0063.png
0,1276,529,221, //48,sgs0064.png
0,1276,529,221, //49,sgs0136.png
0,1276,529,221, //50,sgs0137.png
0,1499,529,221, //51,sgs0062.png
0,1499,529,221, //52,sgs0135.png
0,1722,529,221, //53,sgs0061.png
0,1722,529,221, //54,sgs0134.png
622,482,529,221, //55,sgs0059.png
622,482,529,221, //56,sgs0060.png
622,482,529,221, //57,sgs0132.png
622,482,529,221, //58,sgs0133.png
1153,482,529,221, //59,sgs0057.png
1153,482,529,221, //60,sgs0058.png
622,705,529,221, //61,sgs0055.png
622,705,529,221, //62,sgs0056.png
622,705,529,221, //63,sgs0072.png
622,705,529,221, //64,sgs0073.png
622,705,529,221, //65,sgs0128.png
622,705,529,221, //66,sgs0129.png
622,705,529,221, //67,sgs0145.png
622,705,529,221, //68,sgs0146.png
1153,705,529,221, //69,sgs0053.png
1153,705,529,221, //70,sgs0054.png
1153,705,529,221, //71,sgs0070.png
1153,705,529,221, //72,sgs0071.png
1153,705,529,221, //73,sgs0126.png
1153,705,529,221, //74,sgs0127.png
1153,705,529,221, //75,sgs0143.png
1153,705,529,221, //76,sgs0144.png
622,928,529,221, //77,sgs0052.png
622,928,529,221, //78,sgs0069.png
622,928,529,221, //79,sgs0125.png
622,928,529,221, //80,sgs0142.png
1153,928,529,221, //81,sgs0051.png
1153,928,529,221, //82,sgs0068.png
1153,928,529,221, //83,sgs0124.png
1153,928,529,221, //84,sgs0141.png
622,1151,529,221, //85,sgs0050.png
622,1151,529,221, //86,sgs0067.png
622,1151,529,221, //87,sgs0123.png
622,1151,529,221, //88,sgs0140.png
622,1374,529,221, //89,sgs0049.png
622,1374,529,221, //90,sgs0066.png
622,1374,529,221, //91,sgs0122.png
622,1374,529,221, //92,sgs0139.png
622,1597,529,221, //93,sgs0047.png
622,1597,529,221, //94,sgs0048.png
622,1597,529,221, //95,sgs0120.png
622,1597,529,221, //96,sgs0121.png
622,1820,529,221, //97,sgs0045.png
622,1820,529,221, //98,sgs0046.png
622,1820,529,221, //99,sgs0118.png
622,1820,529,221, //100,sgs0119.png
1153,1151,529,221, //101,sgs0043.png
1153,1151,529,221, //102,sgs0044.png
1153,1151,529,221, //103,sgs0116.png
1153,1151,529,221, //104,sgs0117.png
1153,1374,529,221, //105,sgs0041.png
1153,1374,529,221, //106,sgs0042.png
1153,1374,529,221, //107,sgs0114.png
1153,1374,529,221, //108,sgs0115.png
1153,1597,529,221, //109,sgs0039.png
1153,1597,529,221, //110,sgs0040.png
1153,1820,529,221, //111,sgs0001.png
1153,1820,529,221, //112,sgs0002.png
1153,1820,529,221, //113,sgs0003.png
1153,1820,529,221, //114,sgs0004.png
1153,1820,529,221, //115,sgs0005.png
1153,1820,529,221, //116,sgs0006.png
1153,1820,529,221, //117,sgs0007.png
1153,1820,529,221, //118,sgs0008.png
1153,1820,529,221, //119,sgs0009.png
1153,1820,529,221, //120,sgs0010.png
1153,1820,529,221, //121,sgs0011.png
1153,1820,529,221, //122,sgs0012.png
1153,1820,529,221, //123,sgs0013.png
1153,1820,529,221, //124,sgs0014.png
1153,1820,529,221, //125,sgs0015.png
1153,1820,529,221, //126,sgs0016.png
1153,1820,529,221, //127,sgs0017.png
1153,1820,529,221, //128,sgs0018.png
1153,1820,529,221, //129,sgs0019.png
1153,1820,529,221, //130,sgs0020.png
1153,1820,529,221, //131,sgs0021.png
1153,1820,529,221, //132,sgs0022.png
1153,1820,529,221, //133,sgs0023.png
1153,1820,529,221, //134,sgs0024.png
1153,1820,529,221, //135,sgs0025.png
1153,1820,529,221, //136,sgs0026.png
1153,1820,529,221, //137,sgs0027.png
1153,1820,529,221, //138,sgs0028.png
1153,1820,529,221, //139,sgs0029.png
1153,1820,529,221, //140,sgs0030.png
1153,1820,529,221, //141,sgs0031.png
1153,1820,529,221, //142,sgs0032.png
1153,1820,529,221, //143,sgs0033.png
1153,1820,529,221, //144,sgs0034.png
1153,1820,529,221, //145,sgs0035.png
1153,1820,529,221, //146,sgs0036.png
1153,1820,529,221, //147,sgs0037.png
1153,1820,529,221, //148,sgs0038.png
802,323,439,17, //149,shape2.png
802,342,177,12, //150,shape1.png
};

const unsigned int intro_sequence[]={
	111,112,113,114,115,116,117,118,119,
	120,121,122,123,124,125,126,127,128,
	129,130,131,132,133,134,135,136,137,
	138,139,140,141,142,143,144,145,146,
	147,148,109,110,105,106,101,102,97,
	98,	93,94,89,85,81,77,69,70,61,62,59,
	60,55,56,53,51,47,48,45,90,86,82,78,
	71,72,63,64,7,8,9,10,11,12,13,14,15,
	16,17,18,19,20,21,22,23,24,25,26,27,
	28,29,30,31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,5,6,107,108,103,104,99,100,
	95,96,91,87,83,79,73,74,65,66,3,4,57,58,
	54,52,49,50,46,92,88,84,80,75,76,67,68
};


extern Tween myTween;
Tween myTween2;

#ifdef FONT
extern FontAtlas* m_pFontAtlas;
#endif

int loops=0;

int width1_;

Intro::Intro(){
#ifndef WIN32
	JNIUtil::jni_music_play("title.ogg");
#endif

	#ifdef FONT
	width1_=(m_pFontAtlas->GetFont(AHB_36)->GetWidth("TOUCH TO START"));
	#endif

	numframes=sizeof(intro_sequence)/sizeof(const unsigned int);

	loadTextures();

	loops=0;

	Animation* animation=new Animation();
	animation->currentFrame=0;
	animations.push_back(animation);	

	pos=Vector2D<float>(0,0);

	myTween.setup(120, 0.3f, -0.7f, Easing::SineEaseInOut,0,2);
	myTween.play();	
	//myTween2.setup(30, 0.5f, -0.5f, Easing::SineEaseInOut,0,2);
	//myTween2.play();	
}

Intro::~Intro(){
	std::vector<Animation*>::iterator iterAnim = animations.begin();
	while (iterAnim != animations.end())
	{
		delete * iterAnim;  
		iterAnim = animations.erase(iterAnim);
	}
}

void Intro::loadTextures(){
	glLoadSpriteset("intro.png",spr,2048,2048,151,intro_texcoords,0);
}

bool Intro::update(float time) {

	std::vector<Animation*>::iterator iter = animations.begin();
	while (iter != animations.end())
	{
		if(loops%2==0){
			(*iter)->currentFrame++;
		}

		if((*iter)->currentFrame>=numframes){
			(*iter)->currentFrame=0;
		}
		iter++;
	}

	myTween.update();
	//myTween2.update();
	loops++;

	return true;
}


bool Intro::render(float time){
	
	//printf("%f\n",myTween.num);

	glSprite(0,0,GL2D_NO_SCALE,&spr[0],false,0,1,1,1,1,1,myTween.num);	
	glSprite(0,0,GL2D_NO_SCALE,&spr[0],false,0,1,1,1,1,1,0.5);	
	
	
	glSprite(400,200,GL2D_CENTER|GL2D_NO_SCALE|GL2D_COLOR_ADVANCED,&spr[1],false,0,1.2,1.2,-24,-100,100,100,-255.0f/255.0f,-255.0f/255.0f,-84.0f/255.0f,0.0f/255.0f);
	
	glSprite(370,380,GL2D_CENTER|GL2D_NO_SCALE,&spr[149]);//text
	glSprite(400,380,GL2D_CENTER|GL2D_NO_SCALE,&spr[150]);

	std::vector<Animation*>::iterator iter = animations.begin();
	while (iter != animations.end())
	{
		glSprite(400,200,GL2D_CENTER|GL2D_NO_SCALE|GL2D_COLOR_ADVANCED,&spr[intro_sequence[(*iter)->currentFrame]],false,0,1,1,100,100,100,100,41.0f/255.0f,-5.0f/255.0f,-255.0f/255.0f,0.0f/255.0f);
		iter++;
	}
	
	glSprite(400,300,GL2D_CENTER|GL2D_NO_SCALE,&spr[2],false,0,myTween.num,myTween.num);//flare
	 
	if(loops%60<20){
		
	}else{
#ifdef FONT
		m_pFontAtlas->GetFont(AHB_36)->DrawStringShadow(400-width1_/2,400,"TOUCH TO START",0xffffff,0x0000CC);
#endif
	}

	return true;
}


void Intro::init(){
}

void Intro::cleanUp(){
}