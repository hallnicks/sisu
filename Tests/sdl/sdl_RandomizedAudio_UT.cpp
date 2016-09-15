#if 0
#include "test.hpp"
#include <string>
#include <iostream>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Based on StackOverflow example I believe

namespace
{

class sdl_RandomizedAudio_UT : public context
{
	public:
		sdl_RandomizedAudio_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

Uint32 sampleRate	 = 48000;
Uint32 frameRate 	 =    60;
Uint32 floatStreamLength = 1024;// must be a power of two, decrease to allow for a lower syncCompensationFactor to allow for lower latency, increase to reduce risk of underrun
Uint32 samplesPerFrame;
Uint32 msPerFrame;   

Uint32 audioBufferLength = 48000;// must be a multiple of samplesPerFrame (auto adjusted upwards if not)
float *audioBuffer;
	
SDL_atomic_t 	audioCallbackLeftOff;
Sint32 	 	audioMainLeftOff;
Uint8 		audioMainAccumulator;

SDL_AudioDeviceID AudioDevice;
SDL_AudioSpec 	  audioSpec;

SDL_Event event;
SDL_bool running = SDL_TRUE;

} // namespace


void audioCallback( void * xUnused, Uint8 * xByteStream, int xByteStreamLength ) 
{
	float * floatStream = (float*)xByteStream ;

	Sint32 localAudioCallbackLeftOff = SDL_AtomicGet(&audioCallbackLeftOff);

	Uint32 i;
	for (i=0; i<floatStreamLength; i++)
	{
	    	floatStream[i] = audioBuffer[localAudioCallbackLeftOff];
	    	localAudioCallbackLeftOff++;
		if ( localAudioCallbackLeftOff == audioBufferLength )
		{
		      localAudioCallbackLeftOff = 0;
		}
	}

	SDL_AtomicSet(&audioCallbackLeftOff, localAudioCallbackLeftOff);
}

void init() 
{
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

  	SDL_AudioSpec want;

	SDL_zero(want);// btw, I have no idea what this is...

	want.freq = sampleRate;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = floatStreamLength;
	want.callback = audioCallback;

	AudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &audioSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	if (AudioDevice == 0) 
	{
		std::cout << "SDL_OpenAudioDevice( ) failed" << std::endl;
		exit(-1);
	}

	if (audioSpec.format != want.format) 
	{
		
		std::cout << "Couldn't get audio in float format." << std::endl;
	   	exit(-1);
	}

  	sampleRate = audioSpec.freq;
  	floatStreamLength = audioSpec.size/4;
  	samplesPerFrame = sampleRate/frameRate;
  	msPerFrame = 1000/frameRate;
  	audioMainLeftOff = samplesPerFrame*8;
  	SDL_AtomicSet(&audioCallbackLeftOff, 0);

	if (audioBufferLength % samplesPerFrame)
	{
    		audioBufferLength += samplesPerFrame-(audioBufferLength % samplesPerFrame);
	}

	audioBuffer = static_cast<float*>( malloc( sizeof(float) * audioBufferLength ) );

}

int onExit() 
{
	SDL_CloseAudioDevice(AudioDevice);
  	SDL_Quit();
}


TEST(sdl_RandomizedAudio_UT, PlaySDLAudioWithoutExceptions)
{

  	float  syncCompensationFactor = 0.0016;// decrease to reduce risk of collision, increase to lower latency
  	Sint32 mainAudioLead;
	Uint32 i;

	init( );

  	SDL_PauseAudioDevice(AudioDevice, 0);

  	for (int ii = 0; ii < 45; ++ii) 
	{
	    	for (i=0; i<samplesPerFrame; i++) 
			{ audioBuffer[audioMainLeftOff+i] = 0; }

		if (audioBuffer != NULL) 
		{
			for (int i = 0; i < audioBufferLength; ++i)
			{
				audioBuffer[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			}
		}
		
	
	    	if (audioMainAccumulator > 1) {
	      		for (i=0; i<samplesPerFrame; i++) {
	        		audioBuffer[audioMainLeftOff+i] /= audioMainAccumulator;
	      		}
	    	}
	
    		audioMainAccumulator = 0;
	
	    	audioMainLeftOff += samplesPerFrame;
	
	    	if (audioMainLeftOff == audioBufferLength) 
			{ audioMainLeftOff = 0; }

	    	mainAudioLead = audioMainLeftOff - SDL_AtomicGet(&audioCallbackLeftOff);
    
		if (mainAudioLead < 0)
			{ mainAudioLead += audioBufferLength; }

		SDL_Delay( mainAudioLead*syncCompensationFactor );
	}

	onExit();
}
#endif