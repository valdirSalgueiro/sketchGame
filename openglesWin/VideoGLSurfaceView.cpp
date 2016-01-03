#include "VideoGLSurfaceView.h"

#ifndef WIN32
#include <android\log.h>
#endif
#include "game\gl2d.h"


AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVFrame *pFrame;
AVFrame *pFrameRGB;
AVPacket packet;
uint8_t *buffer;
int videoStream;
AVStream* m_video_stream;

int frameFinished;
int numBytes;
int i;
int height, width;

GLuint textures[1];
GLuint textureId;

float elapsedTime=0;
float totalTime=0;
int frameNumber=0;

void grabFrame();
void printFramesPerSecond() ;

const float movie_fps=1.0f/11.0f;
bool init=false;

bool native_gl_render(float elapsed_)
{
	if(!init){
		return false;
	}

	glImage image;
	image.width=800;
	image.height=480;
	image.textureID=textureId;

	spriteBatchDraw(0,0,GL2D_NO_SCALE|GL2D_NO_SRC,&image);

	elapsedTime+=elapsed_;
	totalTime+=elapsed_;

	bool notFinish=true;
#ifndef WIN32
	if(elapsedTime/1000000.0f>=movie_fps){		
#else
	if(elapsedTime>=movie_fps){		
#endif
		elapsedTime=0;
		notFinish=grabFrame(0);
	}	
	return notFinish;
}



bool grabFrame(float elapsed_) {	
	while(true) {
		if(av_read_frame(pFormatCtx, &packet) >= 0){

			// Is this a packet from the video stream?
			if(packet.stream_index == videoStream) {
				// Decode video frame
				//clock_t time1 = clock(); 
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
				//clock_t time2 = clock();
				//__android_log_print(ANDROID_LOG_DEBUG,"avcodec_decode_video2", "%f", ((double)(time2-time1))/1000);

				// Did we get a video frame?
				if(frameFinished) {
					struct SwsContext *pSWSContext = sws_getContext(pCodecCtx->width, pCodecCtx->height,
						pCodecCtx->pix_fmt, FRAME_X, FRAME_Y, PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0);

					sws_scale(pSWSContext, (const uint8_t * const*)pFrame->data, pFrame->linesize, 0,
						pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_X, FRAME_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, pFrameRGB->data[0]);
					//printf("Frame Number: %d\n", i);
					//__android_log_print(ANDROID_LOG_DEBUG,"Frame Number: ", "%d", i); 
					//i++;
					frameNumber++;
				}
				av_free_packet(&packet);	
				break;
			}
			else{
				av_free_packet(&packet);	
				continue;
			}
		}else{
			return false;
		}		
	}
	return true;
}

void native_init(const char * path)
{
	if(init)
		destroy_();

	pFormatCtx=NULL;
	pCodecCtx=NULL;
	pCodec=NULL;
	pFrame=NULL;
	pFrameRGB=NULL;
	buffer=NULL;
	m_video_stream=NULL;

	frameFinished=0;
	numBytes=0;
	i=0;
	height=0;
	width=0;

	textures[0]=0;
	textureId=0;

	elapsedTime=0;
	totalTime=0;
	frameNumber=0;


	init=false;
	elapsedTime=0;
	totalTime=0;
	frameNumber=0;

	// TURN ON 2D TEXTURE
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// allocate a texture name
	glGenTextures(1, &textureId);

	// BIND THE TEXTURE
	glBindTexture(GL_TEXTURE_2D, textureId);

	// SET TEXTURE PARAMS
#ifdef WIN32
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif
#ifdef WIN32
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif

	// REGISTER ALL THE CODECS AVAILABLE IN FFmpeg FOR USE
	av_register_all();

	bool isCopy;

	// LOAD FILE HEADERS
	int open = avformat_open_input(&pFormatCtx, path, NULL, 0);

#ifndef WIN32
	__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","START");
#endif

	if(open != 0) {
		// IO ERROR
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","could not open file. %d", open);
#endif
		printf( "could not open file.");
		return;
	}

	if(avformat_find_stream_info(pFormatCtx,NULL) < 0) {
		// STREAM INFO ERROR
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","could not find stream info.");
#endif
		printf( "could not find stream info.");
	}

	// FIND THE FIRST VIDEO STREAM
	videoStream = -1;
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_video_stream=pFormatCtx->streams[i];
			videoStream = i;
			break;
		}
	}
	if(videoStream == -1) {
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","didn't find a video stream.");
#endif
		printf( "didn't find a video stream.");
	}

	// POINTER TO CODEC FOR VIDEO STREAM
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	// FIND VIDEO STREAM DECODER
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec == NULL) {
		// CODEC NOT FOUND
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","could not find codec.");
#endif
		printf("could not find codec.");
	}

	// OPEN CODEC
	if(avcodec_open(pCodecCtx, pCodec) < 0) {
		// OPEN CODEC ERROR
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","could not open codec.");
#endif
		printf( "could not open codec.");
	}

	// Allocate video frame
	pFrame = avcodec_alloc_frame();

	// Allocate an AVFrame structure
	pFrameRGB = avcodec_alloc_frame();
	if(pFrameRGB == NULL) {
#ifndef WIN32
		__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","null pFrameRGB.");
#endif
		printf("null pFrameRGB.");
	}

	// Determine required buffer size and allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB24, FRAME_X, FRAME_Y);
	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, FRAME_X, FRAME_Y);
#ifndef WIN32
	__android_log_print(ANDROID_LOG_DEBUG,"LOGGGGGG","LOADING SUCCESSFUL");
#endif
	init=true;
}

void destroy_()
{
	if(init){

		if(textureId!=0)
		{
			glDeleteTextures(1,&textureId);
			textureId=0;
		}

		// Free the RGB image
		av_free(buffer);
		av_free(pFrameRGB);

		// Free the YUV frame
		av_free(pFrame);

		// Close the codec
		avcodec_close(pCodecCtx);

		// Close the video file
		avformat_close_input(&pFormatCtx);
		init=false;
	}
}
