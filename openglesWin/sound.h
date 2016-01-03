#ifndef WCSOUND_H
#define WCSOUND_H

/* Defines */
#define WCSOUND_MAX_SOUNDS 64

/*
	Supported audio formats for FMOD are apparently:
	AIFF
	ASF
	ASX
	FLAC - Lossless OSS format
	IT
	MIDI - Classic format
	MOD
	MP2
	MP3 - Common format
	OGG - Common OSS format
	S3M
	WAV - Common sound effect format
	WMA
	XM - Good tracker format
*/

/* Includes */
#include <stdio.h>

/* Classes */
class music
{
private:
	char *filename;
	int volume;
	int tempo;
	int balance;
	int leadin;
	int from_data;
public:
	/*
		Creates a new music that plays normally
	*/
	music(char *f);
	/*
		Creates a new music that playes with the given tempo, volume, and balance
	*/
	music(char *f,int v,int t,int b,int li);
	/*
		Creates a new music from stream
	*/
	music(FILE *f);
	~music();
	/*
		Gets filename
	*/
	char *get_file();
	/*
		Gets tempo
	*/
	int get_tempo();
	/*
		Gets requested volume
	*/
	int get_volume();
	/*
		Gets the requested balance
	*/
	int get_balance();
	/*
		Gets the requested leadin time
	*/
	int get_leadin();
};

class sound
{
private:
	void *ch;
	void *data;
	char *filename;
	int volume;
	int tempo;
	int balance;
	/*
		Pools sound
	*/
	void pool(char *f);
	/*
		Initializes sound
	*/
	void init(char *f,int v,int t,int b);
public:
	/*
		Creates a new sound
	*/
	sound(char *f,int v,int t,int b);
	/*
		Creates a new sound from data 
	*/
	sound(void *vp);
	/*
		Creates a new sound from stream
	*/
	sound(FILE *f);
	~sound();
	/*
		Plays this sound once
	*/
	void play();
	/*
		Gets filename
	*/
	char *get_file();
	/*
		Gets volume
	*/
	int get_volume();
	/*
		Gets tempo
	*/
	int get_tempo();
	/*
		Gets balance
	*/
	int get_balance();
};

/*
	Starts sound system
*/
extern int wcsound_start();

/*
	Ends sound system
*/
extern void wcsound_end();

/*
	Plays music resource,
	returns true if the music was actually changed and different
*/
extern int wcsound_music_play(char *name);

/*
	Plays music resource data type
*/
extern void wcsound_music_play(music *m);

/*
	Stops music, resetting it
*/
extern void wcsound_music_stop();

/*
	Handles music
*/
extern void wcsound_music_handle();

/*
	Gets the current source file
*/
extern char *wcsound_get_file();

/*
	Sets the music volume slide
*/
extern void wcsound_set_volume_slide(float fv,float dv,int t);

/*
	Memorize song (entering battle)
*/
extern void wcsound_memorize_song();

/*
	Recall song (returning from battle)
*/
extern void wcsound_recall_song();

/*
	Get current song properties
*/
extern int wcsound_get_music_volume();
extern int wcsound_get_music_tempo();
extern int wcsound_get_music_balance();
extern int wcsound_get_music_leadin();
extern char *wcsound_get_music_name();

#endif