/*
	Audio System (FMOD)
*/

/* Includes */
#include <fmod.h>
#include <fmod.hpp>
#include <memory.h>
#include <malloc.h>
#include <string.h>
#include "sound.h"
#include <vector>

/* Local struct */
typedef struct
{
	char *filename;
	FMOD::Sound *snd;
}sound_entry;

/* Globals */
char *fmod_source_file = 0;
FMOD::System *fmod_system;
FMOD::System *fmod_sound_system;
unsigned int fmod_version;
FMOD::Sound *fmod_music_sound;
FMOD::Channel *fmod_music_channel;
int fmod_music_volume = 100;
int fmod_music_tempo = 100;
int fmod_music_balance = 50;
int fmod_music_active = 0;
int fmod_music_leadin = 0;
float fmod_music_frequency;
FMOD_SOUND_TYPE fmod_music_type;
FMOD_REVERB_PROPERTIES fmod_music_reverb = FMOD_PRESET_CONCERTHALL;
FMOD_REVERB_PROPERTIES fmod_no_music_reverb = FMOD_PRESET_OFF;
std::vector<sound_entry*> fmod_sound_list;
int fmod_channel_rotate = 0;
float fmod_slide_start = 1.0f;
float fmod_slide_end = 1.0f;
float fmod_slide_speed = 0.0f;
float fmod_slide_step = 0.0f;
music *wcsound_old_song = 0;
music *wcsound_current_song = 0;



/* Memorize old song (for battle entry ONLY) */
void wcsound_memorize_song()
{
	wcsound_old_song = wcsound_current_song;
}

/* Recall old song */
void wcsound_recall_song()
{
	wcsound_music_play(wcsound_old_song);
}

/* Set music volume slide */
void wcsound_set_volume_slide(float fv,float dv,int t)
{
	/* Infinite time */
	if(t == 0)
	{
		fmod_slide_start = dv;
		fmod_slide_end = dv;
		fmod_slide_step = 0.0f;
		fmod_slide_speed = 0.0f;
		return;
	}
	/* Set as current */
	fmod_slide_start = fv;
	fmod_slide_end = dv;
	fmod_slide_step = 0.0f;
	fmod_slide_speed = 64.0f/((float)t);
}

/* Sound list cleanup */
void wcsound_delete_sound_entry(void *v)
{
	sound_entry *se;
	/* Release string key */
	se = (sound_entry*)v;
	free(se->filename);
	/* Release FMOD portion */
	se->snd->release();
}

/* Source file */
char *wcsound_get_file()
{
	return fmod_source_file;
}

/* Start */
int wcsound_start()
{
	/* Create the system */
	if(FMOD::System_Create(&fmod_system) != FMOD_OK)
	{
		puts("Could not create FMOD system");
		return 0;
	}
	if(FMOD::System_Create(&fmod_sound_system) != FMOD_OK)
	{
		puts("Could not create FMOD sound system");
		return 0;
	}
	/* Quiery version */
	if(fmod_system->getVersion(&fmod_version) == FMOD_OK)
	{
		/* Check error */
		if(fmod_version < FMOD_VERSION)
		{
			printf("Improper FMOD version (%d)",fmod_version);
			return 0;
		}
		/* Announce version */
		puts("FMOD version ");
		puts(" activated.");
	}
	else
	{
		/* Announce strangeness */
		puts("Could not determine FMOD version.");
	}
	/* Initialize system */
	if(fmod_system->init(4,FMOD_INIT_NORMAL,0) != FMOD_OK)
	{
		puts("Could not initialize FMOD system");
		return 0;
	}
	if(fmod_sound_system->init(WCSOUND_MAX_SOUNDS,FMOD_INIT_NORMAL,0) != FMOD_OK)
	{
		puts("Could not initialize FMOD sound system");
		return 0;
	}
	return 1;
}

/* End */
void wcsound_end()
{
	/* Delete list */
	fmod_sound_list.clear();
	/* Release fmod */
	free(fmod_source_file);
	fmod_sound_system->release();
	fmod_system->release();
}

