#if 0
// TODO: Fix all of it!
#include "test.hpp"
#include <string>
#include <iostream>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MUS_PATH "resources/beat01.wav"

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play


// prototype for our audio callback
// see the implementation for more information
// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	
	if (audio_len ==0)
		return;
	
	len = ( len > audio_len ? audio_len : len );
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
	
	audio_pos += len;
	audio_len -= len;
}

namespace
{

class sdl_SimpleWAVMixer_UT : public context
{
	public:
		sdl_SimpleWAVMixer_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(sdl_SimpleWAVMixer_UT, PlaySDLAudioWithoutExceptions)
{

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cout << "SDL_Init failed." << std::endl;
		exit(-1); 
	}

	// local variables
	static Uint32 wav_length; // length of our sample
	static Uint8 *wav_buffer; // buffer containing our audio file
	static SDL_AudioSpec wav_spec; // the specs of our piece of music
	
	
	/* Load the WAV */
	// the specs, length and buffer of our wav are filled
	if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		std::cout << "SDL_LoadWAV failed." << std::endl;
		exit( -1 );
	}
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length
	
	/* Open the audio device */
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 )
	{
		std::cout << "SDL_OpenAudio failed." << std::endl;
		exit(-1);
	}
	
	SDL_PauseAudio( 0 );

	// wait until we're don't playing
	while ( audio_len > 0 ) 
	{
		SDL_Delay( 100 ); 
	}
	
	SDL_CloseAudio( );

	SDL_FreeWAV( wav_buffer );

	exit( -1 ); 

}

#endif