/* Adjusts the currently playing song */
void wcsound_music_adjust(char *name)
{
	float bl;
	/* Can't adjust if not active */
	if(!fmod_music_active)
		return;
	/* Set the volume */
	if(fmod_music_channel->setVolume((float)(fmod_music_volume)/100.0f) != FMOD_OK)
	{
		/* Come on */
		printf("Could not adjust the volume of %s",name);
		return;
	}
	/* Set the tempo */
	fmod_music_sound->getFormat(&fmod_music_type,0,0,0);
	if(fmod_music_type == FMOD_SOUND_TYPE_MIDI)
	{
		/* Set the reverb */
		if(fmod_system->setReverbProperties(&fmod_music_reverb) != FMOD_OK)
		{
			/* Can't do this */
			puts("Could not enable reverb effect");
			return;
		}
		/* Actually speed up or down */
		if(fmod_music_sound->setMusicSpeed((float)(fmod_music_tempo)/100.0f) != FMOD_OK)
		{
			/* No, really? */
			printf("Could not adjust the MIDI tempo of %s",name);
			return;
		}
	}
	else
	{
		/* Set the reverb */
		if(fmod_system->setReverbProperties(&fmod_no_music_reverb) != FMOD_OK)
		{
			/* Can't do this */
			puts("Could not disable reverb effect");
			return;
		}
		/* Do a very annoying music HZ shift */
		fmod_music_channel->getFrequency(&fmod_music_frequency);
		if(fmod_music_channel->setFrequency(fmod_music_frequency*((float)(fmod_music_tempo)/100.0f)) != FMOD_OK)
		{
			/* You've got to be kidding me */
			printf("Could not adjust the tempo of %s",name);
			return;
		}
	}
	/* Adjust its balance */
	bl = (float)((fmod_music_balance-50)/50.0f);
	if(fmod_music_channel->setPan(bl) != FMOD_OK)
	{
		printf("Could not set balance of %s",name);
		return;
	}
}

/* Starts playing a music file */
int wcsound_music_play(char *name)
{
	FMOD_CREATESOUNDEXINFO info;
	char *fname;
	/* Compare names */
	if(fmod_source_file)
	{
		if(!strcmp(name,fmod_source_file)) /* Song already playing */
		{
			puts("Continuing to play ");
			puts(name);
			puts("...");
			return 0;
		}
		else
		{
			puts(name);
			puts(" is different from ");
			puts(fmod_source_file);
		}
	}
	/* Copy names */
	free(fmod_source_file);
	fmod_source_file = name;
	/* Stop the music instead? */
	if(!strcmp(name,"(OFF)"))
	{
		wcsound_music_stop();
		return 1;
	}
	/* Request */
	fname = name;
	puts("Loading ");
	puts(fname);
	/* Prepare info */
	memset(&info,0,sizeof(info));
	info.cbsize = sizeof(info);
	/* Stop old music */
	if(fmod_music_sound)
		wcsound_music_stop();
	/* Attempt to play music */
	if(fmod_system->createSound(name,FMOD_SOFTWARE|FMOD_CREATESTREAM,&info,&fmod_music_sound) != FMOD_OK)
	{
		/* Bah */
		printf("Could not load %s",name);
		return 0;
	}
	if(fmod_system->playSound((FMOD_CHANNELINDEX)0,fmod_music_sound,1,&fmod_music_channel) != FMOD_OK)
	{
		/* Bleh */
		printf("Could not prepare %s",name);
		return 0;
	}
	/* Make it loop */
	if(fmod_music_sound->setMode(FMOD_LOOP_NORMAL) != FMOD_OK)
	{
		/* Gah! */
		printf("Could not set mode for %s",name);
		return 0;
	}
	/* Active */
	fmod_music_active = 1;
	/* Adjust the music */
	wcsound_music_adjust(name);
	/* Fire! */
	if(fmod_music_channel->setPaused(0) != FMOD_OK)
	{
		/* No! */
		printf("Could not play %s",name);
		return 0;
	}
	/* Announce */
	puts("Now playing ");
	puts(name);
	return 1;
}

/* Handles slides and things */
void wcsound_music_handle()
{
	float dv,cv;
	/* Songs must be playing */
	if(!fmod_music_active)
		return;
	/* Figure out current volume */
	dv = fmod_slide_end-fmod_slide_start;
	cv = fmod_slide_start+dv*fmod_slide_step;
	/* Slide the volume */
	if(fmod_music_channel->setVolume((float)(fmod_music_volume)/100.0f*cv) != FMOD_OK)
	{
		/* Come on */
		puts("Could not slide the volume of current song");
		return;
	}
	/* Advance step */
	fmod_slide_step += 1/60.0f*fmod_slide_speed;
}

/* Play music */
void wcsound_music_play(music *m)
{
	/* Do nothing */
	if(!m)
		return;
	/* Memorize */
	wcsound_current_song = m;
	/* Play music */
	fmod_music_volume = m->get_volume();
	fmod_music_tempo = m->get_tempo();
	fmod_music_balance = m->get_balance();
	/* Adjust before if continuing same music with different settings */
	wcsound_music_adjust(m->get_file());
	/* Set leadin */
	if(m->get_leadin() != 0)
		wcsound_set_volume_slide(0.0f,1.0f,m->get_leadin());
	else
		wcsound_set_volume_slide(1.0f,1.0f,0);
	fmod_music_leadin = m->get_leadin();
	wcsound_music_play(m->get_file());
}

/* Get volume */
int wcsound_get_music_volume()
{
	return fmod_music_volume;
}

/* Get tempo */
int wcsound_get_music_tempo()
{
	return fmod_music_tempo;
}

/* Get balance */
int wcsound_get_music_balance()
{
	return fmod_music_balance;
}

/* Get song name */
char *wcsound_get_music_name()
{
	return wcsound_current_song->get_file();
}

/* Get leadin */
int wcsound_get_music_leadin()
{
	return fmod_music_leadin;
}

/* Stop music */
void wcsound_music_stop()
{
	/* Release sound */
	if(fmod_music_sound)
	{
		fmod_music_sound->release();
		fmod_music_sound = 0;
		/*free(fmod_source_file);
		fmod_source_file = 0;*/
	}
	/* Ensure end */
	fmod_music_active = 0;
}

/* Create music */
music :: music(char *f)
{
	/* Set */
	filename = f;
	volume = 100;
	tempo = 100;
	from_data = 0;
}

/* Create music */
music :: music(char *f,int v,int t,int b,int li)
{
	/* Set */
	filename = f;
	volume = v;
	tempo = t;
	leadin = li;
	balance = b;
	from_data = 0;
}

/* Delete music */
music :: ~music()
{
	if(from_data)
		free(filename);
}

/* Get filename */
char *music :: get_file()
{
	return filename;
}

/* Get volume */
int music :: get_volume()
{
	return volume;
}

/* Get tempo */
int music :: get_tempo()
{
	return tempo;
}

/* Get balance */
int music :: get_balance()
{
	return balance;
}

/* Get leadin */
int music :: get_leadin()
{
	return leadin;
}

/* Pool sound */
void sound :: pool(char *f)
{
	int i;
	sound_entry *se;
	/* Stop */
	if(!strcmp(f,"(OFF)"))
		return;
	/* Has data? */
	if(data)
		return;
	/* Log */
	puts("Pooling ");
	puts(f);
	puts(" for first time use.");
	/* Pools the sound */
	for(i = 0;i < fmod_sound_list.size();i++)
	{
		/* Get sound */
		se = (sound_entry*)fmod_sound_list.at(i);
		/* Sound is a match? */
		if(!strcmp(se->filename,filename))
		{
			/* Set */
			data = se;
			/* Log */
			puts("Used an existing sound resource.");
			return;
		}
	}
	/* Not found, add new */
	se = (sound_entry*)malloc(sizeof(sound_entry));
	se->filename = filename;
	/* Create the sound */
	if(fmod_sound_system->createSound(se->filename,FMOD_DEFAULT,0,&se->snd) != FMOD_OK)
	{
		printf("Could not create sound %s",f);
		return;
	}
	/* Add it to the list */
	fmod_sound_list.push_back(se);
	/* Now I have data */
	data = se;
	/* Done */
	puts("Loaded the sound.");
}

/* Initialize sound */
void sound :: init(char *f,int v,int t,int b)
{
	/* Get settings */
	ch = 0;
	filename = f;
	volume = v;
	tempo = t;
	balance = b;
	data = 0;
}

/* Delete sound */
sound :: ~sound()
{
	free(filename);
}

/* Make sound */
sound :: sound(char *f,int v,int t,int b)
{
	/* Pass through */
	init(f,v,t,b);
}



/* Play sound once */
void sound :: play()
{
	float hz,bl;
	FMOD::Channel *chan;
	chan = (FMOD::Channel*)ch;
	/* Attempt to pool */
	pool(filename);
	/* No data? */
	if(!data)
		return;
	/* Stop old */
	if(chan)
		chan->stop();
	/* Prepare the sound to play */
	if(fmod_sound_system->playSound((FMOD_CHANNELINDEX)fmod_channel_rotate,((sound_entry*)data)->snd,1,&chan) != FMOD_OK)
	{
		printf("Could not play sound %s",((sound_entry*)data)->filename);
		return;
	}
	/* Adjust its volume */
	if(chan->setVolume((float)(volume)/100.0f) != FMOD_OK)
	{
		printf("Could not set volume of sound %s",((sound_entry*)data)->filename);
		return;
	}
	/* Adjust its tempo */
	if(chan->getFrequency(&hz) != FMOD_OK)
	{
		printf("Could not get frequency of sound %s",((sound_entry*)data)->filename);
		return;
	}
	hz *= (float)(tempo)/100.0f;
	if(chan->setFrequency(hz) != FMOD_OK)
	{
		printf("Could not set frequency of sound %s",((sound_entry*)data)->filename);
		return;
	}
	/* Adjust its balance */
	bl = (float)((balance-50)/50.0f);
	if(chan->setPan(bl) != FMOD_OK)
	{
		printf("Could not set balance of sound %s",((sound_entry*)data)->filename);
		return;
	}
	/* Let it go */
	if(chan->setPaused(0) != FMOD_OK)
	{
		printf("Could not play sound after applying settings for sound %s",((sound_entry*)data)->filename);
		return;
	}
	/* Rotate */
	fmod_channel_rotate = (fmod_channel_rotate+1)%(WCSOUND_MAX_SOUNDS/2);
}

/* Get file */
char *sound :: get_file()
{
	return filename;
}

/* Get volume */
int sound :: get_volume()
{
	return volume;
}

/* Get tempo */
int sound :: get_tempo()
{
	return tempo;
}

/* Get balance */
int sound :: get_balance()
{
	return balance;
}